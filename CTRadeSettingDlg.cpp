// CTradeSettingDlg.cpp : implementation file
//

#include "pch.h"
#include "MWave CSV.h"
#include "afxdialogex.h"
#include "CTRadeSettingDlg.h"


// CTradeSettingDlg dialog

IMPLEMENT_DYNAMIC(CTradeSettingDlg, CDialogEx)

CTradeSettingDlg::CTradeSettingDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TRADE_SETTINGS_DLG, pParent)
	, m_MinProfit(0)
{

}

CTradeSettingDlg::~CTradeSettingDlg()
{
}

void CTradeSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MIN_PROFIT_EDIT, m_MinProfit);
}


BEGIN_MESSAGE_MAP(CTradeSettingDlg, CDialogEx)
END_MESSAGE_MAP()


// CTradeSettingDlg message handlers
