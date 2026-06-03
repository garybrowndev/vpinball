#!/bin/bash
# Rebuild only BGFX in Release config, copy artifacts to third-party/, relink libvpinball.
# Workaround for emulator's vulkan.ranchu.so crashing on vkSetDebugUtilsObjectNameEXT
# (called by Debug-built BGFX via setDebugObjectName).
set -e

export ANDROID_NDK_HOME=/opt/android/android-ndk-r28c
export ANDROID_NDK=$ANDROID_NDK_HOME
export ANDROID_NDK_ROOT=$ANDROID_NDK_HOME

REPO=/mnt/c/code/Pinball/vpinball_ballhistory/development
cd $REPO

# Source the SHAs/version
source ./platforms/config.sh

NUM_PROCS=$(nproc)
WORKDIR=$REPO/external/android-x86_64/Release-bgfx-only
mkdir -p $WORKDIR
cd $WORKDIR

if [ ! -d bgfx.cmake ]; then
   echo "=== fetching BGFX sources ==="
   curl -sL https://github.com/bkaradzic/bgfx.cmake/releases/download/v${BGFX_CMAKE_VERSION}/bgfx.cmake.v${BGFX_CMAKE_VERSION}.tar.gz -o bgfx.cmake.tar.gz
   tar xzf bgfx.cmake.tar.gz
   curl -sL https://github.com/vbousquet/bgfx/archive/${BGFX_PATCH_SHA}.tar.gz -o bgfx-patch.tar.gz
   tar xzf bgfx-patch.tar.gz
   cd bgfx.cmake
   rm -rf bgfx
   mv ../bgfx-${BGFX_PATCH_SHA} bgfx
   cd ..
fi

cd bgfx.cmake
echo "=== configuring BGFX (Release, x86_64 Android) ==="
rm -rf build
cmake -S. \
    -DANDROID_NDK=${ANDROID_NDK_HOME} \
    -DCMAKE_SYSTEM_NAME=Android \
    -DCMAKE_SYSTEM_VERSION=33 \
    -DCMAKE_ANDROID_ARCH_ABI=x86_64 \
    -DBGFX_BUILD_EXAMPLES=OFF \
    -DBGFX_BUILD_TOOLS=OFF \
    -DBGFX_CONFIG_MULTITHREADED=ON \
    -DBGFX_CONFIG_MAX_FRAME_BUFFERS=256 \
    -DBGFX_OPENGLES_VERSION=32 \
    -DCMAKE_BUILD_TYPE=Release \
    -B build

echo "=== building BGFX (Release) ==="
cmake --build build -- -j${NUM_PROCS}

echo "=== copying Release .a files over the Debug ones in third-party/ ==="
TPLIB=$REPO/third-party/build-libs/android-x86_64
cp build/cmake/bgfx/libbgfx.a $TPLIB/
cp build/cmake/bimg/libbimg.a $TPLIB/
cp build/cmake/bimg/libbimg_decode.a $TPLIB/
cp build/cmake/bimg/libbimg_encode.a $TPLIB/
cp build/cmake/bx/libbx.a $TPLIB/

echo "=== verify ==="
file $TPLIB/libbgfx.a | head -1
ls -lh $TPLIB/lib{bgfx,bimg,bimg_decode,bimg_encode,bx}.a
