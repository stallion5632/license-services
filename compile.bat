@echo off 

set build="build"
if not exist %build% md  %build% .

cd %build% 

cmake.exe -DCMAKE_BUILD_TYPE=Release  ..

cd ..