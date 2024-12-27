#pragma once
#include "afxdialogex.h"
#include "CSortListCtrl.h"

// CReportListDlg dialog

class CReportListDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CReportListDlg)

public:
	CReportListDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CReportListDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REPORT_LIST_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CSortListCtrl m_List;
};
