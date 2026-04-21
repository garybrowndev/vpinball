#!/usr/bin/env bash
# Swap Debug/Release bgfx lib flavors without re-extracting upstream zips.
#
# VPinball's Debug_BGFX and Release_BGFX configs both link the same lib
# filenames (bgfx64.lib, bimg64.lib, bimg_decode64.lib, bx64.lib), but the
# libs themselves differ per flavor. Upstream's workflow is to extract
# whichever zip matches the build. This script keeps local snapshots of
# both flavors so you can swap in seconds.
#
# First-time setup (once per flavor): extract the matching upstream zip,
# then run `./make/swap-bgfx.sh snapshot <flavor>`. After both snapshots
# exist, `activate <flavor>` swaps freely.

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
ACTIVE="$ROOT/third-party/build-libs/windows-x64"
SNAP_ROOT="$ROOT/third-party/build-libs-bgfx-snapshots"
LIBS=(bgfx64.lib bimg_decode64.lib bimg64.lib bx64.lib)

usage() {
   cat <<EOF
Usage: $0 <command> [flavor]

Commands:
  snapshot <debug|release>    Save current active libs as the named snapshot
  activate <debug|release>    Restore named snapshot into the active lib dir
  status                      Show snapshot state and active flavor

Active:    $ACTIVE
Snapshots: $SNAP_ROOT/{debug,release}
EOF
}

flavor_ok() {
   [[ "$1" == "debug" || "$1" == "release" ]]
}

file_size() {
   # portable: Linux stat -c, BSD/macOS stat -f
   stat -c %s "$1" 2>/dev/null || stat -f %z "$1"
}

cmd_snapshot() {
   local flavor="${1:-}"
   flavor_ok "$flavor" || { echo "Invalid flavor: ${flavor:-(none)}" >&2; usage; exit 1; }
   local dest="$SNAP_ROOT/$flavor"
   mkdir -p "$dest"
   for lib in "${LIBS[@]}"; do
      if [[ ! -f "$ACTIVE/$lib" ]]; then
         echo "Missing active lib: $ACTIVE/$lib" >&2
         exit 1
      fi
      cp -p "$ACTIVE/$lib" "$dest/$lib"
   done
   echo "Saved $flavor snapshot to $dest"
}

cmd_activate() {
   local flavor="${1:-}"
   flavor_ok "$flavor" || { echo "Invalid flavor: ${flavor:-(none)}" >&2; usage; exit 1; }
   local src="$SNAP_ROOT/$flavor"
   for lib in "${LIBS[@]}"; do
      if [[ ! -f "$src/$lib" ]]; then
         echo "No $flavor snapshot for $lib -- run '$0 snapshot $flavor' first" >&2
         exit 1
      fi
   done
   for lib in "${LIBS[@]}"; do
      cp -p "$src/$lib" "$ACTIVE/$lib"
   done
   echo "Activated $flavor bgfx libs in $ACTIVE"
}

cmd_status() {
   for flavor in debug release; do
      local f="$SNAP_ROOT/$flavor/bgfx64.lib"
      if [[ -f "$f" ]]; then
         echo "$flavor   snapshot: $SNAP_ROOT/$flavor (bgfx64.lib = $(file_size "$f") bytes)"
      else
         echo "$flavor   snapshot: missing"
      fi
   done
   if [[ -f "$ACTIVE/bgfx64.lib" ]]; then
      echo "active   bgfx64.lib: $(file_size "$ACTIVE/bgfx64.lib") bytes (Debug ~12.7M, Release ~3.8M)"
   fi
}

case "${1:-}" in
   snapshot) cmd_snapshot "${2:-}" ;;
   activate) cmd_activate "${2:-}" ;;
   status) cmd_status ;;
   ""|-h|--help) usage ;;
   *) echo "Unknown command: $1" >&2; usage; exit 1 ;;
esac
