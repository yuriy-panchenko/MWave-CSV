// CChartDlg.cpp : implementation file
//

#include "pch.h"
#include "MWave CSV.h"
#include "afxdialogex.h"
#include "CChartDlg.h"


// CChartDlg dialog

IMPLEMENT_DYNAMIC(CChartDlg, CDialogEx)

CChartDlg::CChartDlg(chart_ptr&& chart, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHART_DLG, pParent)
	, m_Chart{ std::move(chart) }
{

}

CChartDlg::~CChartDlg()
{
}

void CChartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CChartDlg, CDialogEx)
END_MESSAGE_MAP()


// CChartDlg message handlers
