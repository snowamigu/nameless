@echo off

cd %~dp0
set project_directory=%cd%
set output_directory=%project_directory%\output
set source_directory=%project_directory%\source
set msvc_compiler_flags=/MTd /nologo /Od /W4 /Za /Zi /wd4100 /wd4101 /wd4189 /DDEBUG /I"%source_directory%"
set msvc_linker_flags=/DEBUG:FASTLINK /INCREMENTAL:NO /OPT:REF

rmdir /s /q "%output_directory%"
mkdir "%output_directory%"

pushd "%output_directory%"
cls
if %errorlevel% == 0 (cl %msvc_compiler_flags% /LD "%source_directory%\library\hardware.c" /link %msvc_linker_flags%)
if %errorlevel% == 0 (cl %msvc_compiler_flags% /LD "%source_directory%\library\string.c" /link %msvc_linker_flags%)
if %errorlevel% == 0 (cl %msvc_compiler_flags% /Fo"hardware_.obj" "%source_directory%\test\hardware.c" /link %msvc_linker_flags% hardware.lib)
if %errorlevel% == 0 (cl %msvc_compiler_flags% /Fo"string_.obj" "%source_directory%\test\string.c" /link %msvc_linker_flags% hardware.lib string.lib)
if %errorlevel% == 0 (cl %msvc_compiler_flags% "%source_directory%\test\main.c" /link %msvc_linker_flags% hardware.lib string.lib)
popd