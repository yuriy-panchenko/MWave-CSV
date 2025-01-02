
// ChildView.cpp : implementation of the CChildView class
//

#include "pch.h"
#include "framework.h"
#include "MWave CSV.h"
#include "ChildView.h"
#include "CSRevParamDlg.h"
#include "CReportListDlg.h"
#include "CChartDlg.h"
#include "fwd_leaf.h"
#include "CTRadeSettingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef DEBUG
#endif // DEBUG
#define DEBUG_SINGLE_THREAD

#define SECTION_SETTINGS	_T("Settings")
#define ENTRY_PERIOD			_T("Period")
#define PROJECT_EXT			_T("mwp")
#define PROJECT_FILE_FILTER	_T("MWave Projects Files (*.mwp)|*.mwp||")

#define WM_ITEM_CHECKED	(WM_USER+0x0001)

// CChildView
int icon_id_M_Wave, icon_id_W_Wave, icon_id_Leaf;

CChildView::CChildView()
{
}

CChildView::~CChildView()
{
	for (INT_PTR i = 0; i < m_Reports.GetSize(); i++)
		delete m_Reports[i];

	for (INT_PTR i = 0; i < m_Charts.GetSize(); i++)
		delete m_Charts[i];
}

BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_COMMAND(ID_FILE_OPEN, &CChildView::OnFileOpen)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_NOTIFY(TVN_SELCHANGED, ID_TREE_CTRL, &OnTreeSelChanged)
	ON_NOTIFY(NM_CLICK, ID_TREE_CTRL, &OnTreeMouseClick)
	ON_COMMAND(ID_INITIAL_STATE, &CChildView::OnInitialState)
	ON_UPDATE_COMMAND_UI(ID_INITIAL_STATE, &CChildView::OnUpdateInitialState)
	ON_MESSAGE(WM_ITEM_CHECKED, &OnTreeItemChecked)
	ON_NOTIFY(NM_CLICK, ID_LIST_CTRL, &OnListItemClicked)
	//ON_NOTIFY(NM_DBLCLK, ID_LIST_CTRL, &OnListItemDblClicked)
	ON_NOTIFY(NM_DBLCLK, ID_LIST_CTRL, &OnListItemClicked)
	//NM_CLICK
	ON_COMMAND(ID_PROJECT_LOAD, &CChildView::OnProjectLoad)
	ON_COMMAND(ID_PROJECT_SAVE, &CChildView::OnProjectSave)
	ON_COMMAND(ID_TRADE, &CChildView::OnTrade)
	ON_UPDATE_COMMAND_UI(ID_TRADE, &CChildView::OnUpdateTrade)
	ON_COMMAND(ID_TRADE2, &CChildView::OnTrade2)
	ON_UPDATE_COMMAND_UI(ID_TRADE2, &CChildView::OnUpdateTrade2)
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

	if (!m_rInfo.IsRectEmpty())
	{
		auto rect{ m_rInfo };
		CBitmap bmp;
		bmp.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());

		CDC memDC;
		memDC.CreateCompatibleDC(&dc);
		auto oldBmp{ memDC.SelectObject(bmp) };

		dc.BitBlt(m_rInfo.left, m_rInfo.top, m_rInfo.Width(), m_rInfo.Height(), &memDC, 0, 0, SRCCOPY);

		memDC.SelectObject(oldBmp);
	}
}

void CChildView::OnFileOpen()
{
	CFileDialog dlg{ TRUE,_T(".csv"),NULL,6UL,_T("MWave data files (*.csv)|*.csv||"),this };
	if (dlg.DoModal() == IDOK)
		LoadFile(dlg.GetPathName().GetString());
}


