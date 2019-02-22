@echo off
echo Don't tell me you forgot about windows users :P
set BasePath=%1
if [%BasePath%]==[] set BasePath=%~dp0
if not exist %BasePath%\ goto err
	mkdir %BasePath%\atmosphere
    mkdir %BasePath%\atmosphere\titles
    mkdir %BasePath%\atmosphere\titles\010000000000100C
    echo > %BasePath%\atmosphere\titles\010000000000100C\exefs\rtld.stub
    copy layoff.nsp %BasePath%\atmosphere\titles\010000000000100C\exefs.nsp

if "%~2"=="romfs" (goto romfs) else goto :eof
:romfs
mkdir %BasePath%\atmosphere\titles\010000000000100C\romfs
xcopy romfs %BasePath%\atmosphere\titles\010000000000100C\romfs /m
echo > %BasePath%\atmosphere\titles\010000000000100C\fsmitm.flag
echo Make sure that the archive bit is not set for the romfs folder on the sd or RomFS won't work !
goto :eof
:err
echo %BasePath% doesn't exist.