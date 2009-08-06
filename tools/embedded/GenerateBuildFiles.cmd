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

popd
)