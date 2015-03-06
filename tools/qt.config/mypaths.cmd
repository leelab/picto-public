@REM The PATH variable contains parens, so we cannot have it nested in a set, or it crashes
@REM the execution of the script.
@IF NOT DEFINED PICTO_ENV_SET (
  @echo Picto environment hasn't been configured
  @echo Run the appropriate configure script for your platform from the Picto root directory and try again
  EXIT /B
)

@REM Setting the directories for each of the supported builds
@IF "%PICTO_MACHINE_TYPE%"=="X86" (
  @set PICTO_QT_INSTALL_TOP_DIR=Qt
  @set PICTO_QT_MSVC_DIR=msvc2013
  @set PICTO_QWT_DIR=qwt
)

@IF "%PICTO_MACHINE_TYPE%"=="X64" (
  @set PICTO_QT_INSTALL_TOP_DIR=Qt64
  @set PICTO_QT_MSVC_DIR=msvc2013_64
  @set PICTO_QWT_DIR=qwt_64
)

set INCLUDE=%PICTO_THIRD_PARTY%\openssl\inc32;%INCLUDE%
set LIB=%PICTO_THIRD_PARTY%\openssl\lib;%LIB%
set QTTOP=%PICTO_THIRD_PARTY%\%PICTO_QT_INSTALL_TOP_DIR%\Qt5.3.2\5.3
set PATH=%PICTO_THIRD_PARTY%\openssl\bin;%QTTOP%\%PICTO_QT_MSVC_DIR%\bin;%PATH%
set QTDIR=%QTTOP%\%PICTO_QT_MSVC_DIR%
set PROPBROWSDIR=%QTTOP%\Src\qttools\src\shared\qtpropertybrowser
set QWTDIR=%PICTO_THIRD_PARTY%\%PICTO_QWT_DIR%
set PHIDGETSDIR=%PICTO_THIRD_PARTY%\phidgets
set OPENSSLDIR=%PICTO_THIRD_PARTY%\openssl
@echo If desired, run the dx_setenv.cmd script from the Microsoft DirectX SDK
@echo Note that the official build does not compile Qt with DirectX enabled

