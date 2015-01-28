@IF NOT DEFINED PICTO_ENV_SET (
  echo Picto environment hasn't been configured
  echo Run the appropriate configure script for your platform from the Picto root directory and try again
) ELSE (
  @ECHO OFF
  pushd %PICTO_TREE%

  echo Getting Qt libraries
  ROBOCOPY %QTDIR%\bin %PICTO_TREE%\output\bin\release Qt*Core.dll Qt*Gui.dll Qt*Script.dll Qt*Xml.dll Qt*Network.dll Qt*Sql.dll Qt*Svg.dll Qt*ScriptTools.dll Qt*Widgets.dll Qt*Multimedia.dll Qt*OpenGl.dll Qt*PrintSupport.dll Qt*Test.dll libGLESv2.dll libEGL.dll /NFL /NDL /NJH /NJS

  ROBOCOPY %QTDIR%\bin %PICTO_TREE%\output\bin\debug Qt*Cored.dll Qt*Guid.dll Qt*Scriptd.dll Qt*Xmld.dll Qt*Networkd.dll Qt*Sqld.dll Qt*Svgd.dll Qt*ScriptToolsd.dll Qt*Widgetsd.dll Qt*Multimediad.dll Qt*OpenGld.dll Qt*PrintSupportd.dll Qt*Testd.dll libGLESv2d.dll libEGLd.dll Qt*Cored.pdb Qt*Guid.pdb Qt*Scriptd.pdb Qt*Xmld.pdb Qt*Networkd.pdb Qt*Sqld.pdb Qt*Svgd.pdb Qt*ScriptToolsd.pdb Qt*Widgetsd.pdb Qt*Multimediad.pdb Qt*OpenGld.pdb Qt*PrintSupportd.pdb Qt*Testd.pdb libGLESv2d.pdb libEGLd.pdb /NFL /NDL /NJH /NJS

  ROBOCOPY %QTDIR%\bin %PICTO_TREE%\output\bin\release icuin*.dll icuuc*.dll icudt*.dll D3DCompiler_43.dll /NFL /NDL /NJH /NJS

  ROBOCOPY %QTDIR%\bin %PICTO_TREE%\output\bin\debug icuin*.dll icuuc*.dll icudt*.dll D3DCompiler_43.dll /NFL /NDL /NJH /NJS

  ROBOCOPY %QTDIR%\plugins\platforms %PICTO_TREE%\output\bin\release\platforms qwindows.dll /NFL /NDL /NJH /NJS

  ROBOCOPY %QTDIR%\plugins\platforms %PICTO_TREE%\output\bin\debug\platforms qwindowsd.dll /NFL /NDL /NJH /NJS

  ROBOCOPY %QTDIR%\plugins\sqldrivers %PICTO_TREE%\output\bin\release\sqldrivers qsqlite.dll /NFL /NDL /NJH /NJS

  ROBOCOPY %QTDIR%\plugins\sqldrivers %PICTO_TREE%\output\bin\debug\sqldrivers qsqlited.dll /NFL /NDL /NJH /NJS

  ROBOCOPY %QTDIR%\plugins\imageformats %PICTO_TREE%\output\bin\release\imageformats qico.dll qsvg.dll /NFL /NDL /NJH /NJS

  ROBOCOPY %QTDIR%\plugins\imageformats %PICTO_TREE%\output\bin\debug\imageformats qicod.dll qsvgd.dll /NFL /NDL /NJH /NJS

  ROBOCOPY %PICTO_TREE%\3rdparty\bin %PICTO_TREE%\output\bin\release\plugins PlexClient.dll /NFL /NDL /NJH /NJS

  ROBOCOPY %PICTO_TREE%\3rdparty\bin %PICTO_TREE%\output\bin\debug\plugins PlexClient.dll /NFL /NDL /NJH /NJS

  ROBOCOPY %QTDIR%\bin %PICTO_TREE%\output\tests\bin\release Qt*Core.dll Qt*Gui.dll Qt*Script.dll Qt*Xml.dll Qt*Network.dll Qt*Sql.dll Qt*Svg.dll Qt*ScriptTools.dll Qt*Widgets.dll Qt*Multimedia.dll Qt*OpenGl.dll Qt*PrintSupport.dll Qt*Test.dll libGLESv2.dll libEGL.dll icuin*.dll icuuc*.dll icudt*.dll /NFL /NDL /NJH /NJS

  ROBOCOPY %QTDIR%\bin %PICTO_TREE%\output\tests\bin\debug Qt*Cored.dll Qt*Guid.dll Qt*Scriptd.dll Qt*Xmld.dll Qt*Networkd.dll Qt*Sqld.dll Qt*Svgd.dll Qt*ScriptToolsd.dll Qt*Widgetsd.dll Qt*Multimediad.dll Qt*OpenGld.dll Qt*PrintSupportd.dll Qt*Testd.dll libGLESv2d.dll libEGLd.dll icuin*.dll icuuc*.dll icudt*.dll /NFL /NDL /NJH /NJS

 REM echo Getting Qt Property Browser Libraries
 REM ROBOCOPY %PROPBROWSDIR%\bin\release %PICTO_TREE%\output\bin\release *.dll /NFL /NDL /NJH /NJS
 REM tooROBOCOPY %PROPBROWSDIR%\bin\debug %PICTO_TREE%\output\bin\debug *d.dll /NFL /NDL /NJH /NJS

  echo Getting Qwt Libraries
  ROBOCOPY %QWTDIR%\lib %PICTO_TREE%\output\bin\release *.dll /NFL /NDL /NJH /NJS
  ROBOCOPY %QWTDIR%\lib %PICTO_TREE%\output\bin\debug *d.dll /NFL /NDL /NJH /NJS

  echo Getting Phidget Libraries
  ROBOCOPY %PHIDGETSDIR%\x64 %PICTO_TREE%\output\bin\release *.dll /NFL /NDL /NJH /NJS
  ROBOCOPY %PHIDGETSDIR%\x64 %PICTO_TREE%\output\bin\debug *.dll /NFL /NDL /NJH /NJS

  echo Getting FFMPEG Libraries
  ROBOCOPY %PICTO_TREE%/3rdparty/QTFFmpegWrapper/ffmpeg_lib_win64 %PICTO_TREE%\output\bin\release *.dll /NFL /NDL /NJH /NJS
  ROBOCOPY %PICTO_TREE%/3rdparty/QTFFmpegWrapper/ffmpeg_lib_win64 %PICTO_TREE%\output\bin\debug *.dll /NFL /NDL /NJH /NJS

  popd
)