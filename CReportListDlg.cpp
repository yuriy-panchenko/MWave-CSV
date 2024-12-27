// CReportListDlg.cpp : implementation file
//

#include "pch.h"
#include "MWave CSV.h"
#include "afxdialogex.h"
#include "CReportListDlg.h"


// CReportListDlg dialog

IMPLEMENT_DYNAMIC(CReportListDlg, CDialogEx)

CReportListDlg::CReportListDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_REPORT_LIST_DLG, pParent)
{

}

CReportListDlg::~CReportListDlg()
{
}

void CReportListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_List);
}


BEGIN_MESSAGE_MAP(CReportListDlg, CDialogEx)
END_MESSAGE_MAP()


// CReportListDlg message handlers
