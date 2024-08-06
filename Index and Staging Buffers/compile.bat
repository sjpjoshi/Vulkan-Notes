@echo off
REM Set the path to glslc.exe
set GLSLC=C:\VulkanSDK\1.3.283.0\Bin\glslc.exe

REM Set the path to your shader files
set SHADER_DIR=C:\Users\suraj\OneDrive\Documents\Visual Studio Projects\Vulkan Notes\Index and Staging Buffers
REM Compile the vertex shader
"%GLSLC%" "%SHADER_DIR%\simple_shader.vert" -o "%SHADER_DIR%\simple_shader.vert.spv"

REM Compile the fragment shader
"%GLSLC%" "%SHADER_DIR%\simple_shader.frag" -o "%SHADER_DIR%\simple_shader.frag.spv"

echo Shader compilation complete.
pause
