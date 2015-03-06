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

@echo Generating Standard Documentation
@IF EXIST manuals\CodeDoc\html (
  RD /S /Q manuals\CodeDoc\html
)
%DOXYGEN_DIR%\doxygen tools\doxygen\PictoMainDoxyConfig 2>DocBuildErr.txt

@IF NOT EXIST output\documentation (
  mkdir output\documentation
)
@echo Generating Unit Tests Documentation
@IF NOT EXIST output\documentation\unittests (
  mkdir output\documentation\unittests
)
%DOXYGEN_DIR%\doxygen tools\doxygen\PictoUnitTestsDoxyConfig 2>output\documentation\unittests\DocBuildErr.txt

@echo.
@echo.
@echo NOTE:
@echo DoxyGen errors for the project have been redirected to %PICTO_TREE%\DocBuildErr.txt
@echo Check there for any documentation issues
@echo.
@popd
@ENDLOCAL
