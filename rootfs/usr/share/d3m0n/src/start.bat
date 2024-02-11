@echo off

REM use https://objects.githubusercontent.com/github-production-release-asset-2e65be/262681272/1bd1f438-ff5d-46f1-8832-f2f60058b780?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credential=AKIAVCODYLSA53PQK4ZA%2F20240201%2Fus-east-1%2Fs3%2Faws4_request&X-Amz-Date=20240201T151804Z&X-Amz-Expires=300&X-Amz-Signature=520f909821513cb0dfd27f1e5306fe3ea176a99bc5fa8a1d0dd8b4f00333d378&X-Amz-SignedHeaders=host&actor_id=71982379&key_id=0&repo_id=262681272&response-content-disposition=attachment%3B%20filename%3Dw64devkit-1.21.0.zip&response-content-type=application%2Foctet-stream

set path=%path%;C:\Users\users\MinGW\lib\gcc\x86_64-w64-mingw32\13.2.0

set homepath=%cd%

cd C:\Users\users\MinGW\bin\

call gcc "%homepath%\*.c" -o d3m0n.exe

pause

mv d3m0n.exe "%homepath%\d3m0n.exe"

cd "%homepath%"

call d3m0n.exe

pause
