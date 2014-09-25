Elgato Game Capture HD
======================

DirectShow filter usage - Sample Application for Visual Studio 2013
-------------------------------------------------------------------
* Date  : 25-Sep-14
* Author: F. M. Birth

This application sample shows how to build the most simple DirectShow application
that plays audio/video of the Game Capture HD/HD60.

More information on building DirectShow applications on MSDN:
http://msdn.microsoft.com/en-us/library/windows/desktop/ff625867(v=vs.85).aspx


PREPARATION
-----------

Install Software for Elgato GameCapture HD (v.1.21.0.200 or later)

Register capture filter (usually already done with the application setup)
	regsvr32.exe C:\Program Files\Elgato\GameCapture\VideoCaptureFilter.ax


BUILD DIRECT SHOW APPLICATION
-----------------------------

Add include path for DirectShow header file <streams.h>
	e.g. C:\Program Files\Microsoft SDKs\Windows\v7.0\Samples\multimedia\directshow\baseclasses
	
Add library pathes for DirectShow library (strmbase.lib)
	e.g. C:\Program Files\Microsoft SDKs\Windows\v7.0\Samples\multimedia\directshow\baseclasses\Release
	
Implement graph building as in
	HRESULT CVideoCaptureFilterSampleDlg::InitGraph();
	
Implement showing filter properties as 
	CVideoCaptureFilterSampleDlg::ShowProperties()

Configuration
	To configure the Game Capture HD/HD60 you need to query the IElgatoVideoCaptureFilter5 
	interface from the "Elgato Game Capture HD" filter.

	The header file and documentation is available on GitHub:
	https://github.com/elgatosf/gamecapture


