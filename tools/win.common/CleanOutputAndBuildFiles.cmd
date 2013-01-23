@IF NOT DEFINED PICTO_ENV_SET (
  @echo Picto environment hasn't been configured
  @echo Run the appropriate configure script for your platform from the Picto root directory and try again
) ELSE (
  pushd %PICTO_TREE%

  @echo Cleaning Makefiles

  @del makefile /s /q
  @del makefile.release /s /q
  @del makefile.debug /s /q
  @del vc90.pdb /s /q

  @echo Cleaning Visual Studio Projects

  @del *.ncb /s /q
  @del *.aps /s /q
  @del *.sln /s /q
  @del *.vcproj /s /q 
  @del *.vcproj.* /s /q
  @del *.vcxproj /s /q 
  @del *.vcxproj.* /s /q
  @attrib *.suo -h
  @del *.suo /q

  @echo Cleaning Translation Object Files
  @del *.qm /s /q

  @echo Cleaning Intermediate Files
  @rd intermediates /s /q

  @echo Cleaning Output
  @rd output /s /q

  popd
)