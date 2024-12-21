
// ChildView.cpp : implementation of the CChildView class
//

#include "pch.h"
#include "framework.h"
#include "MWave CSV.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CChildView
int icon_id_M_Wave, icon_id_W_Wave, icon_id_Leaf;

CChildView::CChildView()
{
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_COMMAND(ID_FILE_OPEN, &CChildView::OnFileOpen)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
		::LoadCursor(nullptr, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1), nullptr);

	return TRUE;
}

void CChildView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// TODO: Add your message handler code here

	// Do not call CWnd::OnPaint() for painting messages
}

void CChildView::OnFileOpen()
{
	CFileDialog dlg{ TRUE,_T(".csv"),NULL,6UL,_T("MWave data files (*.csv)|*.csv||"),this };
	if (dlg.DoModal() == IDOK)
	{
		m_MWaves.clear();// .RemoveAll();
		m_Patterns.clear();// .RemoveAll();
		const auto filename{ dlg.GetPathName() };
		CStringArray items;
		{
			CFile file;
			if (file.Open(filename, CFile::modeRead | CFile::shareDenyNone))
			{
				CArchive ar{ &file,CArchive::load };
				CString str;
				while (ar.ReadString(str))
					items.Add(str);
			}
		}

		CStringArray item;

		auto tokanize = [&item](const CString& s)->void
			{
				item.RemoveAll();
				int pos{ 0 };
				while (true)
				{
					auto res{ s.Tokenize(_T("\t"), pos) };
					if (pos > -1)
						item.Add(res);
					else break;
				}
			};

		auto items_to_mwave = [&item]()->MWAVE
			{
				MWAVE ret;
				auto ind{ 0 };

				for (INT_PTR i = 0; i < 5; i++)
				{
					ret.mw[i].index = _ttoi(item[ind++]);
					ret.mw[i].value = _ttof(item[ind++]);
				}
				ret.leg.index = _ttoi(item[ind++]);
				ret.leg.value = _ttof(item[ind++]);
				ret.next_leg.index = _ttoi(item[ind++]);
				ret.next_leg.value = _ttof(item[ind++]);
				ret.PProfit = _ttof(item[ind++]);
				ret.maxDD = _ttof(item[ind++]);

				return ret;
			};

		auto to_pattern = [](const MWAVE& mw)->mwave::Pattern
			{
				double prices[5];

				for (size_t i = 0; i < 5; ++i)
					prices[i] = mw.mw[i].value;

				return mwave::Pattern::FromPrices(prices);
			};

		if (items.IsEmpty())
			MessageBox(_T("No MWaves:("));
		else
		{
			tokanize(items[0]);
			const auto column_count{ item.GetSize() };
			if (column_count != 16)
			{
				MessageBox(_T("Wrong file format!"));
				return;
			}

			m_MWaves.reserve(items.GetSize());
			m_Patterns.reserve(items.GetSize());
			m_MWaves.push_back(items_to_mwave());

			for (INT_PTR i = 1; i < items.GetSize(); ++i)
			{
				tokanize(items[i]);
				if (item.GetSize() == column_count)
					m_MWaves.push_back(items_to_mwave());
			}

			for (const auto& mw : m_MWaves)
				m_Patterns.push_back(to_pattern(mw));
		}

		LoadTree();
	}
}


int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_imgList.Create(16, 16, ILC_MASK, 0, 1))
		return -1;

	if (!m_ctrlTree.Create(WS_VISIBLE | WS_CHILD | WS_BORDER | TVS_HASBUTTONS | TVS_HASLINES | TVS_SHOWSELALWAYS,
		{}, this, ID_TREE_CTRL))
		return -1;

	if (!m_ctrlList.Create(WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_NOSORTHEADER | LVS_SHAREIMAGELISTS | LVS_SHOWSELALWAYS | LVS_SINGLESEL,
		{}, this, ID_LIST_CTRL))
		return -1;

	icon_id_M_Wave = m_imgList.Add(theApp.LoadIcon(IDI_M_WAVE));
	icon_id_W_Wave = m_imgList.Add(theApp.LoadIcon(IDI_W_WAVE));
	icon_id_Leaf = m_imgList.Add(theApp.LoadIcon(IDI_LEAF2));
	//m_ctrlTree.SetExtendedStyle()
	//m_ctrlList.InsertColumn(col++,_T("Num"))
	m_ctrlTree.SetImageList(&m_imgList, TVSIL_NORMAL);

	LoadTree();

	return 0;
}