int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_imgList.Create(16, 16, ILC_MASK, 0, 1))
		return -1;

	if (!m_ctrlTree.Create(WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP
		| TVS_HASBUTTONS | TVS_HASLINES | TVS_SHOWSELALWAYS | TVS_LINESATROOT /*| TVS_CHECKBOXES*/,
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

	m_ctrlTree.SetExtendedStyle(TVS_EX_PARTIALCHECKBOXES | TVS_EX_DOUBLEBUFFER, TVS_EX_PARTIALCHECKBOXES | TVS_EX_DOUBLEBUFFER);
	m_ctrlList.SetExtendedStyle(LVS_EX_AUTOSIZECOLUMNS | LVS_EX_BORDERSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES);

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
	m_ctrlList.InsertColumn(col++, _T("AvDiff"));

	//PostMessage(WM_COMMAND, ID_FILE_OPEN);

	return 0;
}

BOOL CChildView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	CRect rect;
	GetClientRect(rect);

	m_rInfo = rect;
	m_rInfo.bottom = rect.top + /*25*/0;

	const int x{ rect.Width() / 4 };

	auto r{ rect };
	r.top = m_rInfo.bottom;
	r.right = x;
	m_ctrlTree.MoveWindow(r);

	r = rect;
	r.top = m_rInfo.bottom;
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

void CChildView::UpdateChildren(const seq::leaf& l)
{
	const int iIcon{ (int)l.get_icon_state() };
	TreeView_SetItemState(m_ctrlTree, l.get_handle(), INDEXTOSTATEIMAGEMASK(iIcon), TVIS_STATEIMAGEMASK);

	for (auto& p : l.get_leaves())
	{
		p->select(l.is_selected());
		UpdateChildren(*p);
	}
}


void CChildView::UpdateParents(seq::leaf& l)
{
	if (auto pParent{ l.parent() })
	{
		ASSERT(!pParent->get_leaves().empty());
		pParent->select_by_children();

		const int iIcon{ (int)pParent->get_icon_state() };
		TreeView_SetItemState(m_ctrlTree, pParent->get_handle(), INDEXTOSTATEIMAGEMASK(iIcon), TVIS_STATEIMAGEMASK);
		UpdateParents(*pParent);
	}
}

std::unique_ptr< trd::leaf> CChildView::Clone(const seq::leaf& l, trd::leaf* parent) const
{
	auto ret{ std::make_unique<trd::leaf>(l, GetInfo(l), parent) };

	for (auto p : l.get_leaves())
		ret->add(Clone(*p, ret.get()).release());

	return ret;
}

void CChildView::OnTreeMouseClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	TVHITTESTINFO tv{};

	const auto mPos{ GetMessagePos() };
	CPoint pnt{ GET_X_LPARAM(mPos), GET_Y_LPARAM(mPos) };

	m_ctrlTree.ScreenToClient(&pnt);

	tv.pt.x = pnt.x;
	tv.pt.y = pnt.y;

	auto hTree = m_ctrlTree.HitTest(&tv);

	if (tv.flags & TVHT_ONITEMSTATEICON)
		PostMessage(WM_ITEM_CHECKED, (WPARAM)hTree);

	*pResult = 0;
}

void CChildView::Insert(seq::leaf& l, HTREEITEM hItem)
{
	CString str;
	str.Format(_T("%d {%s}, count %I64u, leaves %I64u, depth %I64u"),
		l.get_pattern().get_id() + 1,
		l.get_pattern().to_wstring().c_str(),
		l.get_indexes().size(),
		l.get_leaves().size(),
		l.get_max_depth());

	//const auto iid{ l.get_leaves().empty() ? icon_id_Leaf : (l.get_pattern().is_m() ? icon_id_M_Wave : icon_id_W_Wave) };
	const auto iid{ l.get_pattern().is_m() ? icon_id_M_Wave : icon_id_W_Wave };
	hItem = m_ctrlTree.InsertItem(str, iid, iid, hItem);
	l.set_handle(hItem);

	const int iIcon{ (int)l.get_icon_state() };
	TreeView_SetItemState(m_ctrlTree, hItem, INDEXTOSTATEIMAGEMASK(iIcon), TVIS_STATEIMAGEMASK);

	for (auto pL : l.get_leaves())
		Insert(*pL, hItem);
}

void CChildView::TreeFromPatterns()
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

seq::leaf* CChildView::FindLeaf(HTREEITEM h)
{
	for (auto& l : m_Tree)
		if (auto pLeaf{ l.find(h) })
			return pLeaf;

	return nullptr;
}

