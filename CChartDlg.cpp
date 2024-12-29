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
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CChartDlg message handlers


BOOL CChartDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	//return CDialogEx::OnEraseBkgnd(pDC);

	CRect rect;
	GetClientRect(rect);
	InvalidateRect(rect);
	m_Chart->prepare_context(*pDC, rect);

	return FALSE;
}


void CChartDlg::OnPaint()
{
	m_Chart->draw((CDC&)CPaintDC{ this }); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialogEx::OnPaint() for painting messages
}
