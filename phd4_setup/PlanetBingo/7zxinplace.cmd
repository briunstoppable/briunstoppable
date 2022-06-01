set zipperLocation=%programfiles%\7-Zip
if exist "%programfiles%\7-Zip\7z.exe" goto :Use_default_7z
if exist "%programfiles(x86)%\7-Zip\7z.exe" goto :Use_32bit_7z

:BadParameter
@echo off
echo this script uses 7z.exe and cannot find it
echo install 7zip and re-run.
echo exiting...
goto :TheEnd

:Use_default_7z
set zipperLocation=%programfiles%\7-Zip
goto :Do_unzipping

:Use_32bit_7z
set zipperLocation=%programfiles(x86)%\7-Zip
goto :Do_unzipping

:Do_unzipping
"%zipperLocation%\7z" x *.gz

dir /s /b /ad > dirs.txt
pushd .
for /f "delims=" %%i in (dirs.txt) do (cd "%%i" && "%zipperLocation%\7z" x *.gz)
popd

del /s *.gz

:TheEnd
echo TheEnd