MWINFO CChildView::GetInfo(const seq::leaf& l) const
{
	MWINFO ret{};
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

	//	ASSERT(ret.Profit <= ret.PProfit);
	//	ASSERT(ret.Loss <= ret.maxDD);

	return ret;
}

void CChildView::Quotes2MWave(const int period)
{
	const auto rec_count{ m_Quotes.size() };
	const auto empty_value{ .0 };

	m_SRev = period;
	m_SRev.Init(rec_count, empty_value);

	std::vector<double> Open(rec_count), high(rec_count), low(rec_count);
	auto itOpen{ Open.begin() }, itHigh{ high.begin() }, itLow{ low.begin() };

	for (const auto& rec : m_Quotes)
		*itOpen++ = rec.Open,
		*itHigh++ = rec.high,
		*itLow++ = rec.low;

	m_SRev.Apply(Open, high, low);

	MWAVE mw;
	mw.leg.index = mw.next_leg.index = -1;
	mw.PProfit = mw.maxDD = .0;

	auto& bufs{ m_SRev.GetBuffers() };

	for (int i = 0; i < (int)bufs.size(); ++i)
	{
		auto& item{ bufs[i] };
		if (item.Leg == empty_value)
			continue;

		mw.leg = mw.next_leg;

		mw.next_leg.index = i;
		mw.next_leg.value = item.Leg;

		if (mw.leg.index == -1)
			continue;

		int iPat{ 4 };

		for (int u = mw.leg.index - 1; u > -1; --u)
			if (bufs[u].Peaks != empty_value)
			{
				mw.mw[iPat].index = u;
				mw.mw[iPat].value = bufs[u].Peaks;
				if (--iPat < 0)
				{
					m_MWaves.push_back(mw);
					break;
				}
			}
	}
}

