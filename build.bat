@echo off

echo. 
echo *** Compiling image16c
wcc386 %CCOPTS% image16c.c

echo. 
echo *** Compiling bmp
wcc386 %CCOPTS% bmp.c

echo. 
echo *** Compiling display16
wcc386 %CCOPTS% display16.c

echo. 
echo *** Compiling bmpto16
wcc386 %CCOPTS% bmpto16.c

echo. 
echo *** Compiling play16
wcc386 %CCOPTS% play16.c

echo. 
echo *** Compiling defplt
wcc386 %CCOPTS% defplt.c

echo. 
echo *** Linking display16
wlink SYSTEM NT %LOPTS% OPTION MAP FILE display16,image16c

echo. 
echo *** Linking bmpto16
wlink SYSTEM NT %LOPTS% OPTION MAP FILE bmpto16,image16c,bmp

echo. 
echo *** Linking play16
wlink SYSTEM NT %LOPTS% OPTION MAP FILE play16,image16c LIBRARY winmm

echo. 
echo *** Linking defplt
wlink SYSTEM NT %LOPTS% OPTION MAP FILE defplt,image16c

echo. 
echo *** Compiling and linking txtdump
wcl386 %CCOPTS% -"%LOPTS%" txtdump.c

pause
