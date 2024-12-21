
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

//struct PATTERN_INDEX_REC
//{
//	INT_PTR iIndex;
//	mwave::Pattern pat;
//};

// CChildView window

class CChildView : public CWnd
{
	struct MWInfo{double PProfit, maxDD, Profit;};


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
	DECLARE_MESSAGE_MAP()

private:
	void LoadTree();
	void LoadList();
	void Insert(seq::leaf& l, HTREEITEM hParent = TVI_ROOT);
	void UpdateTree();
	const seq::leaf* FindLeaf(HTREEITEM)const;
	MWInfo GetInfo(const seq::leaf& l)const;

private:
	std::vector<MWAVE> m_MWaves;
	seq::chain m_Patterns;
	seq::leaf m_Tree[32];

	CTreeCtrl m_ctrlTree;
	CListCtrl m_ctrlList;
	CImageList m_imgList;
};
