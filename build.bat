@echo off
setlocal

set BUILD_DIR=build

if not exist %BUILD_DIR% mkdir %BUILD_DIR%

cd %BUILD_DIR%

cmake .. -DCMAKE_BUILD_TYPE=Release

cmake --build . --config Release

echo.
echo The build is complete. The library file is located in %BUILD_DIR%/Release/ (or %BUILD_DIR%/)
cd ..
endlocal
pause