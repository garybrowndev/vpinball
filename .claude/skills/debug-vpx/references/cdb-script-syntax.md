# cdb script syntax — gotchas earned the hard way

## Loading a script with `-c "$<file"`

The right way to feed a multi-line breakpoint script to cdb:

```
cdb.exe -G -lines -y "..." -logo C:\Dumps\trace.log -c "$<C:\Dumps\my-bp.txt" <exe>
```

- `$<file` reads the file as a sequence of cdb commands, one per line
- `$$<file`, `$$><file` and `$><file` differ in semicolon handling — for our purposes `$<file` is fine because we put one command per line
- `-cf <file>` is documented as equivalent but failed silently for us; **use `-c "$<file"` instead**

## CRITICAL: drop `-g` when loading a script

`-g` tells cdb to skip the **initial** breakpoint that fires when the launched process is created. **`-c` commands run AT that initial breakpoint.** If `-g` is set, the initial breakpoint is suppressed and `-c` never executes.

For "launch under cdb with breakpoints":
- ✅ `cdb -G -c "$<file" <exe>` — script runs, breakpoints arm, `g` in script resumes
- ❌ `cdb -g -G -c "$<file" <exe>` — script silently doesn't run

For "launch under cdb to catch first-chance exceptions and quit":
- ✅ `cdb -g -G -c "sxe av; sxe c0000005; g" <exe>` — both flags fine here because we want fast-fail-on-exception, not script execution

## Breakpoint command syntax

A breakpoint with an associated command list runs the commands every time the bp hits. Use `gc` (go conditional / continue) at the end so execution resumes after the dump:

```
bu clr!Foo::Bar ".echo === HIT ===; ? @$tid; kbn 30; gc"
```

Quoting is the painful part:
- The outer `"..."` is the cdb command argument
- Inside, semicolons separate commands run on hit
- DO NOT nest unescaped quotes — cmd.exe will eat them. Either use a `$<file` script, or escape with `\"` in the calling shell.

Most robust: put the bp script in a file and load via `$<`. Avoids three layers of shell quoting (PowerShell → PAExec → cmd.exe → cdb).

## MASM expression syntax pitfalls

cdb's default expression evaluator is MASM, not C. This means:
- `&&` does NOT work for boolean AND. Use `.and` keyword or split into nested `.if`.
- `==` works but `!=` does NOT. Use `!=` only inside C++-eval `@@c++(...)`.
- `@$tid`, `@$csp`, `@rip` etc. are pseudo-registers (note the `@`).
- Hex literals: `0x12345678` works but bare `12345678` is parsed as decimal.

When you see `Numeric expression missing from '...'` or `Bad register error at '...'`, it's almost always MASM expression syntax. Switch to `@@c++(condition)` for C-style evaluation:

```
bu Foo::Bar /w "@@c++(thisptr->member == nullptr)" "kbn 30; gc"
```

## Useful one-liners

```
~                              ; list threads (summary)
~* kbn 30                      ; stack of every thread, 30 frames
~0 kbn 50                      ; stack of thread 0, 50 frames
.lastevent                     ; last debug event (the exception)
!analyze -v                    ; auto-analyze the crash, suggest cause
.ecxr                          ; switch to exception context (after crash)
lm m <pattern>                 ; list modules matching pattern
lm m clr* ; lm m mscorlib*     ; list CLR-related modules
!locks                         ; CRITICAL_SECTION ownership / waiters
dt <type> <addr>               ; dump struct (needs symbols)
dps <addr> L40                 ; dump pointer-symbols (great for stack walking)
.frame <N>; dv /V /t           ; switch to frame N and dump locals with types
qd                             ; quit and detach (non-invasive). 'q' kills.
g                              ; go (resume)
gc                             ; go conditional (continue from bp)
```

## When `kbn` shows huge symbol offsets (e.g. `+0x239160`)

That offset is a red flag. It means cdb couldn't find the real function for that PC and fell back to the nearest exported symbol — the actual code lives somewhere else entirely.

Fix: pull public symbols from Microsoft's symbol server. Set `_NT_SYMBOL_PATH` or use `-y`:

```
SRV*C:\symcache*https://msdl.microsoft.com/download/symbols;<your_private_pdb_dir>
```

First fetch is ~2 min. Cached locally afterward. Do this BEFORE you trust any offset > a few hundred bytes.
