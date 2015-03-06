@IF NOT DEFINED PICTO_ENV_SET (
  @echo Picto environment hasn't been configured
  @echo Run the appropriate configure script for your platform from the Picto root directory and try again
  EXIT /B
)

@REM SETLOCAL creates a local scope for environment variables
@REM Just doing this to contain the local, build-dependent variables
@SETLOCAL

@IF "%PICTO_MACHINE_TYPE%" == "X86" (
  @SET DOXYGEN_DIR=%PICTO_TREE%\tools\doxygen\win32
)
@IF "%PICTO_MACHINE_TYPE%" == "X64" (
  @SET DOXYGEN_DIR=%PICTO_TREE%\tools\doxygen\win64
)


@echo Generating Translation Object Files
pushd %PICTO_TREE%
FOR /R source %%T IN (*.ts) DO lrelease %%T


@echo Generating Documentation
@call %PICTO_TREE%\tools\win.common\GenerateDocumentation.cmd


@echo Generating Makefiles
@qmake -recursive


@echo Run 'nmake install' following the build to prepare the application for distribution
popd
@ENDLOCAL
