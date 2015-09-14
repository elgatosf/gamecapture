
// VideoCaptureFilterSampleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VideoCaptureFilterSample.h"
#include "VideoCaptureFilterSampleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVideoCaptureFilterSampleDlg dialog




CVideoCaptureFilterSampleDlg::CVideoCaptureFilterSampleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVideoCaptureFilterSampleDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVideoCaptureFilterSampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CVideoCaptureFilterSampleDlg, CDialog)
	//{{AFX_MSG_MAP(CVideoCaptureFilterSampleDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_PROPERTIES, &CVideoCaptureFilterSampleDlg::OnBnClickedProperties)
END_MESSAGE_MAP()


// CVideoCaptureFilterSampleDlg message handlers

BOOL CVideoCaptureFilterSampleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	QueryCaps(); // Query static capabilities from filter (no filter graph needed)
	
	InitGraph(); // Build a preview graph

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVideoCaptureFilterSampleDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVideoCaptureFilterSampleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CVideoCaptureFilterSampleDlg::OnBnClickedProperties()
{
	ShowProperties();
}


void CVideoCaptureFilterSampleDlg::OnOK()
{
	HRESULT hr = DestroyGraph();
	_ASSERT(SUCCEEDED(hr));

	CDialog::OnOK();
}

void CVideoCaptureFilterSampleDlg::OnCancel()
{
	HRESULT hr = DestroyGraph();
	_ASSERT(SUCCEEDED(hr));

	CDialog::OnCancel();
}


/*=============================================================================
// BUILD GRAPH
=============================================================================*/


#include <initguid.h>
#include <streams.h>	// for DirectShow headers
#include <wmcodecdsp.h>	// for MEDIASUBTYPE_MPEG_ADTS_AAC
#include <mmreg.h>		// for WAVE_FORMAT_MPEG_ADTS_AAC
#include <dvdmedia.h>	// for VIDEOINFOHEADER2
#include <bdaiface.h>	// for IMPEG2PIDMap

#pragma comment(lib, "strmbaseu.lib")		// for <streams.h>
#pragma comment(lib, "wmcodecdspuuid.lib")	// for UUIDs from <wmcodecdsp.h>

#if 1
	#include "../VideoCaptureFilter/IVideoCaptureFilterTypes.h"
	#include "../VideoCaptureFilter/IVideoCaptureFilter.h"
	using namespace ElgatoGameCapture;
#else
	#define ELGATO_VCF_VIDEO_PID	100	//!< video PID in MPEG-TS stream
	#define ELGATO_VCF_AUDIO_PID	101	//!< audio PID in MPEG-TS stream

	// Filter guid of Elgato Video Capture Filter
	// {39F50F4C-99E1-464a-B6F9-D605B4FB5918}
	DEFINE_GUID(CLSID_ElgatoVideoCaptureFilter, 
	0x39f50f4c, 0x99e1, 0x464a, 0xb6, 0xf9, 0xd6, 0x5, 0xb4, 0xfb, 0x59, 0x18);
#endif


#ifndef SAFE_RELEASE
#define SAFE_RELEASE(_pI_) { if(_pI_) { (_pI_)->Release(); (_pI_)=NULL;} }
#endif


// Globals
CComPtr<ICaptureGraphBuilder2>	g_pCapBuilder;
CComPtr<IFilterGraph2>			g_pFilterGraph;
CComPtr<IMediaControl>			g_pMediaControl;
CComPtr<IBaseFilter>			g_pElgatoCapFilter;

//! see https://msdn.microsoft.com/en-us/library/windows/desktop/dd757808(v=vs.85).aspx
AM_MEDIA_TYPE* GetVideoMediaType()
{
	static VIDEOINFOHEADER2 vih;
	ZeroMemory(&vih, sizeof(vih));

	vih.bmiHeader.biSize 			= sizeof(vih.bmiHeader);
	vih.bmiHeader.biWidth 			= 1920;
	vih.bmiHeader.biHeight 			= 1080;
	vih.bmiHeader.biPlanes 			= 1;
	vih.bmiHeader.biBitCount 		= 16;
	vih.bmiHeader.biCompression		= MAKEFOURCC('H', '2', '6', '4');

	vih.dwPictAspectRatioX			= 16;
	vih.dwPictAspectRatioY			= 9;

	static AM_MEDIA_TYPE  mt;
	ZeroMemory(&mt, sizeof(mt));

	mt.majortype 					= MEDIATYPE_Video;
	mt.subtype 						= MEDIASUBTYPE_H264;

	mt.bFixedSizeSamples 			= FALSE;
	mt.bTemporalCompression 		= TRUE;
	mt.lSampleSize 					= 1;

	mt.formattype 					= FORMAT_VideoInfo2;
	mt.cbFormat 					= sizeof(vih);
	mt.pbFormat 					= (BYTE*)&vih;

	return &mt;
}

