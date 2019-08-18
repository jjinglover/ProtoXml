@echo off
set rootPath=%cd%
set outPath=%rootPath%\proto
set inPath=%rootPath%\proto
%rootPath%\protoc --proto_path=%inPath% --cpp_out=%outPath% %inPath%"\loadConf.proto"

if %errorlevel%==0 (
    echo "success"
) else (
    echo "failed"
)
pause