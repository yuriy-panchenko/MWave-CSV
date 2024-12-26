
// ChildView.h : interface of the CChildView class
//


#pragma once
#include "seq.h"

struct PNT
{
	double value;
	int index;
};

struct MWAVE
{
	PNT mw[5], leg, next_leg;
	double PProfit, maxDD;
};

struct QUOTE_REC
{
	CTime time;
	double open, high, low, close;
	int volTick, volume, spread;
};

// CChildView window

class CChildView : public CWnd
{
	struct MWInfo { double PProfit, maxDD, Profit, Loss; };


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
	afx_msg LRESULT OnItemChecked(WPARAM, LPARAM);
	afx_msg void OnListItemClicked(NMHDR*, LRESULT*);
	afx_msg void OnProjectLoad();
	afx_msg void OnProjectSave();
	DECLARE_MESSAGE_MAP()

private:
	void LoadTree();
	void LoadList();
	void Insert(seq::leaf& l, HTREEITEM hParent = TVI_ROOT);
	void UpdateTree();
	const seq::leaf* FindLeaf(HTREEITEM)const;
	seq::leaf* FindLeaf(HTREEITEM);
	MWInfo GetInfo(const seq::leaf& l)const;
	void Quotes2MWave(int period);
	void LoadFile(const std::filesystem::path&);

	void UpdateChildren(const seq::leaf& l);
	void UpdateParents(const seq::leaf& l);

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
};