//! @return audio media type that is accepted by "Microsoft DTV-DVD Audio Decoder"
AM_MEDIA_TYPE* GetAudioMediaType()
{
	static WAVEFORMATEX wfx;
	ZeroMemory(&wfx, sizeof(wfx));

	wfx.wFormatTag 					= WAVE_FORMAT_MPEG_ADTS_AAC;
	wfx.nChannels 					= 2;
	wfx.nSamplesPerSec 				= 48000;
	wfx.nAvgBytesPerSec 			= 14000;
	wfx.nBlockAlign 				= 24;
	wfx.wBitsPerSample 				= 16;
	wfx.cbSize 						= 0;

	static AM_MEDIA_TYPE  mt;
	ZeroMemory(&mt, sizeof(mt));
	mt.majortype 					= MEDIATYPE_Audio;
	mt.subtype 						= MEDIASUBTYPE_MPEG_ADTS_AAC;

	mt.bFixedSizeSamples 			= TRUE;
	mt.bTemporalCompression			= FALSE;
	mt.lSampleSize 					= 1;

	mt.formattype 					= FORMAT_WaveFormatEx;
	mt.cbFormat 					= sizeof(wfx);
	mt.pbFormat 					= (BYTE*)&wfx;
	
	return &mt;
}

void DirectShow_DumpGraph(IFilterGraph2* pIFilterGraph)
{
	_ASSERTE(NULL != pIFilterGraph);
	if (NULL == pIFilterGraph)
		return;

	CComPtr<IEnumFilters> pEnumFilters;
	HRESULT hrTmp = g_pFilterGraph->EnumFilters(&pEnumFilters);
	if (SUCCEEDED(hrTmp))
	{
		IBaseFilter* pFilter = NULL;
		while (S_OK == pEnumFilters->Next(1, &pFilter, NULL))
		{
			FILTER_INFO filterInfo;
			memset(&filterInfo, 0, sizeof(filterInfo));
			pFilter->QueryFilterInfo(&filterInfo);
			SAFE_RELEASE(filterInfo.pGraph);

			SAFE_RELEASE(pFilter);

			OutputDebugStringA("  Filter: ");
			OutputDebugStringW(filterInfo.achName);
			OutputDebugStringA("\n");
		}
	}
}

// {04FE9017-F873-410E-871E-AB91661A4EF7}
DEFINE_GUID(CLSID_FFDSHOW_VIDEO_DECODER,
0x04FE9017, 0xF873, 0x410E, 0x87, 0x1E, 0xAB, 0x91, 0x66, 0x1A, 0x4E, 0xF7);

HRESULT DirectShow_Add_ffdshow_VideoDecoder(IFilterGraph2* pIFilterGraph)
{
	_ASSERTE(NULL != pIFilterGraph);
	if (NULL == pIFilterGraph)
		return E_INVALIDARG;

	CComPtr<IBaseFilter> pVideoDecoderFilter;
	HRESULT hr = CoCreateInstance(CLSID_FFDSHOW_VIDEO_DECODER, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void **)&pVideoDecoderFilter);
	if (SUCCEEDED(hr))
		hr = pIFilterGraph->AddFilter(pVideoDecoderFilter, L"ffdshow Video Decoder");

	return hr;
}

