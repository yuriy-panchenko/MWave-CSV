#pragma once
#include "afxdialogex.h"


// CTradeSettingDlg dialog

class CTradeSettingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTradeSettingDlg)

public:
	CTradeSettingDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CTradeSettingDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TRADE_SETTINGS_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	double m_MinProfit;
};
