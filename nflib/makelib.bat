@echo off
cls
del lib\*.* /q
make clean
make
pause
exit