@IF NOT DEFINED PICTO_ENV_SET (
  @echo Picto environment hasn't been configured
  @echo Run the appropriate configure script for your platform from the Picto root directory and try again
) ELSE (
  pushd %PICTO_TREE%

  @IF NOT EXIST output\documentation (
    mkdir output\documentation
  )

  @echo Generating Standard Documentation
  @IF NOT EXIST output\documentation\main (
    mkdir output\documentation\main
  )
  tools\doxygen\win64\doxygen tools\doxygen\PictoMainDoxyConfig

  @echo Generating Unit Tests Documentation
  @IF NOT EXIST output\documentation\unittests (
    mkdir output\documentation\unittests
  )
  tools\doxygen\win64\doxygen tools\doxygen\PictoUnitTestsDoxyConfig

  popd
)