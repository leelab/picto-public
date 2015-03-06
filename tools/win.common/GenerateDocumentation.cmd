@IF NOT DEFINED PICTO_ENV_SET (
  @echo Picto environment hasn't been configured
  @echo Run the appropriate configure script for your platform from the Picto root directory and try again
  EXIT /B
)

@REM SETLOCAL creates a local scope for environment variables
@REM Just doing this to contain the local, build-dependent variables
@SETLOCAL

@IF "%PICTO_MACHINE_TYPE%" == "X86" (
  @SET DOXYGEN_DIR=tools\doxygen\win32
)
@IF "%PICTO_MACHINE_TYPE%" == "X64" (
  @SET DOXYGEN_DIR=tools\doxygen\win64
)

pushd %PICTO_TREE%

@IF NOT EXIST output\documentation (
  mkdir output\documentation
)


@echo Generating Standard Documentation
@IF NOT EXIST output\documentation\main (
  mkdir output\documentation\main
)
%DOXYGEN_DIR%\doxygen tools\doxygen\PictoMainDoxyConfig


@echo Generating Unit Tests Documentation
@IF NOT EXIST output\documentation\unittests (
  mkdir output\documentation\unittests
)
%DOXYGEN_DIR%\doxygen tools\doxygen\PictoUnitTestsDoxyConfig


popd
@ENDLOCAL
