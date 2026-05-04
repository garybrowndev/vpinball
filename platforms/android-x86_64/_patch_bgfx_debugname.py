#!/usr/bin/env python3
"""Patch bgfx renderer_vk.cpp setDebugObjectName template to be a true no-op.

The default body is gated on BGFX_CONFIG_DEBUG_OBJECT_NAME && EXT_debug_utils.m_supported,
but Android emulator's vulkan.ranchu.so crashes inside vk_common_SetDebugUtilsObjectNameEXT
even when the extension is advertised. Replace the body with an early-return.
"""
import re
import sys

SRC = "/mnt/c/code/Pinball/vpinball_ballhistory/development/external/android-x86_64/Release-bgfx-only/bgfx.cmake/bgfx/src/renderer_vk.cpp"

s = open(SRC).read()

new_body = """\tstatic BX_NO_INLINE void setDebugObjectName(VkDevice _device, Ty _object, const char* _format, ...)
\t{
\t\t// PATCHED for Android emulator: vulkan.ranchu.so crashes in vk_common_SetDebugUtilsObjectNameEXT
\t\tBX_UNUSED(_device, _object, _format);
\t}"""

pattern = re.compile(
    r"\tstatic BX_NO_INLINE void setDebugObjectName\(VkDevice _device, Ty _object, const char\* _format, \.\.\.\)\n"
    r"\t\{.*?VK_CHECK_W\(vkSetDebugUtilsObjectNameEXT\(_device, &ni\) \);\n\t\t\}\n\t\}",
    re.DOTALL
)

m = pattern.search(s)
if not m:
    print("ERROR: could not find setDebugObjectName body")
    sys.exit(1)

new_s = pattern.sub(new_body, s, count=1)

if new_s == s:
    print("ERROR: substitution made no change")
    sys.exit(1)

with open(SRC, "w") as f:
    f.write(new_s)

print("patched OK. Lines 968-978:")
for i, line in enumerate(new_s.split("\n")[967:978], 968):
    print(f"  {i}: {line}")
