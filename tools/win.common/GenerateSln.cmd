@IF NOT DEFINED PICTO_ENV_SET (
  @echo Picto environment hasn't been configured
  @echo Run the appropriate configure script for your platform from the Picto root directory and try again
) ELSE (
  @echo Generating Translation Object Files
  pushd %PICTO_TREE%

  FOR /R source %%T IN (*.ts) DO lrelease %%T

  @echo Generating Makefiles
  @qmake -recursive
  
  @echo Generating Visual Studio Solution
  @qmake -recursive -tp vc

  REM We used the vcprojFilters application below in Visual Studio 2008 and before to setup the visual studio directory structure.  
  REM This broke in VS2010, but we now use "CONFIG -= flat" in the Picto.pro file, which takes care of giving
  REM us a fairly clean structure
  REM @echo Organizing Source Code in Visual Studio Solution
  REM FOR /R source %%T IN (*.vcxproj) DO @%PICTO_TREE%\tools\win.common\vcprojFilters\bin\vcprojFilters %%T

  @echo Setting Dependencies in Visual Studio Solution
    
  %PICTO_TREE%\tools\win.common\vcSlnDependencies\bin\vcSlnDependencies Picto.sln libPicto

  @echo If using nmake, run 'nmake install' following the build to prepare the application for distribution
  popd
)