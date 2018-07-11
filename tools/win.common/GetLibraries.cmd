@IF NOT DEFINED PICTO_ENV_SET (
  @echo Picto environment hasn't been configured
  @echo Run the appropriate configure script for your platform from the Picto root directory and try again
  EXIT /B
)

@ECHO OFF

SETLOCAL

pushd %PICTO_TREE%

@IF "%PICTO_MACHINE_TYPE%" == "X86" (
  echo Getting Qt libraries
  ROBOCOPY %QTDIR%\bin %PICTO_TREE%\output\bin\release\wrapper Qt?Core.dll /NFL /NDL /NJH /NJS
  ROBOCOPY %QTDIR%\bin %PICTO_TREE%\output\bin\debug\wrapper Qt?Cored.dll Qt?Cored.pdb /NFL /NDL /NJH /NJS
  
  ROBOCOPY %PICTO_TREE%\3rdparty\bin %PICTO_TREE%\output\bin\release\wrapper PlexClient.dll /NFL /NDL /NJH /NJS
  ROBOCOPY %PICTO_TREE%\3rdparty\bin %PICTO_TREE%\output\bin\debug\wrapper PlexClient.dll /NFL /NDL /NJH /NJS
  
  ROBOCOPY %QTDIR%\plugins\platforms %PICTO_TREE%\output\bin\release\wrapper qwindows.dll /NFL /NDL /NJH /NJS
  ROBOCOPY %QTDIR%\plugins\platforms %PICTO_TREE%\output\bin\debug\wrapper qwindowsd.dll /NFL /NDL /NJH /NJS
)

@IF "%PICTO_MACHINE_TYPE%" == "X64" (
  echo Getting Qt libraries
  ROBOCOPY %QTDIR%\bin %PICTO_TREE%\output\bin\release Qt*Core.dll Qt*Gui.dll Qt*Script.dll Qt*Xml.dll Qt*Network.dll Qt*Sql.dll Qt*Svg.dll Qt*ScriptTools.dll Qt*Widgets.dll Qt*Multimedia.dll Qt*OpenGl.dll Qt*PrintSupport.dll Qt*Test.dll Qt*WinExtras.dll libGLESv2.dll libEGL.dll /NFL /NDL /NJH /NJS
  
  ROBOCOPY %QTDIR%\bin %PICTO_TREE%\output\bin\debug Qt*Cored.dll Qt*Guid.dll Qt*Scriptd.dll Qt*Xmld.dll Qt*Networkd.dll Qt*Sqld.dll Qt*Svgd.dll Qt*ScriptToolsd.dll Qt*Widgetsd.dll Qt*Multimediad.dll Qt*OpenGld.dll Qt*PrintSupportd.dll Qt*Testd.dll Qt*WinExtrasd.dll libGLESv2d.dll libEGLd.dll Qt*Cored.pdb Qt*Guid.pdb Qt*Scriptd.pdb Qt*Xmld.pdb Qt*Networkd.pdb Qt*Sqld.pdb Qt*Svgd.pdb Qt*ScriptToolsd.pdb Qt*Widgetsd.pdb Qt*Multimediad.pdb Qt*OpenGld.pdb Qt*PrintSupportd.pdb Qt*Testd.pdb Qt*WinExtrasd.pdb libGLESv2d.pdb libEGLd.pdb /NFL /NDL /NJH /NJS
  
  ROBOCOPY %QTDIR%\bin %PICTO_TREE%\output\bin\release icuin*.dll icuuc*.dll icudt*.dll D3DCompiler_*.dll /NFL /NDL /NJH /NJS
  
  ROBOCOPY %QTDIR%\bin %PICTO_TREE%\output\bin\debug icuin*.dll icuuc*.dll icudt*.dll D3DCompiler_*.dll /NFL /NDL /NJH /NJS
  
  ROBOCOPY %QTDIR%\plugins\platforms %PICTO_TREE%\output\bin\release\platforms qwindows.dll /NFL /NDL /NJH /NJS
  
  ROBOCOPY %QTDIR%\plugins\platforms %PICTO_TREE%\output\bin\debug\platforms qwindowsd.dll /NFL /NDL /NJH /NJS
  
  ROBOCOPY %QTDIR%\plugins\sqldrivers %PICTO_TREE%\output\bin\release\sqldrivers qsqlite.dll /NFL /NDL /NJH /NJS
  
  ROBOCOPY %QTDIR%\plugins\sqldrivers %PICTO_TREE%\output\bin\debug\sqldrivers qsqlited.dll /NFL /NDL /NJH /NJS
  
  ROBOCOPY %QTDIR%\plugins\imageformats %PICTO_TREE%\output\bin\release\imageformats qico.dll qsvg.dll /NFL /NDL /NJH /NJS
  
  ROBOCOPY %QTDIR%\plugins\imageformats %PICTO_TREE%\output\bin\debug\imageformats qicod.dll qsvgd.dll /NFL /NDL /NJH /NJS
  
  ROBOCOPY %QTDIR%\plugins\audio %PICTO_TREE%\output\bin\release\audio qtaudio_windows.dll /NFL /NDL /NJH /NJS
  
  ROBOCOPY %QTDIR%\plugins\audio %PICTO_TREE%\output\bin\debug\audio qtaudio_windowsd.dll qtaudio_windowsd.pdb /NFL /NDL /NJH /NJS
  
  
  
  echo Getting Qwt Libraries
  ROBOCOPY %QWTDIR%\lib %PICTO_TREE%\output\bin\release *.dll /NFL /NDL /NJH /NJS
  ROBOCOPY %QWTDIR%\lib %PICTO_TREE%\output\bin\debug *d.dll /NFL /NDL /NJH /NJS
  
  echo Getting Phidget Libraries
  ROBOCOPY %PHIDGETSDIR%\phidgets22\x64 %PICTO_TREE%\output\bin\release *.dll /NFL /NDL /NJH /NJS
  ROBOCOPY %PHIDGETSDIR%\phidgets22\x64 %PICTO_TREE%\output\bin\debug *.dll /NFL /NDL /NJH /NJS
 ROBOCOPY %PHIDGETSDIR%\phidgets21\x64 %PICTO_TREE%\output\bin\release *.dll /NFL /NDL /NJH /NJS
  ROBOCOPY %PHIDGETSDIR%\phidgets21\x64 %PICTO_TREE%\output\bin\debug *.dll /NFL /NDL /NJH /NJS
  
  echo Getting FFMPEG Libraries
  ROBOCOPY %PICTO_TREE%/3rdparty/QTFFmpegWrapper/ffmpeg_lib_win64 %PICTO_TREE%\output\bin\release *.dll /NFL /NDL /NJH /NJS
  ROBOCOPY %PICTO_TREE%/3rdparty/QTFFmpegWrapper/ffmpeg_lib_win64 %PICTO_TREE%\output\bin\debug *.dll /NFL /NDL /NJH /NJS
)

popd
ENDLOCAL
