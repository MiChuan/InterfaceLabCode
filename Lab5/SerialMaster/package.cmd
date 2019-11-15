
set appName=SerialMaster
set dd=%date:~0,4%%date:~5,2%%date:~8,2%%time:~0,2%%time:~3,2%%time:~6,2%
set dd=%dd: =0%
set folder=%appName%-%dd%

rmdir %folder% /S /Q
rmdir build /S /Q
rmdir __pycache__  /S /Q
rmdir dist /S /Q

mkdir %folder%

pyinstaller -w -F -i icon/logo.ico main.py

copy dist\main.exe %folder%\%appName%-%dd%.exe

7z a -t7z %folder%.7z %folder%\

rmdir %folder% /S /Q