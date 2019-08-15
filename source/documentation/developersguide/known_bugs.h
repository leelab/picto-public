/*! \page known_bugs Known Bugs
The following is a partial list of known bugs in the current version of Picto.  Some of these bugs are fairly minor, while others are larger in scope and will require a significant effort to correct.  Where possible we includes ideas for the source of the problem and for methods of correcting it.

<b>Analyzing large files can causes Picto to infrequently crash or simply fail, paricularly in batch analysis</b><br>
We have already made significant strides in minimizing this problem by optimizing storage use for LFP and other typicaly large arrays.  We also worked to make sure that all data associated with each Analysis is completely deleted before starting a new analysis and we added some code (for Windows) to defragment RAM before each new analysis (see PlaybackController::update()).  At this point, we have not come across any Sessions that simply could not be analyzed.  In the few cases where Analysis has failed, restarting the Workstation was enough to make the Analysis run correctly.  Even so, future effors that can be taken to erradicate the problem are:
		- Creating a file backed array type and using it for analysis data.  The array could put all of its data in a file with a simple byte array.  Small sections of the file would be loaded in RAM at a given time based on the current Task state and which analysis functions are called.  Loading of data from this simple file would be significantly faster because it would simply be saved in serial order with no search indexing and we would avoid a lot of overhead that comes with SQL query execution.
		- Picto and the computers that run it have been upgraded to 64 bit to make use of larger quantities of RAM.  This is a less elegant approach, but it should take care of the problem at least temporarily.

<b>When running the Director with Pixmap rendering on Windows 8, the application stops responding after a few seconds</b><br>
In one case, we solved this type of issue by extending the Windows HungAppTimeout registry entry.  This is a good patch, but this is part of a bigger problem which is that the Picto Experiment is not run as part of the Qt Event loop.  This is mentioned in the \ref future_directions_software page, and when it is fixed, this problem should go away.

<b>AnalysisFileOutputWidget loads data without line breaks very slowly</b><br>
Currently these widgets automatically load a set number of lines per page.  If there are no line breaks or infrequent ones, the widget tries to load too much data.

<b>When using the arrow keys to adjust Property values in the TestViewer or RemoteViewer, sometimes the value increments more than once per arrow press</b><br>
We haven't yet looked into this issue.

<b>Frames are skipped when starting a Run and when entering a Paused state</b><br>
This has not been a major issue since in both of these cases there is no need for accurate analysis.  It would be a good idea to find the root of this issue though and fix it.

<b>Setting the alpha value of tokens in a TokenFactoryGraphic doesn't work</b><br>
I think that the issue here may be that in Picto::TokenFactoryGraphic::setTokenColor() the colors might be serialized without any alpha values.  If this is the case, then you will have to think hard about reverse compatibility when fixing this.

<b>Pressing save in the AnalysisOutputWidgetContainer does nothing until data is visible there</b><br>
The issue here is that even though Analysis data may already have been written to the AnalysisFileOutput object, pressing save does nothing since the file buffer has not been flushed to disk yet and save just copies the still empty temporary file to the path selected by the user.  For quick checks, it is useful to be able to just run a few trials and save the data out though.  Ideally, the fix for this would make pressing the save button cause all of the AnalysisOutput objects in the AnalysisOutputWidgetContainer flush their file buffers to disk.

*/
