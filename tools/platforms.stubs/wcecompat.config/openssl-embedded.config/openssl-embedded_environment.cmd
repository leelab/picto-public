set OSVERSION=WCE600
set PLATFORM=PICTOBOX
set TARGETCPU=X86
set INCLUDE=%DevEnvDir%\..\..\VC\ce\include;%PICTO_PICTOBOX_SDK%\Include\x86;%PICTO_THIRD_PARTY%\wcecompat\include
set LIB=%DevEnvDir%\..\..\VC\ce\lib\x86;%PICTO_PICTOBOX_SDK%\Lib\x86;%PICTO_THIRD_PARTY%\wcecompat\lib
set PATH=%DevEnvDir%\..\..\VC\ce\bin\x86_cex86;%PATH%
set WCECOMPAT=%PICTO_THIRD_PARTY%\wcecompat