//==============================================================================
// Elgato Game Capture HD
// DirectShow filter usage - Sample Application for Visual Studio 2013
// -----------------------------------------------------------------------------
// Author: F. M. Birth
// Date  : 25-Sep-14
// Date  : 29-Jan-15 - Added "Using the MPEG-TS pin"
//==============================================================================

This application sample shows how to build the most simple DirectShow application
that plays audio/video of the Game Capture HD/HD60.

More information on building DirectShow applications on MSDN:
http://msdn.microsoft.com/en-us/library/windows/desktop/ff625867(v=vs.85).aspx


//------------------------------------------------------------------------------
// PREPARATION
//------------------------------------------------------------------------------

Install Software for Elgato GameCapture HD (v.1.21.0.200 or later)

Register capture filter (usually already done with the application setup)
	regsvr32.exe C:\Program Files\Elgato\GameCapture\VideoCaptureFilter.ax


//------------------------------------------------------------------------------
// BUILD DIRECT SHOW APPLICATION
//------------------------------------------------------------------------------

Add include path for DirectShow header file <streams.h>
	e.g. C:\Program Files\Microsoft SDKs\Windows\v7.0\Samples\multimedia\directshow\baseclasses
	
Add library pathes for DirectShow library (strmbase.lib)
	e.g. C:\Program Files\Microsoft SDKs\Windows\v7.0\Samples\multimedia\directshow\baseclasses\Release
	
Implement graph building as in
	HRESULT CVideoCaptureFilterSampleDlg::InitGraph();
	
Implement showing filter properties as in
	CVideoCaptureFilterSampleDlg::ShowProperties()

Configuration
	To configure the Game Capture HD/HD60 you need to query the IElgatoVideoCaptureFilter6
	interface from the "Elgato Game Capture HD" filter.

	The header file and documentation are available on GitHub:
	https://github.com/elgatosf/gamecapture


//------------------------------------------------------------------------------
// Using the MPEG-TS pin
//------------------------------------------------------------------------------
Since GameCapture software version 2.20 the Video Capture Filter provides an additional MPEG-TS pin.

The sample application now contains code to render from the this pin. 
You only need to set renderFromMpegTsPin to true.

The sample code uses Microsoft Demultiplexer and Video/Audio Decoder filters 
unless other filters are selected during automatic rendering.
Please note that the "Microsoft DTV-DVD Video Decoder" does not play back correctly all video formats.
For your application we recommend to use other demultiplexer and/or video decoder filters (e.g. FFmpeg).

For running this sample we recommend to operate it with a Game Capture HD60 and a 720p30 input signal.

