@echo off
echo Don't tell me you forgot about windows users :P
set BasePath=%1
if [%BasePath%]==[] set BasePath=%~dp0
if not exist %BasePath%\ goto err
	mkdir %BasePath%\atmosphere
    mkdir %BasePath%\atmosphere\titles
    mkdir %BasePath%\atmosphere\titles\010000000000100C
    mkdir %BasePath%\atmosphere\titles\010000000000100C\exefs\
    npdmtool npdm.json %BasePath%\atmosphere\titles\010000000000100C\exefs\main.npdm
    copy build\exefs\main %BasePath%\atmosphere\titles\010000000000100C\exefs\main
    echo > %BasePath%\atmosphere\titles\010000000000100C\exefs\rtld.stub
goto :eof
:err
echo %BasePath% doesn't exist.