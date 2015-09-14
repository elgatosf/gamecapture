//=============================================================================
//! @file        IVideoCaptureFilter.h
//! @bc -----------------------------------------------------------------------
//! @ec @brief   Interface declaration for Elgato Video Capture Filter
//! @author      F.M.Birth, T.Schnitzler
//! @date        01-Oct-12 FMB - Creation
//! @date        08-Apr-13 TS  - Added <i>IVideoCaptureFilter2</i>
//! @date        14-Nov-13 TS  - Added <i>IVideoCaptureFilter3</i>
//! @date        21-Jul-14 TS  - Added <i>IVideoCaptureFilter4</i>
//! @date        28-Aug-14 FDj - MIT license added
//! @date        04-Sep-14 FMB - Added <i>interfaceVersion</i> to VIDEO_CAPTURE_FILTER_SETTINGS_EX
//! @date        29-Jan-15 FMB - Added MPEG-TS Pin to filter, added interface IElgatoVideoCaptureFilter6
//! @date        15-May-15 FMB - Added <i>IElgatoVideoCaptureFilterEnumeration</i> to enumerate devices 
//!                              and query device caps before the filter is added to the filter graph
//!
//! @note        The DirectShow filter works with
//!              - Elgato Game Capture HD 
//!              - Elgato Game Capture HD60
//! @bc -----------------------------------------------------------------------
//! @ec @par     Copyright
//! @n           (c) 2012-15, Elgato Systems. All Rights Reserved.
//! @n
//! @n The MIT License (MIT)
//! @n
//! @n Permission is hereby granted, free of charge, to any person obtaining a copy
//! @n of this software and associated documentation files (the "Software"), to deal
//! @n in the Software without restriction, including without limitation the rights
//! @n to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//! @n copies of the Software, and to permit persons to whom the Software is
//! @n furnished to do so, subject to the following conditions:
//! @n
//! @n The above copyright notice and this permission notice shall be included in all
//! @n copies or substantial portions of the Software.
//! @n
//! @n THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//! @n IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//! @n FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//! @n AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//! @n LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//! @n OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//! @n SOFTWARE.
//! @n
//=============================================================================

#pragma once

#include "IVideoCaptureFilterTypes.h"
//using namespace	ElgatoGameCapture;

/*=============================================================================
// FILTER INTERFACE
=============================================================================*/
//! Name of the filter as it appears in GraphEdt
#define VIDEO_CAPTURE_FILTER_NAME				"Elgato Game Capture HD"

//! Name of the filter as it appears in GraphEdt
#define VIDEO_CAPTURE_FILTER_NAME_L				L"Elgato Game Capture HD"


#define ELGATO_VCF_VIDEO_PID	100	//!< video PID in MPEG-TS stream
#define ELGATO_VCF_AUDIO_PID	101	//!< audio PID in MPEG-TS stream


//! Interface version:
//! - 1st digit: interface version (e.g. 5 for IElgatoVideoCaptureFilter5)
//! - 2nd digit: revision (changed e.g. when reserved fields in structures changed their meaning)
#define VIDEO_CAPTURE_FILTER_INTERFACE_VERSION	61


// {39F50F4C-99E1-464a-B6F9-D605B4FB5918}
DEFINE_GUID(CLSID_ElgatoVideoCaptureFilter,
0x39f50f4c, 0x99e1, 0x464a, 0xb6, 0xf9, 0xd6, 0x05, 0xb4, 0xfb, 0x59, 0x18);

// {39F50F4C-99E1-464a-B6F9-D605B4FB5919}
DEFINE_GUID(CLSID_ElgatoVideoCaptureFilterProperties,
0x39f50f4c, 0x99e1, 0x464a, 0xb6, 0xf9, 0xd6, 0x05, 0xb4, 0xfb, 0x59, 0x19);

