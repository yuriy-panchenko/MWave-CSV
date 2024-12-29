#pragma once
#include "afxdialogex.h"
#include "chart.h"

// CChartDlg dialog

class CChartDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CChartDlg)

public:
	using chart_ptr = std::unique_ptr<chart::base>;
	CChartDlg(chart_ptr&&, CWnd* pParent = nullptr);   // standard constructor
	virtual ~CChartDlg();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHART_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()

private:
	chart_ptr m_Chart;
};
