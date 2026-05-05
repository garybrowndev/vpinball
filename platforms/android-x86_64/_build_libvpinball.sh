#!/bin/bash
# Cross-compile libvpinball.so for Android x86_64.
# Run after platforms/android-x86_64/external.sh has produced the third-party
# tree under third-party/runtime-libs/android-x86_64/.

set -e

export ANDROID_NDK_HOME=/opt/android/android-ndk-r28c
export ANDROID_NDK=$ANDROID_NDK_HOME
export ANDROID_NDK_ROOT=$ANDROID_NDK_HOME

cd /mnt/c/code/Pinball/vpinball_ballhistory/development

echo "=== libvpinball cross-compile starting at $(date) ==="

# Stage the BGFX library template as the active CMakeLists.txt
cp make/CMakeLists_bgfx_lib.txt CMakeLists.txt

cmake \
    -DPLATFORM=android \
    -DARCH=x86_64 \
    -DCMAKE_BUILD_TYPE=Debug \
    -DANDROID_NDK=$ANDROID_NDK_HOME \
    -B build/android-x86_64

cmake --build build/android-x86_64 -- -j$(nproc)

echo "=== libvpinball cross-compile ended at $(date) ==="
ls -lh build/android-x86_64/libvpinball.so 2>/dev/null || echo "WARN: libvpinball.so not found at expected path"
