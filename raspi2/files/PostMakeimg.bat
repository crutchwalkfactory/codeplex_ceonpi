@echo off
echo Running %0
rem
rem Checks that IMGFLASH is set
rem
if .%IMGFLASH%. == .. goto NoFlash

rem
rem Check that nk not too big
rem
if NOT EXIST nk.nb1 goto sizegood
echo.
echo Error! NK.BIN too big for bib settings.  Change NKLEN in config.bib
echo.
goto errorend

:sizegood

rem
rem Delete existing kernel.img if existing and rename nk.nb0 to kernel.img
rem this is much faster than a copy of nk.nb0
rem
pushd %_flatreleasedir%
if NOT EXIST kernel.img goto nokernyet
del kernel.img
:nokernyet
echo renaming nk.nb0 to kernel.img
rename nk.nb0  kernel.img
popd

goto endit
:NoFlash
echo. 
echo IMGFLASH not set.  It must be set to create kernel.img
echo. 
:errorend
echo Setting error level to indicate error to makeimg
exit /b 1
:endit