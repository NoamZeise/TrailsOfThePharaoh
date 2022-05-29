cd build
call vcvars64
cmake .. -G"Ninja Multi-Config" -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DGFX_ENV_OPENGL=1 -DGFX_ENV_VULKAN=0
cmake --build . --config Release
cd ..
Xcopy resources\audio build\Release\audio /i /c /e /r /y
Xcopy resources\maps build\Release\maps /i /c /e /r /y
Xcopy resources\shaders build\Release\shaders /i /c /e /r /y
Xcopy resources\textures build\Release\textures /i /c /e /r /y
Xcopy resources\windows-dlls build\Release\ /i /c /e /r /y
Xcopy resources\dialogue build\Release\dialogue /i /c /e /r /y
cd build\Release
OpenGL-Environment
cd ../..
