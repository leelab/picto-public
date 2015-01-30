/*! \page build_environment_preparation Create a Picto Development Environment

\section assumptions Assumptions

 - You are running Windows
 - You have a valid Github account that has been added to the Picto project on Github.  If you don't have one already, set up an account through Github.  After that, Leelab has a Leelab.Admin account with administration control that can be used to give your account access to the repository.

\section installing_prerequisites Installing Prerequisite Applications

\subsection get_install_binary Get and Install Binary Distributions

Just about everything you need to create an old version of the development environment (apart from the actual Picto source) is available in \\cog\\it\\Software\\Picto\\Developmentenvironment if you're on the Leelab network.  To build the latest and greatest version based on newer versions of the various libraries underlying Picto, you will need to use the links throughout this document to download locations for each required software package.

 -# Install Visual Studio 2013.

 -# Install the latest version of Tortoise Git.  This is currently available at https://code.google.com/p/tortoisegit/.

 -# Install Qt 5.3.2 for Windows 32-bit (VS 2013 561 MB). This is currently available at http://qt-project.org/downloads.  When asked, specify that Qt 5.3.2 be installed at c:\\Qt\\Qt5.3.2. When you reach the "Select Components" window, use the "Select All" button to make sure that all components are selected (this is not the default setting).

 -# Install the Ni-Daq 9.1 API.  The Picto director uses a National Instruments card and the Director needs to link to this card's libraries.  The Ni-Daq 9.1 installer is currently available at: http://joule.ni.com/nidu/cds/view/p/id/1614/lang/en.  Even as of this writing, this is not the newest version.  You are welcome to (encouraged) to install the newest version of the API; however, be warned that this may require some upgrade of the Picto code if their have been changes in the Ni-Daq API.  This installation takes a while.  Something like an hour.

 -# Install the Phidgets libraries.  These are the libraries used by the Director to control the Pictobox's LCD display and input dial.

  -# Get the Phidgets library zip.  It is available at: http://www.phidgets.com/docs/OS_-_Windows#Quick_Downloads under the name "Phidget21 Libraries" as of this writing.  

  -# Copy the top level directory inside the zip file to c:\\ and change its name to "phidgets"

 -# Install DirectX.  The current version of the PictoDirector uses DirectX to speed up its frame rate.  This means that the current director only works on windows.  Until we work out a better platform independent way to do this, you will only be able to build the Director project in the Picto solution correctly if you install the DirectX SDK first.  As of this writing, the latest version was available from http://www.microsoft.com/download/en/details.aspx?displaylang=en&id=6812.  Download it and install it.

 -# Install Perl.  We use ActivePerl (currently at version 5.16.2.1602) which you can find at http://www.activestate.com/activeperl.  The ActivePerl website is also linked to from the Perl website http://www.perl.org/get.html.  After installation, make sure that the perl.exe is in your path.

 -# Install 7-Zip (http://www.7-zip.org/) or some other application that knows how to open .gz and .tar files.

\subsection get_source_dist Get Source Distributions

\subsection get_openssl Get OpenSSL

 -# Since there is not an official dependable link for OpenSSL binary distributions, we build OpenSSL from source.  Get the latest version of the OpenSSL source from http://www.openssl.org/source/.  The most up to date version as of this writing was openssl-1.0.1c.

 -# Decompress the zipped tarballed OpenSSL source and copy the top level directory (in our case "openssl-1.0.1c") to c:\\ .

 -# Rename the "openssl-Version" directory that you just copied to "openssl".

\subsection get_qwt Get QWT

 At the time of this writing, QWT 6.1.2 was the latest version and it offered support for QT 5.  It is important to remember that QWT is updated after QT such that it is possible that the latest stable version may not support the latest version of QT.  Make sure to check for this when you are selecting QT and QWT versions.  Information on the latest version of QWT is available at http://qwt.sourceforge.net/index.html.

 -# Download qwt-6.1.2 (or the latest available version)

  -# Go to http://sourceforge.net/projects/qwt/files/qwt/ and drill down in the directory tree of the latest version to the level where a zipped archive of the code is available.  At the time of this writing, this was qwt-6.1.2.zip available at http://sourceforge.net/projects/qwt/files/qwt/6.1.2/.

  -# Download the zip archive and extract it.

 -# In the downloaded qwt-# .#.# folder, drill down to the top level containing lots of directories (src, doc, etc) and files (qwt.pro, README, etc).  In our case this was in: (Download Location)\\qwt-6.1.2\\qwt-6.1.2.

 -# Create a c:\\qwt directory, and copy all of the contents of the folder found in the last step to that folder.  

\subsection get_picto_src Get Picto Source

 -# Create directories to form the path c:\\Projects\\Picto.

 -# You have two options for getting the Picto source.  Use option A in a normal scenario and option B in a scenario where you have no direct access to an existing Picto source tree.

  -# Option A
   -# Copy everything including the hiddin .git reporitory folder from the c:\\Projects\\Picto directory of an existing Picto development environment.

   -# When the directory is finished being copied, right click on the c:\\Projects\\Picto directory and select the "Git Sync..." option that was installed with TortoiseGit.
   -# In the dialog that opens, if the "Remote URL:" field is blank (ie. doesn't say 'origin'), use the URL of the Picto repository "https://github.com/leelab/picto.git".

   -# Press the "Pull" button.

   -# An "Authentication" dialog will come up.  Enter the Github Username and Password that we assumed you have at the beginning of this document and press OK.

   -# Once the Git Pull is done, you'll have access to all of the latest and greatest Picto code in your local copy of the Git repository, but your actual source tree will not have changed. You'll probably want to get rid of any local source changes that were copied from the machine where you got your Picto source.  To do this, right click the Picto directory and select "TortoiseGit">"Revert...", then check "Select / deselect all", then "OK".

  -# Option B
   -# Right click on c:\\Projects and use the "Git Clone..." option that was installed with TortoiseGit to get the Picto source.

   -# For "URL:" use "https://github.com/leelab/picto.git"

   -# Press OK.

   -# An "Authentication" dialog will come up.  Enter the Github Username and Password that we assumed you have at the beginning of this document and press OK.

   -# The Picto source will begin downloading.  The download will depend on your internet connection.  In our case with a fairly good connection, this took around 10 minutes.  When the download is complete, you will have a fully set up source tree with local git repository in "c:\projects\Picto".

\subsection get_prop_browser Get Qt Property Browser

The QtPropertyBrowser library is actually included with the QT 5.3.2 installation, but in source form only.  We use this library in our code for our Property view/update widgets.  In the Qt 5.3.2 installation this library does not include any file specifying build configuration so we created one that should be manually copied in.

 -# Copy c:\\Projects\\Picto\\3rdparty\\qtpropertybrowser.pro to the c:\\Qt\\Qt5.3.2\\5.3.2\\Src\\qttools\\src\\shared\\qtpropertybrowser\\ directory.



\subsection build_src_dist Build Source Distributions

\subsection build_openssl Build OpenSSL

 -# In a Visual Studio command prompt type the following:

  -# cd c:\\openssl

  -# perl Configure VC-WIN32

  -# ms\\do_ms.bat

  -# nmake -f ms\\ntdll.mak

 -# Check that there is a out32dll folder and that it contains a bunch of OpenSSL dlls, exe's, etc.

\subsection qt_upgrade In Case of QT Upgrade

 -# If you are using a version of QT that is not Qt 5.3.2, you will need to change the path variable that we use to tell everything where to find the Qt binaries.

  -# Open c:\\Projects\\Picto\\tools\\qt.config\\mypaths.cmd in a text editor.

  -# Find the line that says: set QTTOP=... and change the path to point to the current version of Qt.  In the latest upgrade for example, we changed "set QTTOP=%PICTO_THIRD_PARTY%\Qt\Qt5.1.1\5.1.1" to "set QTTOP=%PICTO_THIRD_PARTY%\Qt\Qt5.3.2\5.3.2".

  -# Save your changes

\subsection build_qwt Build QWT

 -# In a Visual Studio 2013 command prompt type the following:

  -# cd c:\\qwt

  -# c:\\Projects\\Picto\\ConfigureWindowsX86.cmd

  -# c:\\Projects\\Picto\\tools\\qt.config\\mypaths.cmd

  -# qmake

  -# nmake install

\subsection build_qt_prop_browser Build Qt Property Browser

The QtPropertyBrowser library is included with the QT 5.3.2 installation in source form only.  We use this library in our code for our Property view/update widgets.

 -# In a Visual Studio 2013 command prompt type the following:

  -# cd c:\\Qt\\Qt5.3.2\\5.3.2\\Src\\qttools\\src\\shared\\qtpropertybrowser

  -# c:\\Projects\\Picto\\ConfigureWindowsX86.cmd

  -# c:\\Projects\\Picto\\tools\\qt.config\\mypaths.cmd

  -# qmake

  -# nmake all

\subsection build_picto Build Picto

While Picto is theoretically multiplatform (except for the Director due to DirectX), we have only ever built Picto for Windows x86.  To do this we first use qmake to create a visual studio solution where developement takes place. We can then build Picto either from that solution or from the command line.

 -# In a Visual Studio 2013 command prompt type the following:

  -# cd c:\\Projects\\Picto

  -# ConfigureWindowsX86.cmd

  -# tools\\qt.config\\mypaths.cmd

  -# tools\\win.common\\generatebuildfiles.cmd

 -# You should now have a Picto.sln file in the \\Picto\\trunk directory.

  -# To build from the command line, run "nmake all". Otherwise, open and build the c:\\Projects\\Picto\\Picto.sln solution that was just created and build it from within visual studio.



\subsection copy_3rd_party_dlls Copy Third Party DLLs to Run Directories

There are a number of third party dlls used by Picto that come from the libraries installed above.  At this point, we simply copy them into our run directories to allow the Picto applications to start correctly.  Assuming that you copied everything to the locations described above and that version names and directory structures haven't changed, you can run a getlibraries.bat file that will take care of this process for you.  If the names of the libraries have changed too significantly, it may be necessary to update the getlibraries.bat file to copy them correctly.

 -# In a Visual Studio 2013 command prompt type the following:

  -# cd c:\\Projects\\Picto

  -# ConfigureWindowsX86.cmd

  -# tools\\qt.config\\mypaths.cmd

  -# tools\\win.common\\getlibraries.cmd



\subsection install_qt_plugin Install Qt Plugin for Visual Studio

Qt has a number of data types that are difficult to debug using a clean Visual Studio installation.  When debugging an application using Qt5, for example, QString's actual text values cannot be viewed.  To fix this and other similar issues, install the "Visual Studio Add-in 1.2.2 for Qt5" (or newer version if available) which can be found as of this writing at http://qt-project.org/downloads#qt-other.



\subsection problems Problems?



If you encounter problems, most of the tools we are using are well-supported online, so you can probably find your answers there. Otherwise, call someone who has done this before.

 */
