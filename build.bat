@echo off

set path=%path%;C:\Users\celcm\MinGW\lib\gcc\x86_64-w64-mingw32\13.2.0

set homepath=%cd%

cd C:\Users\celcm\MinGW\bin\

call gcc "%homepath%\*.c" -o d3m0n.exe

pause

mv d3m0n.exe "%homepath%\d3m0n.exe"

cd "%homepath%"

call d3m0n.exe

pause