void CChildView::LoadFile(const std::filesystem::path& filename)
{
	theApp.BeginWaitCursor();

	auto to_pattern = [](const MWAVE& mw)->mwave::Pattern
		{
			double prices[5];

			for (size_t i = 0; i < 5; ++i)
				prices[i] = mw.mw[i].value;

			return mwave::Pattern::FromPrices(prices);
		};

	csv::file f;
	if (f.Read(filename))
	{
		m_Filename = filename.wstring().c_str();
		theApp.GetMainWnd()->SetWindowText(CString{ theApp.m_pszAppName } + _T(" - ") + filename.filename().c_str());
		auto& lines{ f.GetLines() };

		m_ctrlTree.DeleteAllItems();
		m_ctrlList.DeleteAllItems();
		m_MWaves.clear();// .RemoveAll();
		m_Patterns.clear();// .RemoveAll();
		m_Quotes.clear();
		m_MWaves.reserve(lines.size());
		m_Patterns.reserve(lines.size());

		if (f.GetColumns().size() == 16)
		{
			auto line_to_mwave = [](const auto& line)->MWAVE
				{
					MWAVE ret;
					auto ind{ 0 };
					auto iter{ line.begin() };

					auto next_value = [&iter]()
						{
							std::wstring ret{ iter->begin(), iter->end() };
							++iter;
							return ret;
						};

					for (INT_PTR i = 0; i < 5; i++)
					{
						ret.mw[i].index = _ttoi(next_value().c_str());
						ret.mw[i].value = _ttof(next_value().c_str());
					}

					ret.leg.index = _ttoi(next_value().c_str());
					ret.leg.value = _ttof(next_value().c_str());
					ret.next_leg.index = _ttoi(next_value().c_str());
					ret.next_leg.value = _ttof(next_value().c_str());
					ret.PProfit = _ttof(next_value().c_str());
					ret.maxDD = _ttof(next_value().c_str());

					return ret;
				};

			for (auto& l : lines)
				if (l.size() == 16)
					m_MWaves.push_back(line_to_mwave(l));
		}
		else if (f.GetColumns().size() == 9)
		{
			auto make_time = [](const std::wstring_view& date, const std::wstring_view& time)->CTime
				{
					const auto itDate{ date.begin() }, itTime{ time.begin() };
					const int
						year{ std::stoi(std::wstring{ itDate, itDate + 4 }) },
						month{ std::stoi(std::wstring{ itDate + 5, itDate + 7 }) },
						day{ std::stoi(std::wstring{ itDate + 8, itDate + 10 }) },
						hour{ std::stoi(std::wstring{ itTime, itTime + 2 }) },
						min{ std::stoi(std::wstring{ itTime + 3, itTime + 5 }) },
						sec{ std::stoi(std::wstring{ itTime + 6, itTime + 8 }) };
					return { year, month, day, hour, min, sec };
				};

			auto line_to_quote = [make_time](const auto& line)->QUOTE_REC
				{
					QUOTE_REC ret;
					ret.time = make_time(line[0], line[1]);
					ret.Open = std::stod(std::wstring{ line[2].begin(), line[2].end() });
					ret.high = std::stod(std::wstring{ line[3].begin(), line[3].end() });
					ret.low = std::stod(std::wstring{ line[4].begin(), line[4].end() });
					ret.Close = std::stod(std::wstring{ line[5].begin(), line[5].end() });
					ret.volTick = std::stoi(std::wstring{ line[6].begin(), line[6].end() });
					ret.volume = std::stoi(std::wstring{ line[7].begin(), line[7].end() });
					ret.spread = std::stoi(std::wstring{ line[8].begin(), line[8].end() });
					return ret;
				};
			CSRevParamDlg dlg{ (int)theApp.GetProfileInt(SECTION_SETTINGS, ENTRY_PERIOD, 7), this };
			theApp.EndWaitCursor();
			if (dlg.DoModal() == IDOK)
			{
				m_Quotes.reserve(lines.size());

				for (auto& l : lines)
					if (l.size() == 9)
						m_Quotes.push_back(line_to_quote(l));

				Quotes2MWave(dlg.Period);
				theApp.WriteProfileInt(SECTION_SETTINGS, ENTRY_PERIOD, dlg.Period);
			}
			theApp.BeginWaitCursor();
		}

		for (const auto& mw : m_MWaves)
			m_Patterns.push_back(to_pattern(mw));

		m_MWaves.shrink_to_fit();
		m_Patterns.shrink_to_fit();
	}

	TreeFromPatterns();

	LoadTree();

	theApp.EndWaitCursor();
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
			const auto ind{ m_ctrlList.InsertItem(&item) };
			m_ctrlList.SetCheck(ind, l.is_selected());

			item.mask = LVIF_TEXT;
			++item.iSubItem;
			str.Format(_T("%d {%s} "), (char)l.get_pattern() + 1, l.get_pattern().to_wstring().c_str());
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
			else str.Format(_T("%.5g"), info.Loss/*, info.Loss * 100. / (info.Profit + info.Loss)*/);
			item.pszText = (LPTSTR)(LPCTSTR)str;
			m_ctrlList.SetItem(&item);

			++item.iSubItem;
			str.Format(_T("%.5g"), info.Profit - info.Loss);
			item.pszText = (LPTSTR)(LPCTSTR)str;
			m_ctrlList.SetItem(&item);

			++item.iSubItem;
			str.Format(_T("%.5g"), (info.Profit - info.Loss) / l.get_indexes().size());
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

LRESULT CChildView::OnTreeItemChecked(WPARAM wParam, LPARAM)
{
	auto hItem{ HTREEITEM(wParam) };

	if (auto pLeaf{ FindLeaf(hItem) })
	{
		pLeaf->select(!pLeaf->is_selected());

		UpdateChildren(*pLeaf);
		UpdateParents(*pLeaf);

		if (hItem == m_ctrlTree.GetSelectedItem())
			LoadList();
		else m_ctrlTree.SelectItem(hItem);
	}

	return LRESULT(0);
}

void CChildView::OnListItemClicked(NMHDR* pHDR, LRESULT* pResult)
{
	const auto msgPos{ GetMessagePos() };
	CPoint pnt{ GET_X_LPARAM(msgPos) ,GET_Y_LPARAM(msgPos) };
	m_ctrlList.ScreenToClient(&pnt);

	UINT flags;
	const auto iItem{ m_ctrlList.HitTest(pnt,&flags) };

	if (iItem > -1 && flags & LVHT_ONITEMSTATEICON)
	{
		const auto hItem{ (HTREEITEM)m_ctrlList.GetItemData(iItem) };
		if (auto pLeaf{ FindLeaf(hItem) })
		{
			pLeaf->select(!m_ctrlList.GetCheck(iItem));
			UpdateChildren(*pLeaf);
			UpdateParents(*pLeaf);


			//static int gClick{ 0 };
			//CString str;
			//str.Format(_T("CLick #%d"), ++gClick);
			//OutputDebugString(str);
		}
	}

	*pResult = 0;
}

void CChildView::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{	// storing code

		ar << m_Filename << m_MWaves << m_Patterns << m_Quotes;

		for (auto& l : m_Tree)
			l.Serialize(ar);

		std::stringbuf buf;
		std::ostream os{ &buf };
		os << m_SRev;

		const auto str{ buf.str() };
		const auto bufSize{ (UINT)str.length() };
		ar.Write(&bufSize, sizeof bufSize);
		ar.Write(str.data(), bufSize);
	}
	else
	{	// loading code
		ar >> m_Filename >> m_MWaves >> m_Patterns >> m_Quotes;

		for (auto& l : m_Tree)
			l.Serialize(ar);

		UINT bufSize;
		ar.Read(&bufSize, sizeof bufSize);

		std::string str;
		str.resize(bufSize);
		ar.Read(str.data(), bufSize);

		std::stringbuf buf{ str };
		std::istream os{ &buf };
		os >> m_SRev;
	}
}


