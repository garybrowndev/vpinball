# Ball History for Visual Pinball
Fork of Visual Pinball for development of Ball History feature.
Ball History allows for historical ball control and training sessions for the purpose of improving pinball skill.
Only tested on Windows x64 with DirectX 3D graphics engine.

To build (and merge):
- Sync code
- Extract VPinballX-<VERSION>-<BUILD>-<HASH>-dev-third-party-windows-x64-Debug to .\third-party and overwrite all files
- Revert all changes from above overwrite of files
- (If merging) Merge changes from branch
- Run .\make\create_vs_solution.bat
- Select 2022
- Open .\.build\vsproject\VisualPinball.sln in Visual Studio 2022
- Set VPinball as the Startup Project
- Build VPinball Debug / x64
- Build VPinball Release / x64

To try out
- Hit "C" button while playing
- Read menus/descriptions and figure it out
- DM  [@garybrowndev](https://www.github.com/garybrowndev) if you have feedback or issues

Original readme for forked repo below...

# Visual Pinball

*An open source pinball table editor and simulator.*

This project was started by Randy Davis, open sourced in 2010 and continued by the Visual Pinball development team. This is the official repository.

## Features

- Simulates pinball table physics and renders the table with DirectX, OpenGL or [bgfx](https://bkaradzic.github.io/bgfx/overview.html)
- Simple editor to (re-)create any kind of pinball table
- Live editing in the rendered viewport
- Table logic (and game rules) can be controlled via Visual Basic Script
- Over 1050 real/unique pinball machines from ~100 manufacturers, plus over 550 original creations were rebuilt/designed using the Visual Pinball X editor (over 3000 if one counts all released tables, incl. MODs and different variants), and even more when including its predecessor versions (Visual Pinball 9.X)
- Emulation of real pinball machines via [PinMAME](https://github.com/vpinball/pinmame) is possible via Visual Basic Script (Visual PinMAME), or via the libPinMAME-API
- Supports configurable camera views (e.g. to allow for correct display in virtual pinball cabinets)
- Support for Tablet/Touch input or specialized pinball controllers
- Support for Stereo3D output
- Support for Head tracking via BAM
- Support for VR HMD rendering (including [PUP](https://www.nailbuster.com/wikipinup), [B2S](https://github.com/vpinball/b2s-backglass) backglass and DMD output support)
- Support for WCG/HDR rendering (for now only via the BGFX (D3D11/12) build)
- Support for Windows (x86), Linux (x86/Arm, incl. RaspberryPi and RK3588), macOS, iOS/tvOS, Android

## Download

All releases are available on the [releases page](https://github.com/vpinball/vpinball/releases).

## Documentation

Documentation is currently sparse. Check the [docs](docs) directory for various guides and references.

## How to build

Build instructions are available in the [make directory README](make/README.md).
