@IF NOT DEFINED PICTO_ENV_SET (
  @echo Picto environment hasn't been configured
  @echo Run the appropriate configure script for your platform from the Picto root directory and try again
  EXIT /B
)

@REM SETLOCAL creates a local scope for environment variables
@REM Just doing this to contain the local, build-dependent variables
@SETLOCAL

@IF "%PICTO_MACHINE_TYPE%"=="X86" SET DOXYGEN_DIR=%PICTO_TREE%\tools\doxygen\win32

@IF "%PICTO_MACHINE_TYPE%"=="X64" SET DOXYGEN_DIR=%PICTO_TREE%\tools\doxygen\win64


@echo Generating Translation Object Files
pushd %PICTO_TREE%
FOR /R source %%T IN (*.ts) DO lrelease %%T


@echo Generating Documentation
@call %PICTO_TREE%\tools\win.common\GenerateDocumentation.cmd


@echo Generating Documentation Resource File
%DOXYGEN_DIR%\docQrcGen %PICTO_TREE%\source\documentation\developersguide\developersguide.qrc %PICTO_TREE%\output\documentation\main\html ../../../output/documentation/main/html/


@echo Generating Visual Studio Solution
@qmake -recursive -tp vc


@REM No special instructions for X86
@REM IF "%PICTO_MACHINE_TYPE%" == "X86" (
@REM )

@REM We rename the solution for 64-bit 
@IF "%PICTO_MACHINE_TYPE%" == "X64" (
  @del "Picto.sln"
  @qmake -tp vc -o Picto_x64.sln 
)


@REM We used the vcprojFilters application below in Visual Studio 2008 and before to setup the visual studio directory structure.  
@REM This broke in VS2010, but we now use "CONFIG -= flat" in the Picto.pro file, which takes care of giving
@REM us a fairly clean structure
@REM @echo Organizing Source Code in Visual Studio Solution
@REM FOR /R source %%T IN (*.vcxproj) DO @%PICTO_TREE%\tools\win.common\vcprojFilters\bin\vcprojFilters %%T

@echo Setting Dependencies in Visual Studio Solution
  
%PICTO_TREE%\tools\win.common\vcSlnDependencies\bin\vcSlnDependencies Picto.sln libPicto

popd
@ENDLOCAL