void CChildView::OnProjectLoad()
{
	CFile file;
	CFileDialog dlg{ TRUE, PROJECT_EXT, NULL, 6UL, PROJECT_FILE_FILTER, this };
	if (dlg.DoModal() == IDOK)
		if (file.Open(dlg.GetPathName(), CFile::modeRead | CFile::typeBinary | CFile::shareDenyWrite))
		{
			CArchive ar{ &file,CArchive::load };
			TRY
			{
				Serialize(ar);
				m_ProjectFilename = dlg.GetPathName();
				theApp.GetMainWnd()->SetWindowText(CString{ theApp.m_pszAppName } + _T(" - ") + std::filesystem::path{m_ProjectFilename.GetString()}.filename().c_str());
			}
				CATCH(CException, e)
			{
				e->ReportError();
			}
			END_CATCH

				LoadTree();
		}
}


void CChildView::OnProjectSave()
{
	CFile file;
	CFileException exc;

	CString fname;
	if (!m_ProjectFilename.IsEmpty())
		fname = std::filesystem::path{ m_ProjectFilename.GetString() }.filename().wstring().c_str();

	CFileDialog dlg{ FALSE, PROJECT_EXT, fname, 6UL, PROJECT_FILE_FILTER, this };
	if (dlg.DoModal() == IDOK)
		if (file.Open(dlg.GetPathName(), CFile::modeCreate | CFile::modeWrite | CFile::typeBinary | CFile::shareDenyRead | CFile::shareDenyWrite, &exc))
		{
			CArchive ar{ &file,CArchive::store };
			TRY
			{
				Serialize(ar);
				m_ProjectFilename = dlg.GetPathName();
			}
				CATCH(CException, e)
			{
				e->ReportError();
			}
			END_CATCH
		}
		else exc.ReportError();
}

seq::leaf make_tradable(const seq::leaf& l)
{
	seq::leaf ret{ l.get_pattern() };
	auto& lvs{ ret.get_leaves() };

	for (auto p : l.get_leaves())
		if (p->is_selected())
		{
			lvs.push_back(new seq::leaf{ p->get_pattern(), &ret });
		}

	return ret;
}

//TradableTree CChildView::CollectTradableTree()const
//{
//	TradableTree ret;
//
//	for (auto& l : m_Tree)
//		if (l.is_selected())
//		{
//			MWaveTrade trd{ make_tradable(l) };
//			trd.i = GetInfo(l);
//			ret.push_back(std::move(trd));
//		}
//
//	return ret;
//}

