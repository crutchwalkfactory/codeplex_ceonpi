@echo off
rem build the common video lib code
pushd %_TARGETPLATROOT%\src\common
build %1
if EXIST build.errgoto skip2
popd

rem build the boot loader
build %1
if EXIST build.err goto skip1
rem popd

del %SG_OUTPUT_ROOT%\platform\%_TGTPLAT%\target\%_TGTCPU%\%WINCEDEBUG%\sldr.nb0 
del %_flatreleasedir%\sldr.nb0

call romimage sboot.bib

if NOT EXIST %SG_OUTPUT_ROOT%\platform\%_TGTPLAT%\target\%_TGTCPU%\%WINCEDEBUG%\sldr.nb0 goto skip1

rem patchfile %SG_OUTPUT_ROOT%\platform\%_TGTPLAT%\target\%_TGTCPU%\%WINCEDEBUG%\sldr.nb0 0 fe
rem patchfile %SG_OUTPUT_ROOT%\platform\%_TGTPLAT%\target\%_TGTCPU%\%WINCEDEBUG%\sldr.nb0 1 03
rem rem patchfile %SG_OUTPUT_ROOT%\platform\%_TGTPLAT%\target\%_TGTCPU%\%WINCEDEBUG%\sldr.nb0 2 00
rem patchfile %SG_OUTPUT_ROOT%\platform\%_TGTPLAT%\target\%_TGTCPU%\%WINCEDEBUG%\sldr.nb0 3 ea

copy %SG_OUTPUT_ROOT%\platform\%_TGTPLAT%\target\%_TGTCPU%\%WINCEDEBUG%\sldr.nb0 %_flatreleasedir%
copy %SG_OUTPUT_ROOT%\platform\%_TGTPLAT%\target\%_TGTCPU%\%WINCEDEBUG%\sldr.bin %_flatreleasedir%\nk.bin

copy %_flatreleasedir%\sldr.nb0 f:\kernel.img

goto end1
:skip2
popd
:skip1
echo.
echo ****************
echo Errors in build.
echo ****************
echo.
:end1