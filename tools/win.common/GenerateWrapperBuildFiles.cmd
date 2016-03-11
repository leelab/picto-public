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
  @SET DEPENDENCY_FIX_DIR=vcSlnDependencies\x86\bin
  @SET PICTO_SOLUTION_NAME=Picto.sln
)

@echo Generating Visual Studio Solution
@qmake -recursive -tp vc PlexonWrapper.pro

popd
@ENDLOCAL
