
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
	InitGraph();

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
	#include "../VideoCaptureFilter/IVideoCaptureFilter.h"
#else
	#define ELGATO_VCF_VIDEO_PID	100	//!< video PID in MPEG-TS stream
	#define ELGATO_VCF_AUDIO_PID	101	//!< audio PID in MPEG-TS stream

	// Filter guid of Elgato Video Capture Filter
	// {39F50F4C-99E1-464a-B6F9-D605B4FB5918}
	DEFINE_GUID(CLSID_ElgatoVideoCaptureFilter, 
	0x39f50f4c, 0x99e1, 0x464a, 0xb6, 0xf9, 0xd6, 0x5, 0xb4, 0xfb, 0x59, 0x18);
#endif

// renderFromMpegTsPin: true  - use compressed data from the MPEG-TS pin (if present)
//                      false - use raw data from the video and audio pins
const bool renderFromMpegTsPin = false;



#ifndef SAFE_RELEASE
#define SAFE_RELEASE(_pI_) { if(_pI_) { (_pI_)->Release(); (_pI_)=NULL;} }
#endif


CComPtr<ICaptureGraphBuilder2>	pCapBuilder;
CComPtr<IFilterGraph2>			pFilterGraph;
CComPtr<IMediaControl>			pMediaControl;
CComPtr<IVideoWindow>			pVideoWindow;
CComPtr<IBasicAudio>			pBasicAudio;
CComPtr<IBaseFilter>			pCapFilter;

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

void _DumpGraph(IFilterGraph2* pIFilterGraph)
{
	_ASSERTE(NULL != pIFilterGraph);
	if (NULL != pIFilterGraph)
	{
		CComPtr<IEnumFilters> pEnumFilters;
		HRESULT hrTmp = pFilterGraph->EnumFilters(&pEnumFilters);
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
}

HRESULT CVideoCaptureFilterSampleDlg::InitGraph()
{
	CoInitialize(NULL);

	HRESULT hr = S_OK;

	/*----------------------------------------------------------------------------
	// Create the graph builder & filter graph
	-----------------------------------------------------------------------------*/
	// FMB NOTE: This is pretty safe, so we can assume it will not fail
	hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC, IID_ICaptureGraphBuilder2, (void **)&pCapBuilder);
	_ASSERT(SUCCEEDED(hr));

	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IFilterGraph2, (void **)&pFilterGraph);
	_ASSERT(SUCCEEDED(hr));

	hr = pCapBuilder->SetFiltergraph(pFilterGraph);
	_ASSERT(SUCCEEDED(hr));

	hr = pFilterGraph->QueryInterface(IID_IMediaControl, (void**)&pMediaControl);
	_ASSERT(SUCCEEDED(hr));


	/*-----------------------------------------------------------------------------
	// Build graph
	-----------------------------------------------------------------------------*/

	// Add "Elgato Game Capture HD" filter
	hr = CoCreateInstance(CLSID_ElgatoVideoCaptureFilter, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void **)&pCapFilter);
	_ASSERT(SUCCEEDED(hr));

	hr = pFilterGraph->AddFilter(pCapFilter, L"Elgato Game Capture HD");
	_ASSERT(SUCCEEDED(hr));

	// Add "Video Mixing Renderer 9" filter (VMR-9)
	CComPtr<IBaseFilter> pVideoRendererFilter;
	hr = CoCreateInstance(CLSID_VideoMixingRenderer9, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void **)&pVideoRendererFilter);
	_ASSERT(SUCCEEDED(hr));

	hr = pFilterGraph->AddFilter(pVideoRendererFilter, L"VMR-9");
	_ASSERT(SUCCEEDED(hr));

	// Add Audio Renderer filter (must be in the graph as reference clock even if it is not connected)
	CComPtr<IBaseFilter> pAudioRendererFilter;
	hr = CoCreateInstance(CLSID_DSoundRender, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void **)&pAudioRendererFilter);
	_ASSERT(SUCCEEDED(hr));

	hr = pFilterGraph->AddFilter(pAudioRendererFilter, L"DirectSound Renderer");
	_ASSERT(SUCCEEDED(hr));

	/*-----------------------------------------------------------------------------
	// a) Render from MPEG-TS pin (if available)
	-----------------------------------------------------------------------------*/
	CComPtr<IPin> pMpegTsOutput;
	HRESULT hrTmp = pCapBuilder->FindPin(pCapFilter, PINDIR_OUTPUT, NULL, &MEDIATYPE_Stream, TRUE, 0, &pMpegTsOutput);
	bool hasMpegTsPin = SUCCEEDED(hrTmp); // Elgato note: older versions of the capture filter don't have an MPEG-TS pin

	if (hasMpegTsPin && renderFromMpegTsPin)
	{
		// Add Microsoft Demultiplexer filter
		CComPtr<IBaseFilter> pDemultiplexerFilter;
		hr = CoCreateInstance(CLSID_MPEG2Demultiplexer, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void **)&pDemultiplexerFilter);
		_ASSERT(SUCCEEDED(hr));

		hr = pFilterGraph->AddFilter(pDemultiplexerFilter, L"MS Demux");
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
		hr = pCapBuilder->FindPin(pDemultiplexerFilter, PINDIR_INPUT, NULL, NULL, TRUE, 0, &pDemultiplexerInput);
		_ASSERT(SUCCEEDED(hr));

		hr = pFilterGraph->Connect(pMpegTsOutput, pDemultiplexerInput);
		_ASSERT(SUCCEEDED(hr));

		hr = pCapBuilder->RenderStream(NULL, &MEDIATYPE_Video, pVideoPin, NULL, pVideoRendererFilter);
		_ASSERT(SUCCEEDED(hr));

		hr = pCapBuilder->RenderStream(NULL, &MEDIATYPE_Audio, pAudioPin, NULL, pAudioRendererFilter);
		_ASSERT(SUCCEEDED(hr));
	}
	/*-----------------------------------------------------------------------------
	// b) Render from audio and video pins
	-----------------------------------------------------------------------------*/
	else
	{
		hr = pCapBuilder->RenderStream(NULL, &MEDIATYPE_Video, pCapFilter, NULL, pVideoRendererFilter);
		_ASSERT(SUCCEEDED(hr));

		hr = pCapBuilder->RenderStream(NULL, &MEDIATYPE_Audio, pCapFilter, NULL, pAudioRendererFilter);
		_ASSERT(SUCCEEDED(hr));
	}

	/*-----------------------------------------------------------------------------
	// Set audio renderer as reference clock for the graph
	-----------------------------------------------------------------------------*/
	if (SUCCEEDED(hr))
	{
		CComPtr<IReferenceClock> pRefClock;
		hr = pAudioRendererFilter->QueryInterface(IID_IReferenceClock, (void**)&pRefClock);
		_ASSERT(SUCCEEDED(hr));
		CComPtr<IMediaFilter> pMediaFilter;
		hr = pFilterGraph->QueryInterface(IID_IMediaFilter, (void**)&pMediaFilter);
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
	}

	/*-----------------------------------------------------------------------------
	// Playback
	-----------------------------------------------------------------------------*/
	// set video window
	if (SUCCEEDED(hr))
	{
		hr = pFilterGraph->QueryInterface(IID_IVideoWindow, (void**)&pVideoWindow);
		_ASSERT(SUCCEEDED(hr));


		CWnd* pWnd = GetDlgItem(IDC_VIDEOWINDOW);
		CRect rcClient;
		pWnd->GetClientRect(&rcClient);

		hr = pVideoWindow->put_Owner((OAHWND)pWnd->m_hWnd);
		_ASSERT(SUCCEEDED(hr));

		pVideoWindow->put_WindowStyleEx(0);
		pVideoWindow->put_WindowStyle(WS_CHILD);
		pVideoWindow->SetWindowPosition(0, 0, rcClient.Width(), rcClient.Height());
	}

	// play
	hr = pMediaControl->Run();
	_ASSERT(SUCCEEDED(hr));

	// set volume to maximum
	hr = pFilterGraph->QueryInterface(IID_IBasicAudio, (void**)&pBasicAudio);
	_ASSERT(SUCCEEDED(hr));
	hr = pBasicAudio->put_Volume(0);
	_ASSERT(SUCCEEDED(hr));	