HRESULT DirectShow_SetVideoWindow(IBaseFilter* pVideoRendererFilter, CWnd* pWnd)
{
	_ASSERTE(NULL != pVideoRendererFilter && NULL != pWnd);
	if (NULL == pVideoRendererFilter || NULL == pWnd)
		return E_INVALIDARG;

	// set video window
	CComPtr<IVideoWindow> pVideoWindow;
	HRESULT hr = pVideoRendererFilter->QueryInterface(IID_IVideoWindow, (void**)&pVideoWindow);
	_ASSERT(SUCCEEDED(hr));

	CRect rcClient;
	pWnd->GetClientRect(&rcClient);

	hr = pVideoWindow->put_Owner((OAHWND)pWnd->m_hWnd);
	_ASSERT(SUCCEEDED(hr));

	pVideoWindow->put_WindowStyleEx(0);
	pVideoWindow->put_WindowStyle(WS_CHILD);
	pVideoWindow->SetWindowPosition(0, 0, rcClient.Width(), rcClient.Height());

	return hr; 	
}

//! Set audio renderer as reference clock for the graph
HRESULT DirectShow_SetAudioAsSyncSource(IBaseFilter* pAudioRendererFilter)
{
	_ASSERTE(NULL != pAudioRendererFilter);
	if (NULL == pAudioRendererFilter)
		return E_INVALIDARG;

	CComPtr<IReferenceClock> pRefClock;
	HRESULT hr = pAudioRendererFilter->QueryInterface(IID_IReferenceClock, (void**)&pRefClock);
	_ASSERT(SUCCEEDED(hr));

	CComPtr<IMediaFilter> pMediaFilter;
	hr = g_pFilterGraph->QueryInterface(IID_IMediaFilter, (void**)&pMediaFilter);
	_ASSERT(SUCCEEDED(hr));

	hr = pMediaFilter->SetSyncSource(pRefClock);
	_ASSERT(SUCCEEDED(hr));

#if 1
	// TS WORKAROUND 15-Aug-11: Avoid cracking noise after graph is started 
	// by increasing the error tolerance of the Audio Renderer from 20ms to 200ms
	CComPtr<IAMClockSlave> pIAMClockSlave;
	HRESULT hrTmp = pAudioRendererFilter->QueryInterface(IID_IAMClockSlave, (void**)&pIAMClockSlave);
	if (SUCCEEDED(hrTmp))
		hrTmp = pIAMClockSlave->SetErrorTolerance(200);
	_ASSERT(SUCCEEDED(hrTmp));
#endif
	return hr;
}

HRESULT DirectShow_RenderPins(IFilterGraph2* pIFilterGraph, IUnknown* pRawAvSourceFilter, CWnd* pWnd)
{
	_ASSERTE(NULL != pIFilterGraph && NULL != pRawAvSourceFilter && NULL != pWnd);
	if (NULL == pIFilterGraph || NULL == pRawAvSourceFilter || NULL == pWnd)
		return E_INVALIDARG;

	HRESULT hr = S_FALSE;

	// Add "Video Mixing Renderer 9" filter (VMR-9)
	CComPtr<IBaseFilter> pVideoRendererFilter;
	hr = CoCreateInstance(CLSID_VideoMixingRenderer9, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void **)&pVideoRendererFilter);
	_ASSERT(SUCCEEDED(hr));

	hr = g_pFilterGraph->AddFilter(pVideoRendererFilter, L"VMR-9");
	_ASSERT(SUCCEEDED(hr));

	// Add Audio Renderer filter (must be in the graph as reference clock even if it is not connected)
	CComPtr<IBaseFilter> pAudioRendererFilter;
	hr = CoCreateInstance(CLSID_DSoundRender, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void **)&pAudioRendererFilter);
	_ASSERT(SUCCEEDED(hr));

	hr = g_pFilterGraph->AddFilter(pAudioRendererFilter, L"DirectSound Renderer");
	_ASSERT(SUCCEEDED(hr));

	// Render
	hr = g_pCapBuilder->RenderStream(NULL, &MEDIATYPE_Video, pRawAvSourceFilter, NULL, pVideoRendererFilter);
	_ASSERT(SUCCEEDED(hr));

	hr = g_pCapBuilder->RenderStream(NULL, &MEDIATYPE_Audio, pRawAvSourceFilter, NULL, pAudioRendererFilter);
	_ASSERT(SUCCEEDED(hr));

	// Set video window
	hr = DirectShow_SetVideoWindow(pVideoRendererFilter, pWnd);
	_ASSERT(SUCCEEDED(hr));

	// Set audio renderer as reference clock for the graph
	hr = DirectShow_SetAudioAsSyncSource(pAudioRendererFilter);
	_ASSERT(SUCCEEDED(hr));

	// Set volume to maximum
	{
		CComPtr<IBasicAudio> pBasicAudio;
		hr = pAudioRendererFilter->QueryInterface(IID_IBasicAudio, (void**)&pBasicAudio);
		_ASSERT(SUCCEEDED(hr));
		hr = pBasicAudio->put_Volume(0);
		_ASSERT(SUCCEEDED(hr));
	}

	return hr;
}


