@set PICTO_TREE=%CD%
@set PICTO_THIRD_PARTY=C:
@REM TODO: Before shipping, change the above line to the below line
@REM       We only set it to the root during development since we'll
@REM       frequently be deleting output files
@REM @set PICTO_THIRD_PARTY=%PICTO_TREE%\3rdparty
@set PICTO_MACHINE_TYPE=X86
@set PICTO_ENV_SET=TRUE

@echo Picto environment set
@echo Be sure the build environment for your compiler and Qt is also prepared
@echo Use the GenerateBuildFiles.cmd script in tools\win.common to build