BOOL CChildView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	CRect rect;
	GetClientRect(rect);
	const int x{ rect.Width() / 4 };

	auto r{ rect };
	r.right = x;
	m_ctrlTree.MoveWindow(r);

	r = rect;
	r.left = x;
	m_ctrlList.MoveWindow(r);

	//return CWnd::OnEraseBkgnd(pDC);
	return TRUE;
}

void CChildView::Insert(seq::leaf& l, HTREEITEM hParent)
{
	CString str;
	str.Format(_T("%s, count %I64u, leaves %I64u, depth %I64u"),
		l.get_pattern().to_wstring().c_str(),
		l.get_indexes().size(),
		l.get_leaves().size(),
		l.get_max_depth());

	//const auto iid{ l.get_leaves().empty() ? icon_id_Leaf : (l.get_pattern().is_m() ? icon_id_M_Wave : icon_id_W_Wave) };
	const auto iid{ l.get_pattern().is_m() ? icon_id_M_Wave : icon_id_W_Wave };
	auto const h{ m_ctrlTree.InsertItem(str, iid, iid, hParent) };
	l.set_handle(h);

	for (auto& l2 : l.get_leaves())
		Insert(l2, h);
}

//UINT CChildView::InsertLeaf(LPVOID pVoid)
//{
//	const mwave::Pattern pat{ char(pVoid) };
//
//	theApp.Get
//
//	return pat.get_id();
//}

//void f(char) {}

void CChildView::LoadTree()
{
	/*auto find_indexes = [this](mwave::Pattern pat)
		{
			std::vector<size_t> ret;

			for (size_t i = 0; i < m_Patterns.size(); ++i)
				if (m_Patterns[i] == pat)
					ret.push_back(i);

			return ret;
		};*/

	m_ctrlTree.DeleteAllItems();

	for (char i = 0; i < 32; ++i)
		m_Tree[i] = { mwave::Pattern{ i } };


	/* {
		std::function<void(char)> fun{ f };
		auto fut = std::async(std::launch::async, fun, char(1));

		//bee::hive<void(const char)> hive{ proc };
		bee::hive<void(const char)> hive{ f };
		for (char i = 0; i < 32; ++i)
			hive.launch(tree[i]);

	}*/


	//auto proc = [this](const char pat)
	//	{
	//		m_Tree[pat].grow(m_Patterns);
	//	};

	{
		const auto thCountMax{ max(1, std::thread::hardware_concurrency()) };

		auto add_thread = [this](seq::leaf& l)->HANDLE
			{
				return *::AfxBeginThread(&CChildView::InsertLeafHelper::Proc, new InsertLeafHelper{ l, m_Patterns });
			};

		std::vector<HANDLE> Ths;

		for (char i = 0; i < 32; ++i)
			if (Ths.size() == thCountMax)
			{
				const auto wait_obj_id{ ::WaitForMultipleObjects(thCountMax, Ths.data(), FALSE, INFINITE) - WAIT_OBJECT_0 };
				if (wait_obj_id < thCountMax)
					Ths[wait_obj_id] = add_thread(m_Tree[i]);
			}
			else
				Ths.push_back(add_thread(m_Tree[i]));

		::WaitForMultipleObjects(thCountMax, Ths.data(), TRUE, INFINITE);
	}

	/* {
		std::thread thrs[32];

		for (char i = 0; i < 32; ++i)
			thrs[i] = std::thread{ proc, i };

		for (auto& th : thrs)
			th.join();
	}*/

	for (auto& pat : m_Tree)
		Insert(pat);

	LoadList();
}

void CChildView::LoadList()
{
	m_ctrlList.DeleteAllItems();
}

CChildView::InsertLeafHelper::InsertLeafHelper(seq::leaf& l, const seq::chain& pts)
	:m_Leaf{ l }
	, m_Patterns{ pts }
{
}

UINT CChildView::InsertLeafHelper::Proc(LPVOID pData)
{
	auto pHlp{ (InsertLeafHelper*)pData };

	pHlp->m_Leaf.grow(pHlp->m_Patterns);

	delete pHlp;

	return 0;
}
