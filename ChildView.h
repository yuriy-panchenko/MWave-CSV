
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
	class InsertLeafHelper
	{
		seq::leaf& m_Leaf;
		const seq::chain& m_Patterns;

	public:
		InsertLeafHelper(seq::leaf&, const seq::chain&);
		static UINT CALLBACK Proc(LPVOID);
	};

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
	DECLARE_MESSAGE_MAP()

private:
	void LoadTree();
	void LoadList();
	void Insert(seq::leaf& l, HTREEITEM hParent = TVI_ROOT);
	//static UINT CALLBACK InsertLeaf(LPVOID);

private:
	std::vector<MWAVE> m_MWaves;
	seq::chain m_Patterns;
	seq::leaf m_Tree[32];
	
	CTreeCtrl m_ctrlTree;
	CListCtrl m_ctrlList;
	CImageList m_imgList;
};
