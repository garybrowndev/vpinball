#!/usr/bin/env python3
"""Patch bgfx renderer_vk.cpp SwapChainVK::update to never recreate the swapchain.

Android emulator's libvulkan.so/CreateSwapchainKHR crashes on swapchain recreate.
Force-disable BGFX's recreate logic by hardcoding recreateSurface and recreateSwapchain
to false at the top of update().
"""
import re
import sys

SRC = "/mnt/c/code/Pinball/vpinball_ballhistory/development/external/android-x86_64/Release-bgfx-only/bgfx.cmake/bgfx/src/renderer_vk.cpp"

s = open(SRC).read()

# Find the recreateSurface assignment block and force it to false
# Original:
#    const bool recreateSurface = false
#       || m_needToRecreateSurface
#       || ...
#       ;
# Patched: const bool recreateSurface = false /* PATCH: always false */;
patches = [
    (
        r"const bool recreateSurface = false\n"
        r"\t\t\t\|\| m_needToRecreateSurface\n"
        r"(?:\t\t\t\|\|[^\n]+\n)+"
        r"\t\t\t;",
        "const bool recreateSurface = false /* PATCHED: always false for emulator */;"
    ),
    (
        r"const bool recreateSwapchain = false\n"
        r"\t\t\t\|\| m_needToRecreateSwapchain\n"
        r"(?:\t\t\t\|\|[^\n]+\n)+"
        r"\t\t\t;",
        "const bool recreateSwapchain = false /* PATCHED: always false for emulator */;"
    ),
]

for pattern, replacement in patches:
    new_s, n = re.subn(pattern, replacement, s, count=1)
    if n != 1:
        print(f"ERROR: pattern not matched: {pattern[:60]}...")
        sys.exit(1)
    s = new_s

open(SRC, "w").write(s)
print("patched OK. Lines around 7418:")
for i, line in enumerate(s.split("\n")[7414:7440], 7415):
    print(f"  {i}: {line}")
