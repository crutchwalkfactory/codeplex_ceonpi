@echo off
rem pushd ..
build %1
if EXIST build.err goto skip1
rem popd

del %SG_OUTPUT_ROOT%\platform\%_TGTPLAT%\target\%_TGTCPU%\%WINCEDEBUG%\sldr.nb0 
del %_flatreleasedir%\sldr.nb0

call romimage test.bib

copy %SG_OUTPUT_ROOT%\platform\%_TGTPLAT%\target\%_TGTCPU%\%WINCEDEBUG%\nk.bin %_flatreleasedir%\nk.bin

goto end1
:skip1
rem popd
echo.
echo ****************
echo Errors in build.
echo ****************
echo.
:end1