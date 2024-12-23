
// ChildView.cpp : implementation of the CChildView class
//

#include "pch.h"
#include "framework.h"
#include "MWave CSV.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef DEBUG
#endif // DEBUG
#define DEBUG_SINGLE_THREAD

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
	ON_NOTIFY(TVN_SELCHANGED, ID_TREE_CTRL, &OnTreeSelChanged)
	//ON_NOTIFY_REFLECT
	ON_COMMAND(ID_INITIAL_STATE, &CChildView::OnInitialState)
	ON_UPDATE_COMMAND_UI(ID_INITIAL_STATE, &CChildView::OnUpdateInitialState)
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
		theApp.BeginWaitCursor();
		m_MWaves.clear();// .RemoveAll();
		m_Patterns.clear();// .RemoveAll();
		const auto filename{ dlg.GetPathName() };

		/*{
			csv::file f;
			if (f.Read(filename.GetString()))
			{
			}
		}*/

		CStringArray items;
		{
			CFile file;
			if (file.Open(filename, CFile::modeRead | CFile::shareDenyNone))
			{
				CArchive ar{ &file,CArchive::load };
				CString str;
				while (file.GetPosition() < file.GetLength())
					if (ar.ReadString(str))
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
			{
				m_Patterns.push_back(to_pattern(mw));
			}
		}

		LoadTree();
		theApp.EndWaitCursor();
	}
}


int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_imgList.Create(16, 16, ILC_MASK, 0, 1))
		return -1;

	if (!m_ctrlTree.Create(WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP
		| TVS_HASBUTTONS | TVS_HASLINES | TVS_SHOWSELALWAYS,
		{}, this, ID_TREE_CTRL))
		return -1;

	if (!m_ctrlList.Create(WS_CHILD | WS_VISIBLE
		| LVS_REPORT | LVS_NOSORTHEADER | LVS_SHAREIMAGELISTS | LVS_SHOWSELALWAYS | LVS_SINGLESEL,
		{}, this, ID_LIST_CTRL))
		return -1;

	icon_id_M_Wave = m_imgList.Add(theApp.LoadIcon(IDI_M_WAVE));
	icon_id_W_Wave = m_imgList.Add(theApp.LoadIcon(IDI_W_WAVE));
	icon_id_Leaf = m_imgList.Add(theApp.LoadIcon(IDI_LEAF2));
	m_ctrlTree.SetImageList(&m_imgList, TVSIL_NORMAL);

	//m_ctrlTree.SetExtendedStyle()
	int col{ 0 };
	m_ctrlList.InsertColumn(col++, _T("Num"), LVCFMT_RIGHT);
	m_ctrlList.InsertColumn(col++, _T("Pattern"), LVCFMT_CENTER);
	m_ctrlList.InsertColumn(col++, _T("Count"), LVCFMT_RIGHT);
	m_ctrlList.InsertColumn(col++, _T("Next"), LVCFMT_RIGHT);
	m_ctrlList.InsertColumn(col++, _T("PProfit"));
	m_ctrlList.InsertColumn(col++, _T("maxDD"));
	m_ctrlList.InsertColumn(col++, _T("Profit"));
	m_ctrlList.InsertColumn(col++, _T("Loss"));
	m_ctrlList.InsertColumn(col++, _T("Diff"));

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

void CChildView::OnTreeSelChanged(NMHDR* pHDR, LRESULT* pResult)
{
	LoadList();
	*pResult = 0;
}

void CChildView::Insert(seq::leaf& l, HTREEITEM hParent)
{
	CString str;
	str.Format(_T("#%d {%s}, count %I64u, leaves %I64u, depth %I64u"),
		l.get_pattern().get_id() + 1,
		l.get_pattern().to_wstring().c_str(),
		l.get_indexes().size(),
		l.get_leaves().size(),
		l.get_max_depth());

	//const auto iid{ l.get_leaves().empty() ? icon_id_Leaf : (l.get_pattern().is_m() ? icon_id_M_Wave : icon_id_W_Wave) };
	const auto iid{ l.get_pattern().is_m() ? icon_id_M_Wave : icon_id_W_Wave };
	auto const h{ m_ctrlTree.InsertItem(str, iid, iid, hParent) };
	l.set_handle(h);

	for (auto pL : l.get_leaves())
		Insert(*pL, h);
}

void CChildView::UpdateTree()
{
	for (char i = 0; i < 32; ++i)
		m_Tree[i] = { mwave::Pattern{ i } };

	const auto thCountMax{ max(1, std::thread::hardware_concurrency()) };

	auto proc = [this](seq::leaf& l)
		{
			l.grow(m_Patterns);
		};
	for (auto& l : m_Tree)
		proc(l);
#ifdef DEBUG_SINGLE_THREAD
#else
	std::vector<std::future<void>> res;

	for (auto& l : m_Tree)
		res.push_back(std::async(std::launch::async, proc, std::ref(l)));

	for (auto& fut : res)
		fut.wait();
#endif // DEBUG_SINGLE_THREAD
}

