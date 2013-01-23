@IF NOT DEFINED PICTO_ENV_SET (
  echo Picto environment hasn't been configured
  echo Run the appropriate configure script for your platform from the Picto root directory and try again
) ELSE (
  @ECHO OFF
  pushd %PICTO_TREE%

  echo Getting Qt libraries
  ROBOCOPY %QTDIR%\bin %PICTO_TREE%\output\bin\release Qt5Core.dll Qt5Gui.dll Qt5Script.dll Qt5Xml.dll   Qt5Network.dll Qt5Sql.dll Qt5Svg.dll Qt5ScriptTools.dll Qt5Widgets.dll Qt5Multimedia.dll Qt5OpenGl.dll Qt5PrintSupport.dll Qt5Test.dll libGLESv2.dll libEGL.dll icuin49.dll icuuc49.dll icudt49.dll /NFL /NDL /NJH /NJS

  ROBOCOPY %QTDIR%\bin %PICTO_TREE%\output\bin\debug Qt5Cored.dll Qt5Guid.dll Qt5Scriptd.dll Qt5Xmld.dll Qt5Networkd.dll Qt5Sqld.dll Qt5Svgd.dll Qt5ScriptToolsd.dll Qt5Widgetsd.dll Qt5Multimediad.dll Qt5OpenGld.dll Qt5PrintSupportd.dll Qt5Testd.dll libGLESv2d.dll libEGLd.dll icuin49.dll icuuc49.dll icudt49.dll /NFL /NDL /NJH /NJS

  ROBOCOPY %QTDIR%\bin %PICTO_TREE%\output\tests\bin\release Qt5Core.dll Qt5Gui.dll Qt5Script.dll Qt5Xml.dll   Qt5Network.dll Qt5Sql.dll Qt5Svg.dll Qt5ScriptTools.dll Qt5Widgets.dll Qt5Multimedia.dll Qt5OpenGl.dll Qt5PrintSupport.dll Qt5Test.dll libGLESv2.dll libEGL.dll icuin49.dll icuuc49.dll icudt49.dll /NFL /NDL /NJH /NJS

  ROBOCOPY %QTDIR%\bin %PICTO_TREE%\output\tests\bin\debug Qt5Cored.dll Qt5Guid.dll Qt5Scriptd.dll Qt5Xmld.dll Qt5Networkd.dll Qt5Sqld.dll Qt5Svgd.dll Qt5ScriptToolsd.dll Qt5Widgetsd.dll Qt5Multimediad.dll Qt5OpenGld.dll Qt5PrintSupportd.dll Qt5Testd.dll libGLESv2d.dll libEGLd.dll icuin49.dll icuuc49.dll icudt49.dll /NFL /NDL /NJH /NJS

 REM echo Getting Qt Property Browser Libraries
 REM ROBOCOPY %PROPBROWSDIR%\bin\release %PICTO_TREE%\output\bin\release *.dll /NFL /NDL /NJH /NJS
 REM tooROBOCOPY %PROPBROWSDIR%\bin\debug %PICTO_TREE%\output\bin\debug *d.dll /NFL /NDL /NJH /NJS

  echo Getting Qwt Libraries
  ROBOCOPY %QWTDIR%\lib %PICTO_TREE%\output\bin\release *.dll /NFL /NDL /NJH /NJS
  ROBOCOPY %QWTDIR%\lib %PICTO_TREE%\output\bin\debug *d.dll /NFL /NDL /NJH /NJS

  echo Getting Phidget Libraries
  ROBOCOPY %PHIDGETSDIR%\x86 %PICTO_TREE%\output\bin\release *.dll /NFL /NDL /NJH /NJS
  ROBOCOPY %PHIDGETSDIR%\x86 %PICTO_TREE%\output\bin\debug *.dll /NFL /NDL /NJH /NJS

  popd
)