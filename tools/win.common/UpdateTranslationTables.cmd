@IF NOT DEFINED PICTO_ENV_SET (
  @echo Picto environment hasn't been configured
  @echo Run the appropriate configure script for your platform from the Picto root directory and try again
) ELSE (
  @REM Do not call lupdate on the root project file.
  @REM While it will parse all the sub-project files, it will add translation strings found in
  @REM earlier sub-projects into the later sub-projects.
  @REM Instead, we parse all the sub-project files individually.

  @echo Updating Translation Tables
  FOR /R %PICTO_TREE%\source %%T IN (*.pro) DO lupdate %%T
)