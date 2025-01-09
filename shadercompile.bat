@echo off

set srcpath=%1

if not exist "%srcpath%\bin\assets\shaders\" mkdir "%srcpath%\bin\assets\shaders"

echo "Compiling shaders"

echo "assets/shaders/Builtin.ObjectShader.vert.glsl -> bin/assets/shaders/Builtin.ObjectShader.vert.spv"
%VULKAN_SDK%\bin\glslc.exe -fshader-stage=vert %srcpath%\assets\shaders\Builtin.ObjectShader.vert.glsl -o %srcpath%\bin\assets\shaders\Builtin.ObjectShader.vert.spv
IF %ERRORLEVEL% NEQ 0 (echo Error: %ERRORLEVEL% && exit)

echo "assets/shaders/Builtin.ObjectShader.frag.glsl -> bin/assets/shaders/Builtin.ObjectShader.frag.spv"
%VULKAN_SDK%\bin\glslc.exe -fshader-stage=frag %srcpath%\assets\shaders\Builtin.ObjectShader.frag.glsl -o %srcpath%\bin\assets\shaders\Builtin.ObjectShader.frag.spv
IF %ERRORLEVEL% NEQ 0 (echo Error: %ERRORLEVEL% && exit)

echo "Copying assets"
echo xcopy "assets" "bin\assets" /h /i /c /k /e /r /y
xcopy "assets" "bin\assets" /h /i /c /k /e /r /y

echo "Done"