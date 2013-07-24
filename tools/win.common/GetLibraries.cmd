@IF NOT DEFINED PICTO_ENV_SET (
  echo Picto environment hasn't been configured
  echo Run the appropriate configure script for your platform from the Picto root directory and try again
) ELSE (
  @ECHO OFF
  pushd %PICTO_TREE%

  echo Getting Qt libraries
  ROBOCOPY %QTDIR%\bin %PICTO_TREE%\output\bin\release Qt5Core.dll Qt5Gui.dll Qt5Script.dll Qt5Xml.dll Qt5Network.dll Qt5Sql.dll Qt5Svg.dll Qt5ScriptTools.dll Qt5Widgets.dll Qt5Multimedia.dll Qt5OpenGl.dll Qt5PrintSupport.dll Qt5Test.dll libGLESv2.dll libEGL.dll /NFL /NDL /NJH /NJS

  ROBOCOPY %QTDIR%\bin %PICTO_TREE%\output\bin\debug Qt5Cored.dll Qt5Guid.dll Qt5Scriptd.dll Qt5Xmld.dll Qt5Networkd.dll Qt5Sqld.dll Qt5Svgd.dll Qt5ScriptToolsd.dll Qt5Widgetsd.dll Qt5Multimediad.dll Qt5OpenGld.dll Qt5PrintSupportd.dll Qt5Testd.dll libGLESv2d.dll libEGLd.dll Qt5Cored.pdb Qt5Guid.pdb Qt5Scriptd.pdb Qt5Xmld.pdb Qt5Networkd.pdb Qt5Sqld.pdb Qt5Svgd.pdb Qt5ScriptToolsd.pdb Qt5Widgetsd.pdb Qt5Multimediad.pdb Qt5OpenGld.pdb Qt5PrintSupportd.pdb Qt5Testd.pdb libGLESv2d.pdb libEGLd.pdb /NFL /NDL /NJH /NJS

  ROBOCOPY %QTDIR%\bin %PICTO_TREE%\output\bin\release icuin49.dll icuuc49.dll icudt49.dll D3DCompiler_43.dll /NFL /NDL /NJH /NJS

  ROBOCOPY %QTDIR%\bin %PICTO_TREE%\output\bin\debug icuin49.dll icuuc49.dll icudt49.dll D3DCompiler_43.dll /NFL /NDL /NJH /NJS

  ROBOCOPY %QTDIR%\plugins\platforms %PICTO_TREE%\output\bin\release\platforms qwindows.dll /NFL /NDL /NJH /NJS

  ROBOCOPY %QTDIR%\plugins\platforms %PICTO_TREE%\output\bin\debug\platforms qwindowsd.dll /NFL /NDL /NJH /NJS

  ROBOCOPY %QTDIR%\plugins\sqldrivers %PICTO_TREE%\output\bin\release\sqldrivers qsqlite.dll /NFL /NDL /NJH /NJS

  ROBOCOPY %QTDIR%\plugins\sqldrivers %PICTO_TREE%\output\bin\debug\sqldrivers qsqlited.dll /NFL /NDL /NJH /NJS

  ROBOCOPY %QTDIR%\plugins\imageformats %PICTO_TREE%\output\bin\release\imageformats qico.dll qsvg.dll /NFL /NDL /NJH /NJS

  ROBOCOPY %QTDIR%\plugins\imageformats %PICTO_TREE%\output\bin\debug\imageformats qicod.dll qsvgd.dll /NFL /NDL /NJH /NJS

  ROBOCOPY %PICTO_TREE%\3rdparty\bin %PICTO_TREE%\output\bin\release\plugins PlexClient.dll /NFL /NDL /NJH /NJS

  ROBOCOPY %PICTO_TREE%\3rdparty\bin %PICTO_TREE%\output\bin\debug\plugins PlexClient.dll /NFL /NDL /NJH /NJS

  ROBOCOPY %QTDIR%\lib %PICTO_TREE%\output\tests\bin\release Qt5Core.dll Qt5Gui.dll Qt5Script.dll Qt5Xml.dll Qt5Network.dll Qt5Sql.dll Qt5Svg.dll Qt5ScriptTools.dll Qt5Widgets.dll Qt5Multimedia.dll Qt5OpenGl.dll Qt5PrintSupport.dll Qt5Test.dll libGLESv2.dll libEGL.dll icuin49.dll icuuc49.dll icudt49.dll /NFL /NDL /NJH /NJS

  ROBOCOPY %QTDIR%\lib %PICTO_TREE%\output\tests\bin\debug Qt5Cored.dll Qt5Guid.dll Qt5Scriptd.dll Qt5Xmld.dll Qt5Networkd.dll Qt5Sqld.dll Qt5Svgd.dll Qt5ScriptToolsd.dll Qt5Widgetsd.dll Qt5Multimediad.dll Qt5OpenGld.dll Qt5PrintSupportd.dll Qt5Testd.dll libGLESv2d.dll libEGLd.dll icuin49.dll icuuc49.dll icudt49.dll /NFL /NDL /NJH /NJS

 REM echo Getting Qt Property Browser Libraries
 REM ROBOCOPY %PROPBROWSDIR%\bin\release %PICTO_TREE%\output\bin\release *.dll /NFL /NDL /NJH /NJS
 REM tooROBOCOPY %PROPBROWSDIR%\bin\debug %PICTO_TREE%\output\bin\debug *d.dll /NFL /NDL /NJH /NJS

  echo Getting Qwt Libraries
  ROBOCOPY %QWTDIR%\lib %PICTO_TREE%\output\bin\release *.dll /NFL /NDL /NJH /NJS
  ROBOCOPY %QWTDIR%\lib %PICTO_TREE%\output\bin\debug *d.dll /NFL /NDL /NJH /NJS

  echo Getting Phidget Libraries
  ROBOCOPY %PHIDGETSDIR%\x86 %PICTO_TREE%\output\bin\release *.dll /NFL /NDL /NJH /NJS
  ROBOCOPY %PHIDGETSDIR%\x86 %PICTO_TREE%\output\bin\debug *.dll /NFL /NDL /NJH /NJS

  echo Getting FFMPEG Libraries
  ROBOCOPY %PICTO_TREE%/3rdparty/QTFFmpegWrapper/ffmpeg_lib_win32 %PICTO_TREE%\output\bin\release *.dll /NFL /NDL /NJH /NJS
  ROBOCOPY %PICTO_TREE%/3rdparty/QTFFmpegWrapper/ffmpeg_lib_win32 %PICTO_TREE%\output\bin\debug *.dll /NFL /NDL /NJH /NJS

  popd
)