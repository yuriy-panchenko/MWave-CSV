// CSRevParamDlg.cpp : implementation file
//

#include "pch.h"
#include "MWave CSV.h"
#include "afxdialogex.h"
#include "CSRevParamDlg.h"


// CSRevParamDlg dialog

IMPLEMENT_DYNAMIC(CSRevParamDlg, CDialogEx)

CSRevParamDlg::CSRevParamDlg(const int period, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SREVERSAL_PARAM_DLG, pParent)
	, Period{ period }
{

}

CSRevParamDlg::~CSRevParamDlg()
{
}

void CSRevParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PERIOD, Period);
	DDV_MinMaxInt(pDX, Period, 1, 10000);
	DDX_Control(pDX, IDC_PERIOD_SPIN, m_spinPeriod);
}


BEGIN_MESSAGE_MAP(CSRevParamDlg, CDialogEx)
END_MESSAGE_MAP()


// CSRevParamDlg message handlers


BOOL CSRevParamDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_spinPeriod.SetRange(1, 10000);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
