@echo off

dotnet build

C:\Windows\Microsoft.net\framework64\v4.0.30319\msbuild d3m0n.csproj -property:Configuration=Release -property:SelfContained=true -property:RuntimeIdentifier=linux-arm

pause