#ifdef _DEBUG	// Dump graph
	_DumpGraph(pFilterGraph);
#endif

	return hr;
}

HRESULT CVideoCaptureFilterSampleDlg::DestroyGraph()
{
	HRESULT hr = S_FALSE;
	
	_ASSERTE(NULL != pMediaControl);
	hr = pMediaControl->Stop();
	_ASSERT(SUCCEEDED(hr));


	// Elgato note: This may take a few seconds
	_ASSERTE(NULL != pFilterGraph);
	hr = pFilterGraph->RemoveFilter(pCapFilter);
	_ASSERT(SUCCEEDED(hr));

	return S_OK;
}

HRESULT CVideoCaptureFilterSampleDlg::ShowProperties()
{
	HRESULT hr = E_FAIL;
	if(NULL != pCapFilter)
	{
		CComPtr<ISpecifyPropertyPages> pSpec;
		hr = pCapFilter->QueryInterface(IID_ISpecifyPropertyPages, (void **) &pSpec);
		if (hr == S_OK) 
		{
			CAUUID  cauuid;
			hr = pSpec->GetPages(&cauuid);
			if (hr == S_OK && cauuid.cElems > 0)
			{
				IBaseFilter* pFilter = pCapFilter;
				hr = OleCreatePropertyFrame(NULL,40, 40, NULL, 1,(IUnknown **)&pFilter, 
					cauuid.cElems,(GUID *)cauuid.pElems, 0, 0, NULL);
				CoTaskMemFree(cauuid.pElems);
			}
		}
	}
	return hr;
}




