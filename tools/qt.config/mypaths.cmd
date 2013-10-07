set INCLUDE=%PICTO_THIRD_PARTY%\openssl\inc32;%INCLUDE%
set LIB=%PICTO_THIRD_PARTY%\openssl\lib;%LIB%
set QTTOP=%PICTO_THIRD_PARTY%\QT\Qt5.1.1\5.1.1
set PATH=%PICTO_THIRD_PARTY%\openssl\bin;%QTTOP%\msvc2010\bin;%PATH%
set QTDIR=%QTTOP%\msvc2010
set PROPBROWSDIR=%QTTOP%\Src\qttools\src\shared\qtpropertybrowser
set QWTDIR=%PICTO_THIRD_PARTY%\qwt
set PHIDGETSDIR=%PICTO_THIRD_PARTY%\phidgets
set OPENSSLDIR=%PICTO_THIRD_PARTY%\openssl
@echo If desired, run the dx_setenv.cmd script from the Microsoft DirectX SDK
@echo Note that the official build does not compile Qt with DirectX enabled
