@echo off
pushd .
build %1
if EXIST build.err goto skip1
popd

call romimage sloader_ram.bib

if NOT EXIST %SG_OUTPUT_ROOT%\platform\%_TGTPLAT%\target\%_TGTCPU%\%WINCEDEBUG%\sldr.bin goto skip2

copy %SG_OUTPUT_ROOT%\platform\%_TGTPLAT%\target\%_TGTCPU%\%WINCEDEBUG%\sldr.bin %_flatreleasedir%\nk.bin
copy %SG_OUTPUT_ROOT%\platform\%_TGTPLAT%\target\%_TGTCPU%\%WINCEDEBUG%\sldr.bin  C:\WINCE700\OSDesigns\PiProj2\PiProj2\RelDir\raspi2_ARMV6_Checked\nk.bin

goto end1
:skip1
popd
:skip2
echo.
echo ****************
echo Errors in build.
echo ****************
echo.
:end1