//! Must be called after the Game Capture filter was added to the graph
HRESULT QueryCapsForSelectedDevice(IBaseFilter* pGameCaptureFilter, VIDEO_CAPTURE_FILTER_DEVICE_CAPS* pCaps)
{
	_ASSERTE(NULL != pGameCaptureFilter && NULL != pCaps);
	if (NULL == pGameCaptureFilter || NULL == pCaps)
		return E_INVALIDARG;

	// Get current device index
	int deviceIdx = 0;
	CComPtr<IElgatoVideoCaptureFilter6> pICapFilter;
	HRESULT hr = pGameCaptureFilter->QueryInterface(IID_IElgatoVideoCaptureFilter6, (void**)&pICapFilter);
	_ASSERT(SUCCEEDED(hr));
	if (SUCCEEDED(hr))
	{
		VIDEO_CAPTURE_FILTER_SETTINGS_EX settingsEx;
		hr = pICapFilter->GetSettingsEx(&settingsEx);
		_ASSERT(SUCCEEDED(hr));
		if (SUCCEEDED(hr))
			deviceIdx = settingsEx.deviceIndex;
	}

	// Query caps for current device index
	CComPtr<IElgatoVideoCaptureFilterEnumeration> pICapFilterEnum;
	if (SUCCEEDED(hr))
	{
		hr = pGameCaptureFilter->QueryInterface(IID_IElgatoVideoCaptureFilterEnumeration, (void**)&pICapFilterEnum);
		int numDevices = 0;
		hr = pICapFilterEnum->GetNumAvailableDevices(&numDevices);
		_ASSERT(SUCCEEDED(hr));
		if (SUCCEEDED(hr) && deviceIdx < numDevices)
		{
			VIDEO_CAPTURE_FILTER_DEVICE_CAPS deviceCaps;
			ZeroMemory(&deviceCaps, sizeof(deviceCaps));
			hr = pICapFilterEnum->GetDeviceCaps(deviceIdx, &deviceCaps);
			_ASSERT(SUCCEEDED(hr));
			if (SUCCEEDED(hr))
				*pCaps = deviceCaps;
		}

	}
	return hr;
}

//! Sample code
HRESULT CVideoCaptureFilterSampleDlg::QueryCaps()
{
	CoInitialize(NULL);

	CComPtr<IBaseFilter> pElgatoCapFilter;

	// Instantiate "Elgato Game Capture HD" filter
	HRESULT hr = CoCreateInstance(CLSID_ElgatoVideoCaptureFilter, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void **)&pElgatoCapFilter);
	_ASSERT(SUCCEEDED(hr));
	if (SUCCEEDED(hr))
	{
		CComPtr<IElgatoVideoCaptureFilterEnumeration> pICapFilterEnum;
		hr = pElgatoCapFilter->QueryInterface(IID_IElgatoVideoCaptureFilterEnumeration, (void**)&pICapFilterEnum);
		_ASSERT(SUCCEEDED(hr));
		if (SUCCEEDED(hr))
		{
			int numDevices = 0;
			hr = pICapFilterEnum->GetNumAvailableDevices(&numDevices);
			_ASSERT(SUCCEEDED(hr));
			if (SUCCEEDED(hr) && numDevices > 0)
			{
				for (int i = 0; i < numDevices; i++)
				{
					VIDEO_CAPTURE_FILTER_DEVICE_CAPS deviceCaps;
					ZeroMemory(&deviceCaps, sizeof(deviceCaps));
					hr = pICapFilterEnum->GetDeviceCaps(i, &deviceCaps);
					_ASSERT(SUCCEEDED(hr));

					OutputDebugStringA("  Device: ");
					OutputDebugStringW(deviceCaps.deviceName);
					OutputDebugStringA("\n");
				}
			}
		}
	}

	CoUninitialize();

	return hr;
}


