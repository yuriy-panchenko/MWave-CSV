
// ChildView.cpp : implementation of the CChildView class
//

#include "pch.h"
#include "framework.h"
#include "MWave CSV.h"
#include "ChildView.h"
#include "CSRevParamDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef DEBUG
#endif // DEBUG
#define DEBUG_SINGLE_THREAD

#define SECTION_SETTINGS	_T("Settings")
#define ENTRY_PERIOD			_T("Period")

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

	PostMessage(WM_COMMAND, ID_FILE_OPEN);

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

void CChildView::Insert(seq::leaf& l, HTREEITEM hParent)
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

	//	ASSERT(ret.Profit <= ret.PProfit);
	//	ASSERT(ret.Loss <= ret.maxDD);

	return ret;
}

void CChildView::Quotes2MWave(const int period)
{
	const auto rec_count{ m_Quotes.size() };
	const auto empty_value{ .0 };

	mwave::SReversal sRev{ period };
	sRev.Init(rec_count, empty_value);

	std::vector<double> open(rec_count), high(rec_count), low(rec_count);
	auto itOpen{ open.begin() }, itHigh{ high.begin() }, itLow{ low.begin() };

	for (const auto& rec : m_Quotes)
		*itOpen++ = rec.open,
		*itHigh++ = rec.high,
		*itLow++ = rec.low;

	sRev.Apply(open, high, low);

	MWAVE mw;
	mw.leg.index = mw.next_leg.index = -1;
	mw.PProfit = mw.maxDD = .0;

	auto& bufs{ sRev.GetBuffers() };

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

		for (int u = i - 1; u > -1; --u)
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
					ret.open = std::stod(std::wstring{ line[2].begin(), line[2].end() });
					ret.high = std::stod(std::wstring{ line[3].begin(), line[3].end() });
					ret.low = std::stod(std::wstring{ line[4].begin(), line[4].end() });
					ret.close = std::stod(std::wstring{ line[5].begin(), line[5].end() });
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
			m_ctrlList.InsertItem(&item);

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
