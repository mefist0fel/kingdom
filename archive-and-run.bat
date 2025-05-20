@echo off
setlocal

rem === Settings ===
set PROJECT=Kingdom
set PROJECT_DIR=%PROJECT%.pdx

rem Remove folder
if exist %PROJECT_DIR% (
    echo Clear %PROJECT_DIR%...
    rmdir /s /q %PROJECT_DIR%
) else (
    echo Catalog %PROJECT_DIR% removing skipped
)

rem Build project folder
echo Build...
pdc Source %PROJECT_DIR%
if errorlevel 1 (
    echo Build error!
    exit /b 1
)

rem Start emulator
echo Start simulator...
PlaydateSimulator %PROJECT_DIR%

endlocal