// {13DD0CCF-A773-4CB7-8C98-8E31E69F0252}
DEFINE_GUID(IID_IElgatoVideoCaptureFilterEnumeration, 
0x13dd0ccf, 0xa773, 0x4cb7, 0x8c, 0x98, 0x8e, 0x31, 0xe6, 0x9f, 0x2, 0x52);


// {39F50F4C-99E1-464a-B6F9-D605B4FB5920}
DEFINE_GUID(IID_IElgatoVideoCaptureFilter,
0x39f50f4c, 0x99e1, 0x464a, 0xb6, 0xf9, 0xd6, 0x05, 0xb4, 0xfb, 0x59, 0x20);

// {585B2914-252E-49bd-B730-7B4C40F4D4E5}
DEFINE_GUID(IID_IElgatoVideoCaptureFilter2,
0x585b2914, 0x252e, 0x49bd, 0xb7, 0x30, 0x7b, 0x4c, 0x40, 0xf4, 0xd4, 0xe5);

// {CC415EB7-B1C7-428c-9E5E-D9747DB4BE76}
DEFINE_GUID(IID_IElgatoVideoCaptureFilter3,
0xcc415eb7, 0xb1c7, 0x428c, 0x9e, 0x5e, 0xd9, 0x74, 0x7d, 0xb4, 0xbe, 0x76);

// {197992FF-ED65-47CB-8032-D287AB40B33F}
DEFINE_GUID(IID_IElgatoVideoCaptureFilter4,
0x197992ff, 0xed65, 0x47cb, 0x80, 0x32, 0xd2, 0x87, 0xab, 0x40, 0xb3, 0x3f);

// {7E6E9E9E-4062-4364-99B1-15C2F662B502}
DEFINE_GUID(IID_IElgatoVideoCaptureFilter5,
0x7e6e9e9e, 0x4062, 0x4364, 0x99, 0xb1, 0x15, 0xc2, 0xf6, 0x62, 0xb5, 0x02);

// {39F50F4C-99E1-464a-B6F9-D605B4FB5925}
DEFINE_GUID(IID_IElgatoVideoCaptureFilter6,
0x39f50f4c, 0x99e1, 0x464a, 0xb6, 0xf9, 0xd6, 0x05, 0xb4, 0xfb, 0x59, 0x25);



/*=============================================================================
// IElgatoVideoCaptureFilterEnumeration
=============================================================================*/

//! Interface IElgatoVideoCaptureFilterEnumeration.
//! Methods of this interface can be called before the Elgato Video Capture Filter was added to a filter graph.
DECLARE_INTERFACE_(IElgatoVideoCaptureFilterEnumeration, IUnknown)
{
	//! Enumerate available devices
	STDMETHOD(GetNumAvailableDevices)(THIS_ __out int* pNumDevices) PURE;

	//! Query device capabilities
	//! @return S_OK, VFW_E_NO_CAPTURE_HARDWARE etc
	STDMETHOD(GetDeviceCaps)(THIS_ __in int deviceIdx, __out ElgatoGameCapture::VIDEO_CAPTURE_FILTER_DEVICE_CAPS* pDeviceCaps) PURE;
};


/*=============================================================================
// IElgatoVideoCaptureFilter
=============================================================================*/

//! Interface IElgatoVideoCaptureFilter
DECLARE_INTERFACE_(IElgatoVideoCaptureFilter, IUnknown)
{
};

/*=============================================================================
// IElgatoVideoCaptureFilter2
=============================================================================*/

//! Interface IElgatoVideoCaptureFilter2
DECLARE_INTERFACE_(IElgatoVideoCaptureFilter2, IElgatoVideoCaptureFilter)
{
	//! Get current settings
	//! @note Deprecated! Use IElgatoVideoCaptureFilter5::GetSettingsEx() instead
	STDMETHOD(GetSettings)(THIS_ ElgatoGameCapture::PVIDEO_CAPTURE_FILTER_SETTINGS pSettings) PURE;

	//! Set settings
	//! @note Deprecated! Use IElgatoVideoCaptureFilter5::SetSettingsEx() instead
	STDMETHOD(SetSettings)(THIS_ ElgatoGameCapture::PCVIDEO_CAPTURE_FILTER_SETTINGS pcSettings) PURE;
};

