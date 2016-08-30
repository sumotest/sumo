call %~dp0\testEnv.bat %1
start %SUMO_HOME%\tests\runSikulixServerWindows.py
start %TEXTTESTPY% -a netedit
