#!/bin/bash
# Wrapper that sets up env vars and runs external.sh.
# This file exists because PowerShell mangles $ in `bash -c '...'` args —
# variable expansion only works reliably when bash runs from a file.

export ANDROID_NDK_HOME=/opt/android/android-ndk-r28c
export ANDROID_NDK=$ANDROID_NDK_HOME
export ANDROID_NDK_ROOT=$ANDROID_NDK_HOME

cd /mnt/c/code/Pinball/vpinball_ballhistory/development

echo "=== external.sh starting at $(date) ==="
echo "ANDROID_NDK_HOME=$ANDROID_NDK_HOME"
echo "ANDROID_NDK=$ANDROID_NDK"
echo "ANDROID_NDK_ROOT=$ANDROID_NDK_ROOT"

BUILD_TYPE=Debug ./platforms/android-x86_64/external.sh
RC=$?
echo "=== external.sh ended at $(date) — exit $RC ==="
exit $RC
