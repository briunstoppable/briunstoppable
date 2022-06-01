@echo ON

rem
rem remove any .svn directories from subversion tree
rem

rd /s /q .svn

rem
rem remove any hidden .svn directories
rem

dir /s /b /ah .svn > it
for /f "delims=," %%i in (it) do (rd /s /q "%%i")

rem
rem remove any un-needed thumb files
rem

dir /s /b /a h Thumbs.db > it
for /f "delims=," %%i in (it) do (attrib -r -a -s -h "%%i" && del "%%i")
if exist it del it

del manifest.xml /Q

manifest_creator.exe manifest.xml Startup.exe Session L3
