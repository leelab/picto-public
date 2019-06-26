
/*! \page build_environment_preparation Create a Picto Development Environment

\section assumptions Assumptions

 - You are running Windows
 - You have a valid Github account that has been added to the Picto project on Github.  If you don't have one already, set up an account through Github.  After that, Leelab has a Leelab.Admin account with administration control that can be used to give your account access to the repository.

\section installing_prerequisites Installing Prerequisite Applications

\subsection get_install_binary Get and Install Binary Distributions

Just about everything you need to create an old version of the development environment (apart from the actual Picto source) is available in \\ganglia\\it\\Software\\Picto\\DevelopmentEnvironment if you're on the Leelab network.  To build the latest and greatest version based on newer versions of the various libraries underlying Picto, you will need to use the links throughout this document to download locations for each required software package.

 -# Install Visual Studio 2013.

 -# Install the latest version of Tortoise Git.  This is currently available at https://code.google.com/p/tortoisegit/.

 -# Install Qt 5.5.1 for Windows 32-bit (VS 2013, 804 MB). This is currently available via http://download.qt.io/archive/qt/5.5/5.5.1/qt-opensource-windows-x86-msvc2013-5.5.1.exe.  When asked, specify that Qt 5.5.1 be installed at c:\\Qt\\Qt5.5.1. When you reach the "Select Components" window, use the "Select All" button to make sure that all components are selected (this is not the default setting).  The 32-bit package is required for the Plexon library wrapper.

 -# Install Qt 5.5.1 for Windows 64-bit (VS 2013, 823 MB). This is currently available at http://download.qt.io/archive/qt/5.5/5.5.1/qt-opensource-windows-x86-msvc2013_64-5.5.1.exe.  When asked, specify that Qt 5.5.1 be installed at c:\\Qt64\\Qt5.5.1. When you reach the "Select Components" window, use the "Select All" button to make sure that all components are selected (this is not the default setting).  The 64-bit package is required for Picto itself.

 -# Install the Ni-Daq 18.6 API.  The Picto director uses a National Instruments card and the Director needs to link to this card's libraries.  The Ni-Daq 9.1 installer is currently available at: http://www.ni.com/en-us/support/downloads/drivers/download.ni-daqmx.html.  Even as of this writing, this is not the newest version.  You are welcome to (encouraged) to install the newest version of the API; however, be warned that this may require some upgrade of the Picto code if their have been changes in the Ni-Daq API. Note that this should be done after Visual Studio 2013 is installed, to ensure Visual Studio has its include paths set correctly.

 -# Install the Phidgets libraries.  These are the libraries used by the Director to control the Pictobox's LCD display and input dial. There are two of these libraries, for two different generations of Phidget devices.  In order to have one build environment work for both older and newer Picto builds, the older libraries is duplicated in two different places.

  -# Get the Phidgets21 library zip.  It is available at: https://www.phidgets.com/downloads/phidget21/libraries/windows/Phidget21-windevel.zip as of this writing.

  -# Copy the top level directory inside the zip file to c:\\ and change its name to "phidgets"

  -# Make another copy of the top level directory inside the zip file to c:\\phidgets and change its name to "phidgets21"

  -# Get the Phidgets22 library zip.  It is available at: https://www.phidgets.com/downloads/phidget22/libraries/windows/Phidget22-windevel.zip as of this writing.

  -# Copy the top level directory inside the zip file to c:\\phidgets and change its name to "phidgets22"

  -# Install DirectX.  The current version of the PictoDirector uses DirectX to speed up its frame rate.  This means that the current director only works on windows.  Until we work out a better platform independent way to do this, you will only be able to build the Director project in the Picto solution correctly if you install the DirectX SDK first.  As of this writing, the latest version was available from https://www.microsoft.com/en-us/download/confirmation.aspx?id=6812 and was named DXSDK_Jun10.exe.  Download it and install it.

 -# Install Perl.  We use ActivePerl (currently at version 5.23.3.2603) which you can find at http://www.activestate.com/activeperl.  After installation, make sure that the perl.exe is in your path.

 -# Install 7-Zip (http://www.7-zip.org/) or some other application that knows how to open .gz and .tar files.

\subsection get_source_dist Get Source Distributions

\subsection get_openssl Get OpenSSL

 -# Since there is not an official dependable link for OpenSSL binary distributions, we build OpenSSL from source.  Get the latest version of the OpenSSL source from http://www.openssl.org/source/.  The most up to date version as of this writing was openssl-1.1.1b.

 -# Decompress the zipped tarballed OpenSSL source and copy the top level directory (in our case "openssl-1.1.1b") to c:\\ .

 -# Rename the "openssl-?.?.??" directory that you just copied to "openssl".

\subsection get_qwt Get Qwt

 At the time of this writing, Qwt 6.1.4 was the latest version and it offered support for Qt 5.  It is important to remember that QWT is updated after Qt such that it is possible that the latest stable version may not support the latest version of Qt.  Make sure to check for this when you are selecting Qt and Qwt versions.  Information on the latest version of Qwt is available at http://qwt.sourceforge.net/index.html.

 -# Download qwt-6.1.4 (or the latest available version)

  -# At the time of this writing, this was at https://sourceforge.net/projects/qwt/files/qwt/6.1.4/qwt-6.1.4.zip/download.

  -# Download the zip archive and extract it.

 -# In the downloaded qwt-# .#.# folder, drill down to the top level containing lots of directories (src, doc, etc) and files (qwt.pro, README, etc).  In our case this was in: (Download Location)\\qwt-6.1.4\\qwt-6.1.4.

 -# Create a c:\\qwt directory, and copy all of the contents of the folder found in the last step to that folder.  

\subsection get_picto_src Get Picto Source

 -# Create directories to form the path c:\\Projects\\Picto.

   -# Right click on c:\\Projects and use the "Git Clone..." option that was installed with TortoiseGit to get the Picto source.

   -# For "URL:" use "https://github.com/leelab/picto.git"

   -# Press OK.

   -# An "Authentication" dialog will come up.  Enter the Github Username and Password that we assumed you have at the beginning of this document and press OK.

   -# The Picto source will begin downloading.  The download will depend on your internet connection.  When the download is complete, you will have a fully set up source tree with local git repository in "c:\projects\Picto".

\subsection get_prop_browser Get Qt Property Browser

The QtPropertyBrowser library is actually included with the Qt 5.5.1 installation, but in source form only.  We use this library in our code for our Property view/update widgets.  In the Qt 5.5.1 installation this library does not include any file specifying build configuration so we created one that should be manually copied in.

 -# Copy c:\\Projects\\Picto\\3rdparty\\qtpropertybrowser.pro to the c:\\Qt64\\Qt5.5.1\\5.5\\Src\\qttools\\src\\shared\\qtpropertybrowser\\ directory.



\subsection build_src_dist Build Source Distributions

\subsection build_openssl Build OpenSSL

 -# In a Visual Studio command prompt type the following:

  -# cd c:\\openssl

  -# perl Configure VC-WIN32

  -# ms\\do_ms.bat

  -# nmake -f ms\\ntdll.mak

 -# Check that there is a out32dll folder and that it contains a bunch of OpenSSL dlls, exe's, etc.

\subsection qt_upgrade In Case of Qt Upgrade

 -# If you are using a version of Qt that is not Qt 5.5.1, you will need to change the path variable that we use to tell everything where to find the Qt binaries.

  -# Open c:\\Projects\\Picto\\tools\\qt.config\\mypaths.cmd in a text editor.

  -# Find the line that says: set QTTOP=... and change the path to point to the current version of Qt.  In the latest upgrade for example, we changed "set QTTOP=%PICTO_THIRD_PARTY%\%PICTO_QT_INSTALL_TOP_DIR%\Qt5.3.2\5.3.2" to "set QTTOP=%PICTO_THIRD_PARTY%\%PICTO_QT_INSTALL_TOP_DIR%\Qt5.5.1\5.5".

  -# Save your changes

\subsection build_qwt Build Qwt

 -# In a Visual Studio 2013 x64 Native Tools Command Prompt type the following:

  -# cd c:\\qwt

  -# c:\\Projects\\Picto\\ConfigureWindowsX64.cmd

  -# c:\\Projects\\Picto\\tools\\qt.config\\mypaths.cmd

  -# qmake

  -# nmake install

\subsection build_qt_prop_browser Build Qt Property Browser

The QtPropertyBrowser library is included with the Qt 5.5.1 installation in source form only.  We use this library in our code for our Property view/update widgets.

 -# In a Visual Studio 2013 x64 Native Tools Command Prompt type the following:

  -# cd c:\\Qt64\\Qt5.5.1\\5.5\\Src\\qttools\\src\\shared\\qtpropertybrowser

  -# c:\\Projects\\Picto\\ConfigureWindowsX64.cmd

  -# c:\\Projects\\Picto\\tools\\qt.config\\mypaths.cmd

  -# qmake

  -# nmake all

\subsection build_picto Build Picto

While the foundation for Picto was built with multiplatform deployment in mind (except for the Director due to DirectX), we have only ever built Picto for Windows x86 and x64.  To do this we first use qmake to create a visual studio solution where developement takes place. We then build Picto from that solution.

 -# In a Visual Studio 2013 x64 Native Tools Command Prompt type the following:

  -# cd c:\\Projects\\Picto

  -# ConfigureWindowsX64.cmd

  -# tools\\qt.config\\mypaths.cmd

  -# tools\\win.common\\GenerateBuildFiles.cmd

 -# You should now have a Picto.sln file in the c:\\Projects\\Picto directory.

  -# Open the c:\\Projects\\Picto\\Picto.sln solution that was just created, choose "Release" and "x64" and build it from within visual studio.



\subsection copy_3rd_party_dlls Copy Third Party DLLs to Run Directories

There are a number of third party dlls used by Picto that come from the libraries installed above.  At this point, we simply copy them into our run directories to allow the Picto applications to start correctly.  Assuming that you copied everything to the locations described above and that version names and directory structures haven't changed, you can run a getlibraries.bat file that will take care of this process for you.  If the names of the libraries have changed too significantly, it may be necessary to update the getlibraries.bat file to copy them correctly.

 -# In a Visual Studio 2013 x64 Native Tools Command Prompt type the following:

  -# cd c:\\Projects\\Picto

  -# ConfigureWindowsX64.cmd

  -# tools\\qt.config\\mypaths.cmd

  -# tools\\win.common\\GetLibraries.cmd


\subsection build_plexon_wrapper Build Plexon Wrapper

While Picto is now built as an x64 application, the Plexon library is exclusively available as x86 binaries.  To support Plexon systems, we need to use an x86 wrapper that communicates with our ProxyServer.

  -# In a Visual Studio 2013 x86 Native Tools Command Prompt type the following:

  -# cd c:\\Projects\\Picto

  -# ConfigureWindowsX86.cmd

  -# tools\\qt.config\\mypaths.cmd

  -# tools\\win.common\\GenerateWrapperBuildFiles.cmd

  -# tools\\win.common\\GetLibraries.cmd

  -# You should now have a PlexonWrapper.sln file in the c:\\Projects\\Picto directory.

  -# Open and build the c:\\Projects\\Picto\\Picto.sln solution, that was just created, choose "Release", and build it from within visual studio.


\subsection install_qt_plugin Install Qt Plugin for Visual Studio

Qt has a number of data types that are difficult to debug using a clean Visual Studio installation.  When debugging an application using Qt5, for example, QString's actual text values cannot be viewed.  To fix this and other similar issues, install the "QT Visual Studio Tools 2.3.2" (or newer version if available) which can be found as of this writing at https://download.qt.io/official_releases/vsaddin/.


\subsection documentation Documentation

Much of the Picto documentation is generated by the doxygen tool from comments in the Picto code. The tools\\win.common\\GenerateBuildFiles.cmd step in building Picto uses doxygen to create .html files in manuals\\CodeDoc\\html.
 - The starting point of this generated documentation is found in <a href="file:///c:/Projects/picto/manuals/CodeDoc/html/index.html">file:///c:/Projects/picto/manuals/CodeDoc/html/index.html</a>.  That file is generated from source\\documentation\\developersguide\\mainpage.h, and this page of that documentation is generated from source\\documentation\\developersguide\\build_environment_preparation.h.
 - This circularity should be mitigated by manually updating https://github.com/leelab/picto/wiki/BuildingDevEnvironment after each change to this source\\documentation\\developersguide\\build_environment_preparation.h file.
 - There is other documentation included in the git repository under manuals\\; it can be modified and then checked back into git.

\subsection problems Problems?


If you encounter problems, most of the tools we are using are well-supported online, so you can probably find your answers there. Otherwise, call someone who has done this before.

 */
