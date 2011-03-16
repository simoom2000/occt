@echo off
rem Launch MS VC with environment prepared for building OCCT Java sample

rem Set build environment 
call %~dp0..\..\..\..\ros\env_build.bat vc8 win32

rem Define path to project file
set PRJFILE=%~dp0SAMPLE.sln

rem Launch Visual Studio - either professional (devenv) or Express, as available
if exist %DevEnvDir%\devenv.exe (
    start %DevEnvDir%\devenv.exe %PRJFILE% /useenv
) else if exist %DevEnvDir%\VCExpress.exe (
    start %DevEnvDir%\VCExpress.exe %PRJFILE% /useenv
) else (
    echo Error: Could not find MS Visual Studio ^(%VCVER%^)
    echo Check relevant environment variable ^(e.g. VS80COMNTOOLS for vc8^)
)

