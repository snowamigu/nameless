@echo off

:: variables.
set project_directory=
set output_directory=
set msvc_compiler_flags=
set msvc_linker_flags=

:: gets script working directory and sets "project_directory", "output_directory".
cd %~dp0
set project_directory=%cd%
set output_directory=%project_directory%\output

:: removes old files and create a new empty folder named "output".
rmdir /s /q "%output_directory%"
mkdir "%output_directory%"

:: compiler/linker flags.
set msvc_compiler_flags=/MTd /nologo /Od /W4 /Za /Zi /wd4100 /wd4101 /wd4189 /DDEBUG /I"%project_directory%\source"
set msvc_linker_flags=/DEBUG:FASTLINK /INCREMENTAL:NO /OPT:REF

:: compilation.
pushd "%output_directory%"
cls
if %errorlevel% == 0 (cl %msvc_compiler_flags% /LD "%project_directory%\source\library\console.c" /link %msvc_linker_flags%)
if %errorlevel% == 0 (cl %msvc_compiler_flags% /LD "%project_directory%\source\library\string.c" /link %msvc_linker_flags%)
if %errorlevel% == 0 (cl %msvc_compiler_flags% /Fo"console_.obj" "%project_directory%\source\test\console.c" /link %msvc_linker_flags% console.lib)
if %errorlevel% == 0 (cl %msvc_compiler_flags% /Fo"string_.obj" "%project_directory%\source\test\string.c" /link %msvc_linker_flags% console.lib string.lib)
popd