/*=============================================================================
// IElgatoVideoCaptureFilter3
=============================================================================*/

//! Interface IElgatoVideoCaptureFilter3
DECLARE_INTERFACE_(IElgatoVideoCaptureFilter3, IElgatoVideoCaptureFilter2)
{
	//! Get A/V delay in milli-seconds
	//! (approximate delay between input signal and DirectShow filter output)
	//! @note Deprecated! Use IElgatoVideoCaptureFilter6::GetDelayMs()
	STDMETHOD(GetDelayMs_Deprecated)(THIS_ int* pnDelayMs) PURE;
};

/*=============================================================================
// IElgatoVideoCaptureFilter4
=============================================================================*/

//! Custom event that can be received by IMediaEvent::GetEvent().
//! @n If SetNotificationCallback() was not set this method is used to send notifications.
//! @param lEventCode = VIDEO_CAPTURE_FILTER_EVENT
//! @param lParam1    = VIDEO_CAPTURE_FILTER_NOTIFICATION
//! @param lParam2    = reserved for future use (e.g. notifications with additional data)
#define VIDEO_CAPTURE_FILTER_EVENT		EC_USER + 0x0FD9

//! Message callback
typedef void (CALLBACK* PFN_VIDEO_CAPTURE_FILTER_NOTIFICATION_CALLBACK)(ElgatoGameCapture::VIDEO_CAPTURE_FILTER_NOTIFICATION nMessage, void* pData, int nSize, void* pContext);

//! Interface IElgatoVideoCaptureFilter4
DECLARE_INTERFACE_(IElgatoVideoCaptureFilter4, IElgatoVideoCaptureFilter3)
{
	//! Check device is present; only available after the filter was added to a filter graph
	STDMETHOD(GetDevicePresent)(THIS_ BOOL* pfDevicePresent) PURE;

	//! Get current device type; only available after the filter was added to a filter graph
	STDMETHOD(GetDeviceType)(THIS_ ElgatoGameCapture::VIDEO_CAPTURE_FILTER_DEVICE_TYPE* pnDeviceType) PURE;

	//! Set callback to receive notifications from the filter in your app.
	STDMETHOD(SetNotificationCallback)(THIS_ PFN_VIDEO_CAPTURE_FILTER_NOTIFICATION_CALLBACK pCallback, void* pContext) PURE;
};

/*=============================================================================
// IElgatoVideoCaptureFilter5
=============================================================================*/

//! Interface IElgatoVideoCaptureFilter5
DECLARE_INTERFACE_(IElgatoVideoCaptureFilter5, IElgatoVideoCaptureFilter4)
{
	//! Get current settings; only available after the filter was added to a filter graph
	STDMETHOD(GetSettingsEx)(THIS_ ElgatoGameCapture::PVIDEO_CAPTURE_FILTER_SETTINGS_EX pSettings) PURE;

	//! Set settings
	STDMETHOD(SetSettingsEx)(THIS_ ElgatoGameCapture::PCVIDEO_CAPTURE_FILTER_SETTINGS_EX pcSettings) PURE;
};


/*=============================================================================
// IElgatoVideoCaptureFilter6
=============================================================================*/

//! Interface IElgatoVideoCaptureFilter6
DECLARE_INTERFACE_(IElgatoVideoCaptureFilter6, IElgatoVideoCaptureFilter5)
{
	//! Get A/V delay in milliseconds; only available after the filter was added to a filter graph
	//! @param pnDelayMs latency of stream in milliseconds (delay between input signal and DirectShow filter output)
	//! @param forCompressedData true -> get delay for MPEG-TS pin, false -> get delay for audio/video pins
	STDMETHOD(GetDelayMs)(THIS_ __out int* pnDelayMs, __in bool forCompressedData) PURE;
};


