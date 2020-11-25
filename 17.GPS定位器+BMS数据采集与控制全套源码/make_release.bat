@echo off

echo ***********************************************************************     
echo ************************* start to build ******************************
echo ***********************************************************************

make.exe clean

make.exe -f makefile DEBUG_SYMBOLS=0 APP_BUILD_TIME="%date:~,10% %time:~,8%"

rd /s /q output

copy build\GmAppMain.bin build\GmAppMain.ex

perl filecreat.pl
