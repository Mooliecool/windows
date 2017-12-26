After calling env.bat, you'll need to set up the INCLUDE environment variable to point to the relavent VC includes, for example: 

set INCLUDE=%INCLUDE%;c:\PROGRA~1\MICROS~2.0\VC\include;c:\PROGRA~1\MICROS~2.0\VC\atlmfc\include;c:\PROGRA~1\MIA713~1\Windows\v7.0A\Include;c:\sscli20\clr\src\inc;c:\sscli20\prebuilt\idl;c:\sscli20\palrt\inc;c:\sscli20\vscommon\inc;c:\sscli20\csharp\inc

and also, have LIBPATH point to the Windows SDK directory that Visual Studio 2010 lays out: 

set LIBPATH=%LIBPATH%;C:\progra~1\mia713~1\windows\v7.0a\Lib

If you're running x64, and the default install for the Windows SDK is in the Program Files (x86) directory
then you may need to change line 286 in sscli20\env\bin\devdiv.def to point to the correct kernel32.lib: 

LINKLIBS=$(LINKLIBS) C:\PROGRA~1\MIA713~1\Windows\v7.0A\Lib\kernel32.lib $(PERFLIBS)