void CChildView::LoadTree()
{
	m_ctrlTree.SetRedraw(FALSE);
	m_ctrlTree.DeleteAllItems();

	UpdateTree();

	for (auto& pat : m_Tree)
		Insert(pat);
	m_ctrlTree.SetRedraw();

	LoadList();
}

const seq::leaf* CChildView::FindLeaf(HTREEITEM h)const
{
	for (auto& l : m_Tree)
		if (auto pLeaf{ l.find(h) })
			return pLeaf;

	return nullptr;
}

CChildView::MWInfo CChildView::GetInfo(const seq::leaf& l) const
{
	MWInfo ret{};
	double diff;
	const auto ch{ l.get_chain() };

	for (auto ind : l.get_indexes())
	{
		const auto head_index{ ind + ch.size() - 1 };
		auto& mw{ m_MWaves[head_index] };
		ret.PProfit += mw.PProfit;
		ret.maxDD += mw.maxDD;
		diff = mw.next_leg.value - mw.leg.value;
		if (m_Patterns[head_index].is_w())diff = -diff;
		if (diff > .0)
			ret.Profit += diff;
		else ret.Loss += -diff;
	}

	/*auto count{ l.get_indexes().size() };
	ret.PProfit /= count;
	ret.maxDD /= count;
	ret.Profit /= count;*/

	return ret;
}

void CChildView::LoadList()
{
	m_ctrlList.SetRedraw(FALSE);
	m_ctrlList.DeleteAllItems();

	CString str;
	LVITEM item{};

	auto insert_leaf = [&](const seq::leaf& l)
		{
			item.mask = LVIF_PARAM | LVIF_TEXT;
			item.iSubItem = 0;
			item.lParam = (LPARAM)l.get_handle();
			str.Format(_T("%d"), item.iItem + 1);
			item.pszText = (LPTSTR)(LPCTSTR)str;
			m_ctrlList.InsertItem(&item);

			item.mask = LVIF_TEXT;
			++item.iSubItem;
			str.Format(_T("#%d {%s} "), (char)l.get_pattern(), l.get_pattern().to_wstring().c_str());
			item.pszText = (LPTSTR)(LPCTSTR)str;
			m_ctrlList.SetItem(&item);

			++item.iSubItem;
			str.Format(_T("%I64u"), l.get_indexes().size());
			item.pszText = (LPTSTR)(LPCTSTR)str;
			m_ctrlList.SetItem(&item);

			++item.iSubItem;
			str.Format(_T("%I64u"), l.get_leaves().size());
			item.pszText = (LPTSTR)(LPCTSTR)str;
			m_ctrlList.SetItem(&item);

			const auto info{ GetInfo(l) };

			++item.iSubItem;
			str.Format(_T("%.5g"), info.PProfit);
			item.pszText = (LPTSTR)(LPCTSTR)str;
			m_ctrlList.SetItem(&item);

			++item.iSubItem;
			str.Format(_T("%.5g"), info.maxDD);
			item.pszText = (LPTSTR)(LPCTSTR)str;
			m_ctrlList.SetItem(&item);

			++item.iSubItem;
			if (info.Profit == .0)
				str = _T("0");
			else str.Format(_T("%.5g (%.1f%%)"), info.Profit, info.Profit * 100. / (info.Profit + info.Loss));
			item.pszText = (LPTSTR)(LPCTSTR)str;
			m_ctrlList.SetItem(&item);

			++item.iSubItem;
			if (info.Loss == .0)
				str = _T("0");
			else str.Format(_T("%.5g (% .1f%%)"), info.Loss, info.Loss * 100. / (info.Profit + info.Loss));
			item.pszText = (LPTSTR)(LPCTSTR)str;
			m_ctrlList.SetItem(&item);

			++item.iSubItem;
			str.Format(_T("%.5g"), info.Profit - info.Loss);
			item.pszText = (LPTSTR)(LPCTSTR)str;
			m_ctrlList.SetItem(&item);

			++item.iItem;
		};

	auto hItem{ m_ctrlTree.GetSelectedItem() };
	if (auto pLeaf{ FindLeaf(hItem) })
		for (auto pL : pLeaf->get_leaves())
			insert_leaf(*pL);
	else
		for (auto& l : m_Tree)
			insert_leaf(l);

	for (int i = 0; i < m_ctrlList.GetHeaderCtrl()->GetItemCount(); i++)
		m_ctrlList.SetColumnWidth(i, LVSCW_AUTOSIZE);

	m_ctrlList.SetRedraw();
}


void CChildView::OnInitialState()
{
	m_ctrlTree.SelectItem(NULL);
}


void CChildView::OnUpdateInitialState(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_ctrlTree.GetSelectedItem() == NULL);
}
