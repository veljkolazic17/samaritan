@echo off

if "%~1"=="" (set srcpath=%~dp0) else (set srcpath=%1)

if not exist "%srcpath%\out\build\bin\assets\shaders\" mkdir "%srcpath%\out\build\bin\assets\shaders"

echo "Compiling shaders"

echo "assets/shaders/Builtin.ObjectShader.vert.glsl -> out/build/bin/assets/shaders/Builtin.ObjectShader.vert.spv"
%VULKAN_SDK%\bin\glslc.exe -fshader-stage=vert %srcpath%\assets\shaders\Builtin.ObjectShader.vert.glsl -o %srcpath%\out\build\bin\assets\shaders\Builtin.ObjectShader.vert.spv
IF %ERRORLEVEL% NEQ 0 (echo Error: %ERRORLEVEL% && exit)

echo "assets/shaders/Builtin.ObjectShader.frag.glsl -> out/build/bin/assets/shaders/Builtin.ObjectShader.frag.spv"
%VULKAN_SDK%\bin\glslc.exe -fshader-stage=frag %srcpath%\assets\shaders\Builtin.ObjectShader.frag.glsl -o %srcpath%\out\build\bin\assets\shaders\Builtin.ObjectShader.frag.spv
IF %ERRORLEVEL% NEQ 0 (echo Error: %ERRORLEVEL% && exit)

echo "assets/shaders/Builtin.InfiniteGridShader.vert.glsl -> out/build/bin/assets/shaders/Builtin.InfiniteGridShader.vert.spv"
%VULKAN_SDK%\bin\glslc.exe -fshader-stage=vert %srcpath%\assets\shaders\Builtin.InfiniteGridShader.vert.glsl -o %srcpath%\out\build\bin\assets\shaders\Builtin.InfiniteGridShader.vert.spv
IF %ERRORLEVEL% NEQ 0 (echo Error: %ERRORLEVEL% && exit)

echo "assets/shaders/Builtin.InfiniteGridShader.frag.glsl -> out/build/bin/assets/shaders/Builtin.InfiniteGridShader.frag.spv"
%VULKAN_SDK%\bin\glslc.exe -fshader-stage=frag %srcpath%\assets\shaders\Builtin.InfiniteGridShader.frag.glsl -o %srcpath%\out\build\bin\assets\shaders\Builtin.InfiniteGridShader.frag.spv
IF %ERRORLEVEL% NEQ 0 (echo Error: %ERRORLEVEL% && exit)

echo "assets/shaders/Builtin.PickingShader.vert.glsl -> out/build/bin/assets/shaders/Builtin.PickingShader.vert.spv"
%VULKAN_SDK%\bin\glslc.exe -fshader-stage=vert %srcpath%\assets\shaders\Builtin.PickingShader.vert.glsl -o %srcpath%\out\build\bin\assets\shaders\Builtin.PickingShader.vert.spv
IF %ERRORLEVEL% NEQ 0 (echo Error: %ERRORLEVEL% && exit)

echo "assets/shaders/Builtin.PickingShader.frag.glsl -> out/build/bin/assets/shaders/Builtin.PickingShader.frag.spv"
%VULKAN_SDK%\bin\glslc.exe -fshader-stage=frag %srcpath%\assets\shaders\Builtin.PickingShader.frag.glsl -o %srcpath%\out\build\bin\assets\shaders\Builtin.PickingShader.frag.spv
IF %ERRORLEVEL% NEQ 0 (echo Error: %ERRORLEVEL% && exit)

echo "Copying assets"
echo xcopy "%srcpath%\assets" "%srcpath%\out\build\bin\assets" /h /i /c /k /e /r /y
xcopy "%srcpath%\assets" "%srcpath%\out\build\bin\assets" /h /i /c /k /e /r /y

echo "Done"