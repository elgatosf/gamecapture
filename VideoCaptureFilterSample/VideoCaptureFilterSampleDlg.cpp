
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


/*=============================================================================
// BUILD GRAPH
=============================================================================*/


#include <initguid.h>
#include <streams.h> //  for DirectShow headers
#pragma comment(lib, "strmbaseu.lib")

// {39F50F4C-99E1-464a-B6F9-D605B4FB5918}
DEFINE_GUID(CLSID_VideoCaptureFilter, 
0x39f50f4c, 0x99e1, 0x464a, 0xb6, 0xf9, 0xd6, 0x5, 0xb4, 0xfb, 0x59, 0x18);


#ifndef SAFE_RELEASE
#define SAFE_RELEASE(_pI_) { if(_pI_) { (_pI_)->Release(); (_pI_)=NULL;} }
#endif


CComPtr<ICaptureGraphBuilder2>	pCapBuilder;
CComPtr<IFilterGraph2>			pFilterGraph;
CComPtr<IMediaControl>			pMediaControl;
CComPtr<IVideoWindow>			pVideoWindow;
CComPtr<IBasicAudio>			pBasicAudio;
CComPtr<IBaseFilter>			pCapFilter;


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

    hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IFilterGraph2,  (void **)&pFilterGraph);
	_ASSERT(SUCCEEDED(hr));

    hr = pCapBuilder->SetFiltergraph(pFilterGraph);
	_ASSERT(SUCCEEDED(hr));

	hr = pFilterGraph->QueryInterface(IID_IMediaControl, (void**)&pMediaControl);
	_ASSERT(SUCCEEDED(hr));


	/*-----------------------------------------------------------------------------
	// Build graph
	-----------------------------------------------------------------------------*/

	// Add "Elgato Game Capture HD" filter
	hr = CoCreateInstance(CLSID_VideoCaptureFilter, NULL, CLSCTX_INPROC, IID_IBaseFilter,  (void **)&pCapFilter);
	_ASSERT(SUCCEEDED(hr));

	hr = pFilterGraph->AddFilter(pCapFilter, L"Elgato Game Capture HD");
	_ASSERT(SUCCEEDED(hr));

	// Add "Video Mixing Renderer 9" filter (VMR-9)
	CComPtr<IBaseFilter> pVideoRendererFilter;
	hr = CoCreateInstance(CLSID_VideoMixingRenderer9, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void **)&pVideoRendererFilter);
	_ASSERT(SUCCEEDED(hr));

	hr = pFilterGraph->AddFilter(pVideoRendererFilter, L"VMR-9");
	_ASSERT(SUCCEEDED(hr));

	// Add Audio Renderer filter
	CComPtr<IBaseFilter> pAudioRendererFilter;
	hr = CoCreateInstance(CLSID_DSoundRender, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void **)&pAudioRendererFilter);
	_ASSERT(SUCCEEDED(hr));

	hr = pFilterGraph->AddFilter(pAudioRendererFilter, L"DirectSound Renderer");
	_ASSERT(SUCCEEDED(hr));

	hr = pCapBuilder->RenderStream(NULL, &MEDIATYPE_Video, pCapFilter, NULL, pVideoRendererFilter);
	_ASSERT(SUCCEEDED(hr));

	hr = pCapBuilder->RenderStream(NULL, &MEDIATYPE_Audio, pCapFilter, NULL, pAudioRendererFilter);
	_ASSERT(SUCCEEDED(hr));

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

	return hr;
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




