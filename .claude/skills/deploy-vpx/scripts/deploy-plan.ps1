<#
.SYNOPSIS
  Resolve a GREEN fork CI run, pull the 3 windows-x64 Release app exes (BGFX / GL / DX9),
  verify each actually contains the real Ball History implementation (not the no-op stub),
  and print a deploy plan for the VPM cabinet.

.DESCRIPTION
  This is the read-only "preview" for deploy-vpx (the analogue of status-vpx's status.sh and
  sync-vpx's sync-plan.sh). Without -Deploy it touches nothing on the cabinet: it only resolves
  the run, downloads + stages the artifacts, verifies Ball History is compiled in, and prints the
  src->dst mapping. Review that plan, then re-run with -Deploy to perform the copy (after closing
  VPX on the cabinet — see the SKILL.md checkpoint).

  Deploying is outward-facing and hard to undo (it overwrites the exe the cabinet runs), so the
  two-step "plan then -Deploy" split is deliberate: it gives Gary a checkpoint to review before
  anything on the cabinet changes.

.PARAMETER Branch
  Branch whose latest GREEN run to deploy (default: integration — the build/test branch).
  Ignored if -RunId is given.

.PARAMETER RunId
  Deploy a specific CI run instead of the latest green run on -Branch. The script still refuses
  to proceed unless that run's conclusion is 'success'.

.PARAMETER Deploy
  Actually copy the staged exes to the cabinet. Omit for a dry-run plan (the safe default).

.PARAMETER Repo
  Fork repo (default: garybrowndev/vpinball). gh must be authed on the personal account —
  the EMU account (gary-brown_bplogix) can't read the fork.

.PARAMETER CabinetDir
  Cabinet deploy folder as seen from the dev box (default: 'Z:\visual pinball').

.EXAMPLE
  ./deploy-plan.ps1 -Branch integration            # dry-run: stage + verify + print plan
.EXAMPLE
  ./deploy-plan.ps1 -Branch integration -Deploy     # perform the copy after reviewing
.EXAMPLE
  ./deploy-plan.ps1 -RunId 28481960501 -Deploy       # deploy a specific known-green run
#>
param(
  [string]$Branch = 'integration',
  [string]$RunId,
  [switch]$Deploy,
  [string]$Repo = 'garybrowndev/vpinball',
  [string]$CabinetDir = 'Z:\visual pinball'
)

$ErrorActionPreference = 'Stop'

function Info($m) { Write-Host $m }
function Warn($m) { Write-Host "WARN: $m" -ForegroundColor Yellow }
function Die($m)  { Write-Host "ERROR: $m" -ForegroundColor Red; exit 1 }

# --- gh account check (fork reads need the personal account) ------------------------------------
$acct = (gh auth status 2>&1 | Select-String 'Active account: true' -Context 1,0).Context.PreContext
if ($acct -and ($acct -join ' ') -notmatch 'garybrowndev') {
  Warn "gh active account is not garybrowndev — fork artifact download may 401. Run: gh auth switch --user garybrowndev"
}

# --- Resolve the run --------------------------------------------------------------------------
if (-not $RunId) {
  Info "Resolving latest GREEN run on '$Branch'..."
  $run = gh run list --repo $Repo --branch $Branch --limit 15 `
           --json databaseId,headSha,status,conclusion,workflowName,createdAt |
         ConvertFrom-Json |
         Where-Object { $_.status -eq 'completed' -and $_.conclusion -eq 'success' } |
         Select-Object -First 1
  if (-not $run) { Die "No completed+green run found on '$Branch'. Is CI still running, or red? (deploy-vpx never ships a non-green build.)" }
  $RunId = $run.databaseId
  Info "  -> run $RunId ($($run.headSha.Substring(0,9)), $($run.workflowName))"
} else {
  $run = gh run view $RunId --repo $Repo --json databaseId,headSha,status,conclusion,workflowName | ConvertFrom-Json
  if ($run.status -ne 'completed' -or $run.conclusion -ne 'success') {
    Die "Run $RunId is status=$($run.status) conclusion=$($run.conclusion) — refusing. deploy-vpx only ships GREEN runs (its artifacts + build are only trustworthy once the run succeeds)."
  }
  Info "Using run $RunId ($($run.headSha.Substring(0,9))) — verified GREEN."
}
$sha = $run.headSha.Substring(0,9)

# --- Enumerate the 3 windows-x64 Release app artifacts ----------------------------------------
# DX9 prefix 'VPinballX-<digit>' (bare); BGFX 'VPinballX_BGFX-'; GL 'VPinballX_GL-'. Exclude deps.
$allArts = (gh api "repos/$Repo/actions/runs/$RunId/artifacts" --paginate -q '.artifacts[].name' 2>$null) -split "`n" | Where-Object { $_ }
$want = @(
  @{ key='DX9';  pat='^VPinballX-\d.*windows-x64-Release\.zip$';   exe='VPinballX64.exe';      dst='VPinballX64_BH.exe' },
  @{ key='BGFX'; pat='^VPinballX_BGFX-.*windows-x64-Release\.zip$'; exe='VPinballX_BGFX64.exe'; dst='VPinballX_BGFX64_BH.exe' },
  @{ key='GL';   pat='^VPinballX_GL-.*windows-x64-Release\.zip$';   exe='VPinballX_GL64.exe';   dst='VPinballX_GL64_BH.exe' }
)
foreach ($w in $want) {
  $w.artifact = $allArts | Where-Object { $_ -match $w.pat } | Select-Object -First 1
  if (-not $w.artifact) { Die "Could not find the $($w.key) windows-x64-Release artifact on run $RunId. Artifacts present:`n  $($allArts -join "`n  ")" }
}

# --- Download to a FRESH temp dir (never Remove-Item a shared path — the sandbox blocks it,
#     and a stale-dir glob once destroyed a live file; see CLAUDE.md) ---------------------------
$stage = Join-Path $env:TEMP "vpx-deploy-$RunId"
if (-not (Test-Path $stage)) { New-Item -ItemType Directory -Force $stage | Out-Null }
foreach ($w in $want) {
  $outDir = Join-Path $stage $w.key
  if (-not (Test-Path (Join-Path $outDir $w.exe))) {
    Info "downloading $($w.key): $($w.artifact) ..."
    gh run download $RunId --repo $Repo -n $w.artifact --dir $outDir 2>&1 | Out-Null
  }
  $found = Get-ChildItem $outDir -Recurse -Filter $w.exe -ErrorAction SilentlyContinue | Select-Object -First 1
  if (-not $found) { Die "Artifact $($w.artifact) did not contain $($w.exe)." }
  $w.src = $found.FullName
}

# --- Verify the real Ball History impl is compiled in (stub lacks these strings) ---------------
# This is the load-bearing safety net: the stub compiles when __BALLHISTORY_WIN32__ is missing
# (as happened when upstream's build restructure dropped the define), and a stubbed exe looks
# fine until you press V and nothing happens. Refuse to deploy a stub.
foreach ($w in $want) {
  $txt = [System.Text.Encoding]::ASCII.GetString([System.IO.File]::ReadAllBytes($w.src))
  $w.hasBH = ($txt -match 'Trainer' -and $txt -match 'RunRecord')
  $w.mb    = [math]::Round((Get-Item $w.src).Length/1MB,1)
}

# --- Print the plan ---------------------------------------------------------------------------
Info ""
Info "=== Deploy plan (run $RunId @ $sha -> $CabinetDir) ==="
$want | ForEach-Object {
  "{0,-5} {1,-22} {2,6} MB  BH={3,-5} -> {4}" -f $_.key, $_.exe, $_.mb, $_.hasBH, (Join-Path $CabinetDir $_.dst)
} | ForEach-Object { Info $_ }

$stub = $want | Where-Object { -not $_.hasBH }
if ($stub) { Die "$($stub.key -join ', ') exe(s) have Ball History STUBBED OUT (no Trainer/RunRecord). Do NOT deploy — check __BALLHISTORY_WIN32__ is defined in CMakeLists.txt and rebuild. See CLAUDE.md." }

# --- Cabinet reachability ----------------------------------------------------------------------
$reachable = Test-Connection -TargetName virtualpin -Count 1 -Quiet -ErrorAction SilentlyContinue
$zmapped   = Test-Path $CabinetDir
Info ""
Info "cabinet 'virtualpin' reachable: $reachable ; '$CabinetDir' mapped: $zmapped"
if (-not $zmapped) { Warn "Z: not mapped. Remap: net use Z: \\virtualpin\c$ /persistent:yes  (fallback host: 192.168.1.31)" }

if (-not $Deploy) {
  Info ""
  Info "DRY RUN — nothing copied. Review the plan above, then re-run with -Deploy (close VPX on the cabinet first)."
  return
}

# --- Deploy (only with -Deploy) ---------------------------------------------------------------
if (-not $reachable) { Die "Cabinet not reachable — cannot deploy." }
if (-not $zmapped)   { Die "$CabinetDir not mapped — cannot deploy." }
Info ""
Info "=== Deploying (VPX must be closed on the cabinet or the copy fails 'file in use') ==="
foreach ($w in $want) {
  $dst = Join-Path $CabinetDir $w.dst
  Copy-Item $w.src $dst -Force -ErrorAction Stop
  $now = Get-Item $dst
  Info "  {0,-5} -> {1}  ({2} MB, {3})" -f $w.key, $dst, [math]::Round($now.Length/1MB,1), $now.LastWriteTime
}
Info ""
Info "Deployed. Default launcher (VPinballX.exe symlink) points at whatever renderer it was set to;"
Info "have Gary launch a table and confirm Ball History (press V) + flipper menu nav work."
