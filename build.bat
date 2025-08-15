@echo off
setlocal enabledelayedexpansion

set project_directory=
set output_directory=
set source_directory=
set msvc_compiler_flags=
set msvc_linker_flags=
set lines=

cd %~dp0
set project_directory=%cd%
set output_directory=%project_directory%\output
set source_directory=%project_directory%\source
set msvc_compiler_flags=/MTd /nologo /Od /W4 /Z7 /wd4100 /wd4101 /wd4189 /I"%source_directory%"
set msvc_linker_flags=/DEBUG:FASTLINK /INCREMENTAL:NO /OPT:REF

rmdir /s /q "%output_directory%"
mkdir "%output_directory%"

pushd "%output_directory%"
cls
if %errorlevel% == 0 (cl %msvc_compiler_flags% "%source_directory%\test\main.c" /link %msvc_linker_flags%)
popd

if %errorlevel% neq 0 (exit /b 1)

set /a lines=0
pushd "%source_directory%"
for /R %%f in (*.h *.c) do (
    set /a file_lines=0

    for /f "usebackq delims=" %%l in ("%%f") do (
        set /a file_lines+=1
    )

    set /a lines+=!file_lines!
    echo %%f: !file_lines!
)
popd

echo.
echo Project lines count: !lines!.
echo.