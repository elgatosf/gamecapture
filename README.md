Elgato Game Capture HD
======================

Elgato Game Capture HD open source components.

https://www.elgato.com/en/gaming

VideoCaptureFilter
-------------------

###### IVideoCaptureFilter.h  
This header file is used to find and use the video capture filter that the Elgato Game Capture HD software installs. 


VideoCaptureFilterSample
------------------------
Sample application for Visual Studio 2013

This application sample shows how to build the most simple DirectShow application that plays audio/video of the Game Capture HD/HD60.

For more details see ReadMe.txt


Project History
----------------
14-Sep-15: 
	- Support for Elgato Game Capture HD60 Pro
	- Types are now wrapped in namespace ElgatoGameCapture
	- Moved types to separate header file
	- Added new interface IElgatoVideoCaptureFilterEnumeration for Elgato device enumeration
	
15-May-15	
	- Added MPEG-TS output pin to Video Capture Filter
 
28-Aug-14: 
	- Initial version.
