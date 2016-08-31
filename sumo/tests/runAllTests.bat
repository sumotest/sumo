call %~dp0\testEnv.bat %1
start %SUMO_HOME%\tests\runSikulixServerWindows.pyw
start %TEXTTESTPY%
