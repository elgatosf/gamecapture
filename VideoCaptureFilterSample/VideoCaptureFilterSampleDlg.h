
// VideoCaptureFilterSampleDlg.h : header file
//

#pragma once


// CVideoCaptureFilterSampleDlg dialog
class CVideoCaptureFilterSampleDlg : public CDialog
{
// Construction
public:
	CVideoCaptureFilterSampleDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_VIDEOCAPTUREFILTERSAMPLE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	virtual void OnOK();
	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedProperties();

	HRESULT InitGraph();
	HRESULT DestroyGraph();
	HRESULT ShowProperties();

};
