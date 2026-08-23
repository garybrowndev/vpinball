# Release body template

Fill `{{TAG}}`, `{{SHA}}`, `{{BRANCH}}`, `{{CHANGES}}` and write the result to a temp file, then
pass it with `gh release edit $TAG --notes-file <file>`.

`{{CHANGES}}` is 3–6 themed bullets distilled from `git log <prev-release-tag>..<sha> --format='%s'
--no-merges` — named subsystems and kinds of change, not raw commit subjects. Drop the whole
"What's changed" section on a first release or when the range is empty.

Derived from the April 2026 release body, with its DX9 claim corrected: the workflow's `VPinballX_*`
glob has never attached DX9 assets, and upstream's releases don't carry them either.

---

## Ball History Build

This is a release of **VPinballX with Ball History** — a fork that adds ball state recording,
replay, and trainer mode to Visual Pinball.

### What's included
- **Record & Replay** — capture ball positions during gameplay and step back through history
- **Trainer Mode** — set up training runs with configurable start/pass/fail locations, corridors,
  physics variance, and difficulty settings
- **In-game overlay** — press **V** during gameplay to open the Ball History menu; press **R** to
  recall previous ball states

### What's changed since {{PREV_TAG}}
{{CHANGES}}

### Getting started (Windows)
1. Download the appropriate zip for your system — most users want
   `VPinballX_BGFX-{{TAG_NOV}}-windows-x64-Release.zip` or the `VPinballX_GL-` equivalent
2. Extract to a folder
3. Run the exe and load any `.vpx` table
4. Press **F5** to play, then **V** to open the Ball History menu

### Platforms
BGFX and OpenGL builds are attached for Windows, macOS, Linux, Raspberry Pi / RK3588, Android and
iOS. Ball History is developed and tested on **Windows x64** only — the other platforms include the
code but are not actively tested. (A Direct3D 9 build exists in CI but, as upstream, is not
attached here.)

Built from the `{{BRANCH}}` branch at commit `{{SHA}}`.
