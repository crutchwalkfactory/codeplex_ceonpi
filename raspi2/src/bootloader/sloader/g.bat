@echo off
pushd .
build %1
if EXIST build.err goto skip1
popd

del %SG_OUTPUT_ROOT%\platform\%_TGTPLAT%\target\%_TGTCPU%\%WINCEDEBUG%\sldr.nb0

call romimage sloader.bib

if NOT EXIST %SG_OUTPUT_ROOT%\platform\%_TGTPLAT%\target\%_TGTCPU%\%WINCEDEBUG%\sldr.nb0 goto skip2

rem copy C:\WINCE700\OSDesigns\PiProj1\PiProj1\Wince700\raspi_ARMV6_Debug\cesysgen\platform\raspi\target\ARMV6\debug\SLDR.nb0
del /q %_flatreleasedir%\sldr.nb0

copy %SG_OUTPUT_ROOT%\platform\%_TGTPLAT%\target\%_TGTCPU%\%WINCEDEBUG%\sldr.nb0 %_flatreleasedir%
copy %SG_OUTPUT_ROOT%\platform\%_TGTPLAT%\target\%_TGTCPU%\%WINCEDEBUG%\sldr.nb0 %_flatreleasedir%\kernel.img


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