Trader::TradePoint CChildView::ToTimePrice(const PNT& pnt)const
{
	if (m_Quotes.empty())
		return std::make_pair(pnt.index, pnt.value);
	else
		return std::make_pair(m_Quotes[pnt.index].time, pnt.value);
}

void CChildView::OnTrade()
{
	//ASSERT(!m_Quotes.empty());
	ASSERT(m_Patterns.size() == m_MWaves.size());

	Trader trader;
	trd::tree tree;

	for (auto& l : m_Tree)
		tree.set(Clone(l));

	const auto rEnd{ m_Patterns.crend() };
	auto itMWave{ m_MWaves.begin() };

	for (auto iter{ m_Patterns.begin() }; iter != m_Patterns.end(); ++iter, ++itMWave)
	{
		trader.Close(ToTimePrice(itMWave->leg));
		if (auto pLeaf{ tree.is_tradable(std::make_reverse_iterator(iter + 1), rEnd) })
			trader.Open(pLeaf, ToTimePrice(itMWave->leg));
	}

	ShowReportDlg(trader);
	ShowCumulativeChart(trader);
}

void CChildView::ShowReportDlg(const Trader& trader)
{
	auto pDlg{ new CReportListDlg };
	if (pDlg->Create(IDD_REPORT_LIST_DLG, this))
	{
		trader.Fill(pDlg->m_List);
		pDlg->ShowWindow(SW_SHOW);
		m_Reports.Add(pDlg);
	}
	else delete pDlg;
}

void CChildView::ShowCumulativeChart(const Trader& trader)
{
	auto get_total_data = [trader]()->CChartDlg::chart_ptr
		{
			double total{ .0 };
			std::vector<double> arr(trader.GetTrades().size());
			auto iter{ arr.begin() };

			for (auto& tr : trader.GetTrades())
			{
				total += tr.Profit();
				*iter++ = total;
			}

			return std::make_unique<chart::line>(std::move(arr));
		};

	auto pDlg{ new CChartDlg{get_total_data()} };
	if (pDlg->Create(IDD_CHART_DLG, this))
	{
		pDlg->ShowWindow(SW_SHOW);
		m_Charts.Add(pDlg);
	}
	else delete pDlg;
}

void CChildView::OnUpdateTrade(CCmdUI* pCmdUI)
{
	//pCmdUI->Enable(!m_Quotes.empty());
}

const fwd::leaf* ConfirmIsTradable(const fwd::leaf* pL, const seq::chain::const_reverse_iterator iter, const double minProfit)
{
	if (/*abs*/(pL->get_info().Net()/pL->get_info().TradeCount()) >= minProfit
		//&& pL->get_info().iWin < pL->get_info().iLose
		)
		return pL;

	if (auto pChild{ pL->find_child(*(iter + 1)) })
		return ConfirmIsTradable(pChild, iter + 1, minProfit);

	return nullptr;
}

void CChildView::OnTrade2()
{
	CTradeSettingDlg dlg;
	dlg.m_MinProfit = 2.;
	if (dlg.DoModal() != IDOK)
		return;

	Trader trader;
	fwd::tree tree;

	const auto rPatEnd{ m_Patterns.crend() };
	auto itMWave{ m_MWaves.begin() };

	for (auto itPat{ m_Patterns.begin() }; itPat != m_Patterns.end(); ++itPat, ++itMWave)
	{
		if (itPat->get_id() < 0 || itPat->get_id() > 31)
			continue;

		auto rPatFrom{ std::make_reverse_iterator(itPat + 1) };

		trader.Close(ToTimePrice(itMWave->leg));

		if (auto pLeaf{ tree.is_tradable(rPatFrom, rPatEnd) })
			if (pLeaf = ConfirmIsTradable(&pLeaf->head(), rPatFrom, dlg.m_MinProfit))
				trader.Open(pLeaf->is_buy(), pLeaf->get_chain(), ToTimePrice(itMWave->leg));

		auto pLeaf{ tree.add(rPatFrom, rPatEnd, { *itPat, *itMWave }) };
	}

	ShowReportDlg(trader);
	ShowCumulativeChart(trader);
}


void CChildView::OnUpdateTrade2(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_Quotes.empty());
}