HRESULT CVideoCaptureFilterSampleDlg::InitGraph()
{
	CoInitialize(NULL);

	HRESULT hr = S_OK;

	/*----------------------------------------------------------------------------
	// 1. Create the graph builder & filter graph
	-----------------------------------------------------------------------------*/
	// FMB NOTE: This is pretty safe, so we can assume it will not fail
	hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC, IID_ICaptureGraphBuilder2, (void **)&g_pCapBuilder);
	_ASSERT(SUCCEEDED(hr));

	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IFilterGraph2, (void **)&g_pFilterGraph);
	_ASSERT(SUCCEEDED(hr));

	hr = g_pCapBuilder->SetFiltergraph(g_pFilterGraph);
	_ASSERT(SUCCEEDED(hr));

	hr = g_pFilterGraph->QueryInterface(IID_IMediaControl, (void**)&g_pMediaControl);
	_ASSERT(SUCCEEDED(hr));


	/*-----------------------------------------------------------------------------
	// 2. Add the Elgato Video Capture filter
	-----------------------------------------------------------------------------*/

	// Add "Elgato Game Capture HD" filter
	hr = CoCreateInstance(CLSID_ElgatoVideoCaptureFilter, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void **)&g_pElgatoCapFilter);
	_ASSERT(SUCCEEDED(hr));

	hr = g_pFilterGraph->AddFilter(g_pElgatoCapFilter, L"Elgato Game Capture HD");
	_ASSERT(SUCCEEDED(hr));


	// Query caps
	VIDEO_CAPTURE_FILTER_DEVICE_CAPS caps;
	hr = QueryCapsForSelectedDevice(g_pElgatoCapFilter, &caps);
	_ASSERT(SUCCEEDED(hr));

	/*-----------------------------------------------------------------------------
	// 3a) Render from MPEG-TS pin (if available)
	-----------------------------------------------------------------------------*/
	CComPtr<IPin> pMpegTsOutput;
	HRESULT hrTmp = g_pCapBuilder->FindPin(g_pElgatoCapFilter, PINDIR_OUTPUT, NULL, &MEDIATYPE_Stream, TRUE, 0, &pMpegTsOutput);
	bool hasMpegTsPin = SUCCEEDED(hrTmp); // Elgato note: older versions of the capture filter don't have an MPEG-TS pin

	if (hasMpegTsPin && caps.hardwareDeliversCompressedVideoData)
	{
		// Add Microsoft Demultiplexer filter
		CComPtr<IBaseFilter> pDemultiplexerFilter;
		hr = CoCreateInstance(CLSID_MPEG2Demultiplexer, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void **)&pDemultiplexerFilter);
		_ASSERT(SUCCEEDED(hr));

		hr = g_pFilterGraph->AddFilter(pDemultiplexerFilter, L"MS Demux");
		_ASSERT(SUCCEEDED(hr));

		// Configure Microsoft Demultiplexer filter
		CComPtr<IPin> pVideoPin;
		CComPtr<IPin> pAudioPin;
		{
			CComPtr<IMpeg2Demultiplexer> pIDemultiplexer;
			hr = pDemultiplexerFilter->QueryInterface(IID_IMpeg2Demultiplexer, (void**)&pIDemultiplexer);
			_ASSERT(SUCCEEDED(hr));

			// Add video pin
			{
				hr = pIDemultiplexer->CreateOutputPin(GetVideoMediaType(), L"H.264", &pVideoPin);
				_ASSERT(SUCCEEDED(hr));

				CComPtr<IMPEG2PIDMap> pIPidMap;
				hr = pVideoPin->QueryInterface(__uuidof(IMPEG2PIDMap), (void**)&pIPidMap);
				_ASSERT(SUCCEEDED(hr));

				ULONG pid = ELGATO_VCF_VIDEO_PID;
				hr = pIPidMap->MapPID(1, &pid, MEDIA_ELEMENTARY_STREAM);
				_ASSERT(SUCCEEDED(hr));
			}

			// Add audio pin
			{
				hr = pIDemultiplexer->CreateOutputPin(GetAudioMediaType(), L"AAC", &pAudioPin);
				_ASSERT(SUCCEEDED(hr));

				CComPtr<IMPEG2PIDMap> pIPidMap;
				hr = pAudioPin->QueryInterface(__uuidof(IMPEG2PIDMap), (void**)&pIPidMap);
				_ASSERT(SUCCEEDED(hr));

				ULONG pid = ELGATO_VCF_AUDIO_PID;
				hr = pIPidMap->MapPID(1, &pid, MEDIA_ELEMENTARY_STREAM);
				_ASSERT(SUCCEEDED(hr));
			}
		}

		// Connect filters
		CComPtr<IPin> pDemultiplexerInput;
		hr = g_pCapBuilder->FindPin(pDemultiplexerFilter, PINDIR_INPUT, NULL, NULL, TRUE, 0, &pDemultiplexerInput);
		_ASSERT(SUCCEEDED(hr));

		hr = g_pFilterGraph->Connect(pMpegTsOutput, pDemultiplexerInput);
		_ASSERT(SUCCEEDED(hr));

		// Add ffsshow video decoder if installed (because Microsoft DTV-DVD Video Decoder is buggy)
		hr = DirectShow_Add_ffdshow_VideoDecoder(g_pFilterGraph);
		if (FAILED(hr))
			MessageBoxA(m_hWnd, "ffdshow not installed.\n\nUsing Microsoft DTV-DVD Video Decoder as fallback!", "Warning", MB_OK|MB_ICONWARNING);
		
		CWnd* pWnd = GetDlgItem(IDC_VIDEOWINDOW_COMPRESSED);
		hr = DirectShow_RenderPins(g_pFilterGraph, pDemultiplexerFilter, pWnd);
	}

	/*-----------------------------------------------------------------------------
	// 3b) Render from raw audio and video pins
	-----------------------------------------------------------------------------*/
	if (caps.hardwareDeliversRawVideoData)
	{
		CWnd* pWnd = GetDlgItem(IDC_VIDEOWINDOW_RAW);
		hr = DirectShow_RenderPins(g_pFilterGraph, g_pElgatoCapFilter, pWnd);
	}

	/*-----------------------------------------------------------------------------
	// 4. Start Playback
	-----------------------------------------------------------------------------*/

	// play
	hr = g_pMediaControl->Run();
	_ASSERT(SUCCEEDED(hr));


