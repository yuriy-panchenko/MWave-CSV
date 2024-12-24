#pragma once
#include "afxdialogex.h"


// CSRevParamDlg dialog

class CSRevParamDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSRevParamDlg)

public:
	CSRevParamDlg(int period, CWnd* pParent = nullptr);   // standard constructor
	virtual ~CSRevParamDlg();

	int Period;
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SREVERSAL_PARAM_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	CSpinButtonCtrl m_spinPeriod;
};
