set INCLUDE=%PICTO_THIRD_PARTY%\openssl\inc32;%INCLUDE%
set LIB=%PICTO_THIRD_PARTY%\openssl\lib;%LIB%
set QTTOP=%PICTO_THIRD_PARTY%\Qt64\Qt5.3.2\5.3
set PATH=%PICTO_THIRD_PARTY%\openssl\bin;%QTTOP%\msvc2013_64\bin;%PATH%
set QTDIR=%QTTOP%\msvc2013_64
set PROPBROWSDIR=%QTTOP%\Src\qttools\src\shared\qtpropertybrowser
set QWTDIR=%PICTO_THIRD_PARTY%\qwt_64
set PHIDGETSDIR=%PICTO_THIRD_PARTY%\phidgets
set OPENSSLDIR=%PICTO_THIRD_PARTY%\openssl
@echo If desired, run the dx_setenv.cmd script from the Microsoft DirectX SDK
@echo Note that the official build does not compile Qt with DirectX enabled
