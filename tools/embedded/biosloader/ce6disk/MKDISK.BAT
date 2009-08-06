@REM
@REM Copyright (c) Microsoft Corporation.  All rights reserved.
@REM
@REM
@REM Use of this source code is subject to the terms of the Microsoft end-user
@REM license agreement (EULA) under which you licensed this SOFTWARE PRODUCT.
@REM If you did not accept the terms of the EULA, you are not authorized to use
@REM this source code. For a copy of the EULA, please see the LICENSE.RTF on your
@REM install media.
@REM
@REM
@REM This batch file prepares a storage device with the Windows CE BIOS
@REM bootloader.  It assumes the storage device has been properly partitioned
@REM and formatted first (disk should be clean).
@REM
@REM Usage: argv[0] drive
@REM
@REM drive  - drive letter of storage device (a:, c:, etc.).
@REM
@echo off


@REM Display a MKDisk version number to the user

set drv=%1
if (%1) == () set drv=c:

set fs=bl_%2
if (%2) == () set fs=bldr

echo BiosLoader MKDISK Version 1.2.00

echo Preparing %drv% to run biosloader

@REM Check to see if the bootsector image is available and, if so,
@REM place it onto the destination drive

:Bsect
if exist bsect.img goto Bldr
echo ERROR: Cannot find boot sector image (bsect.img).
goto Done

:Bldr
if exist %fs% goto Xfer:
echo ERROR: Cannot find bootloader image (%fs%).
goto Done

:Xfer
echo Preparing the boot sector of %drv%
if exist %drv%\bldr attrib -r -s -h %drv%\bldr
cesys -b:62 bsect.img %fs% %drv%
if exist %drv%\bldr attrib +r +s +h %drv%\bldr


@REM Now copy the remaining configuration files to the destination drive

if exist boot.ini goto BootIni
echo ERROR: Cannot find boot config file (boot.ini).
goto Done

:BootIni
if exist %drv%\boot.ini attrib -r -s -h %drv%\boot.ini
echo Copying boot.ini to %drv%
copy boot.ini %drv% > NUL

if exist splash.bmx goto SplashBmx
echo ERROR: Cannot find splash screen image (splash.bmx).
goto Done

:SplashBmx
if exist %drv%\splash.bmx attrib -r -s -h %drv%\splash.bmx
echo Copying splash.bmx to %drv%
copy splash.bmx %drv% > NUL

if exist eboot.bix goto EbootBix
echo ERROR: Cannot find eboot bootloader (eboot.bix).
goto Done

:EbootBix
if exist %drv%\eboot.bix attrib -r -s -h %drv%\eboot.bix
echo Copying eboot.bix to %drv%
copy eboot.bix %drv% > NUL

echo %drv% is setup to boot nk.bin or eboot.bix if nk.bin is not present.
echo Edit boot.ini if you wish to change these boot settings.
echo Please remove the floppy disk and reset system for changes to take affect.

:Done
