/*! \mainpage Picto Developer's Guide
\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
\version 2.0.2
\date Last modified: March 6, 2015

This is the Developer's Guide for the Picto suite of experimental control applications
and associated PictoBox hardware.  If you are an investigator utilizing the software as
part of your research then you probably want the PictoManual.docx located in Picto\\manuals.  If you are modifying or attempting to build a Pictobox, you want the PictoboxDocumentation.docx from Picto\\manuals.  If you are interested in learning how the system itself works and/or modifying the system, then this guide is the right place to start.

\section devdocs_getting_started_introduction Introduction
The Picto suite of experimental control applications is utilized by researchers to perform neurobiological experiments in the study of decision making.  The system consists of serveral components:
	- The Director component runs the experimental "game" that the test subject "plays."  This component takes in user input from a mouse input, eye tracker, joystick, or any other device that can provide analog x/y data.  Inputs are monitored to control the flow of the "game" and all user input, reward, and experimental state data are sent over the network to the Picto Server Component.  A hardware line connects the Director to the Proxy component and an alignment code is sent over this line periodically for the purpose of time alignment.
	- The Proxy component (ProxyMainWindow) runs on a neural data acquisition computer (currently supported models are Plexon and TDT).  It collects data from the acquisition system, including neural spikes, local field potential data and alignment codes coming in from the Director, repackages the data and sends it over the network to the Picto Server component.
	- The Picto Server component is the main hub of the application suite.  This component takes in data from the Proxy and Director, aligns the timestreams from the two components based on the alignment codes and saves the data to disk in a Session file.  It also makes Director/Proxy data available in real time for monitoring from the Workstation component.  Lastly, the Picto Server acts as the glue that binds the Workstation to the Proxy and Director.  Whenever the Workstation sends a command to be handled on either the Proxy or the Director, the command is sent to the Server.  The Server takes care of forwarding the command to the appropriate component and informs the Workstation when/if the component complies with the command directive.
	- The Workstation component is the Researcher's window into the Picto system.  It includes a RemoteViewer which Researchers use to start/stop Experimental sessions, control them and monitor their activity.  It includes a StateEditViewer, used to design the Experiment that runs on the Director and the Analysis that is used to extract data from session files.  It includes a TestViewer for locally testing and debugging Experimental designs and Analyses.  Lastly, the Workstation includes a ReplayViewer which is used to playback Session files, record Session activity to a video file, and run Session Analyses.

While Neurobiological research is the main focus on the application suite, we have made every effort to design it in such a way that it will be generalizable and extendable for other types of applications as well.  In this vain, and for the purpose of training users, the suite can be used without an attached Proxy component.  In its essence, Picto is a system for designing and running experiments consisting of precisely timed, carefully controlled stimuli and user feedback with an optional capability for tracking neurobiological data and aligning it with the behavioral stream.

\section devdocs_getting_started_prerequisites Prerequisites
This document assumes that the reader has experience with C++ based software development and is familiar with the Qt framework.  Many many details and tutorials about getting started with C++ and Qt are available online.  We also assume that the reader has a basic level of comfort with the command-line interface, as it will be used to prepare a build environment.

\section devdocs_getting_started Getting Started
In order to get up to speed with Picto development, you will need to create a development environment, become familiar with the various files used in the Picto system and familiarize yourself with the code base.  The following pages should help guide you through these tasks.
	- \subpage build_environment_preparation - Use this page to set up your development environment to build and edit Picto code.
	- \subpage user_files - Use this page to gain a broad understanding of the files used generated and used by Picto.
	- \subpage first_code_look - Use this as a guide to becoming familiar with the Picto code base.

\section to_do To Do...
	- \subpage known_bugs - This page contains the list of current known bugs.
	- \subpage future_directions_software - This page contains some ideas for features that we would like to add to the Picto software.

 */
