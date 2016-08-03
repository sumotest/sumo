call %~dp0\testEnv.bat %1
start %TEXTTEST_HOME%/sikulixServerRunner.py
start %TEXTTESTPY% -a netedit