#ifdef _DEBUG	// Dump graph
	DirectShow_DumpGraph(g_pFilterGraph);
#endif

	return hr;
}

HRESULT CVideoCaptureFilterSampleDlg::DestroyGraph()
{
	HRESULT hr = S_FALSE;
	
	_ASSERTE(NULL != g_pMediaControl);
	hr = g_pMediaControl->Stop();
	_ASSERT(SUCCEEDED(hr));


	// Elgato note: This may take a few seconds
	_ASSERTE(NULL != g_pFilterGraph);
	hr = g_pFilterGraph->RemoveFilter(g_pElgatoCapFilter);
	_ASSERT(SUCCEEDED(hr));

	return S_OK;
}

HRESULT CVideoCaptureFilterSampleDlg::ShowProperties()
{
	HRESULT hr = E_FAIL;
	if(NULL != g_pElgatoCapFilter)
	{
		CComPtr<ISpecifyPropertyPages> pSpec;
		hr = g_pElgatoCapFilter->QueryInterface(IID_ISpecifyPropertyPages, (void **) &pSpec);
		if (hr == S_OK) 
		{
			CAUUID  cauuid;
			hr = pSpec->GetPages(&cauuid);
			if (hr == S_OK && cauuid.cElems > 0)
			{
				IBaseFilter* pFilter = g_pElgatoCapFilter;
				hr = OleCreatePropertyFrame(NULL,40, 40, NULL, 1,(IUnknown **)&pFilter, 
					cauuid.cElems,(GUID *)cauuid.pElems, 0, 0, NULL);
				CoTaskMemFree(cauuid.pElems);
			}
		}
	}
	return hr;
}




