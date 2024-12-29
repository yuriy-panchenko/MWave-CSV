
// ChildView.h : interface of the CChildView class
//
#pragma once
#include "trd_leaf.h"
#include "Trader.h"

// CChildView window
class CReportListDlg;
class CChartDlg;

class CChildView : public CWnd
{


	// Construction
public:
	CChildView();

	// Attributes
public:

	// Operations
public:

	// Overrides
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void Serialize(CArchive& ar);

	// Implementation
public:
	virtual ~CChildView();

	// Generated message map functions
protected:
	afx_msg void OnPaint();
	afx_msg void OnFileOpen();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTreeSelChanged(NMHDR*, LRESULT*);
	afx_msg void OnTreeMouseClick(NMHDR*, LRESULT*);
	afx_msg void OnInitialState();
	afx_msg void OnUpdateInitialState(CCmdUI* pCmdUI);
	afx_msg LRESULT OnTreeItemChecked(WPARAM, LPARAM);
	afx_msg void OnListItemClicked(NMHDR*, LRESULT*);
	afx_msg void OnProjectLoad();
	afx_msg void OnProjectSave();
	afx_msg void OnTrade();
	afx_msg void OnUpdateTrade(CCmdUI* pCmdUI);
	afx_msg void OnTrade2();
	afx_msg void OnUpdateTrade2(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()

private:
	void LoadTree();
	void LoadList();
	void Insert(seq::leaf& l, HTREEITEM hParent = TVI_ROOT);
	void TreeFromPatterns();
	const seq::leaf* FindLeaf(HTREEITEM)const;
	seq::leaf* FindLeaf(HTREEITEM);
	MWINFO GetInfo(const seq::leaf& l)const;
	void Quotes2MWave(int period);
	void LoadFile(const std::filesystem::path&);
	
	void UpdateChildren(const seq::leaf& l);
	void UpdateParents(seq::leaf& l);
	std::unique_ptr<trd::leaf> Clone(const seq::leaf&, trd::leaf* parent=nullptr)const;
	Trader::TradePoint ToTimePrice(const PNT& pnt)const;
	void ShowReportDlg(const Trader&);
	void ShowCumulativeChart(const Trader&);

private:
	CString m_Filename, m_ProjectFilename;
	std::vector<MWAVE> m_MWaves;
	seq::chain m_Patterns;
	std::vector<QUOTE_REC> m_Quotes;
	seq::leaf m_Tree[32];
	mwave::SReversal m_SRev;

	CRect m_rInfo;
	CTreeCtrl m_ctrlTree;
	CListCtrl m_ctrlList;
	CImageList m_imgList;
	CTypedPtrArray<CPtrArray, CReportListDlg*> m_Reports;
	CTypedPtrArray<CPtrArray, CChartDlg*> m_Charts;
};
