#include "pch.h"
#include "Trader.h"

void Trader::Open(const trd::leaf* pLeaf, const TradePoint& tp)
{
	ASSERT(m_State == Closed);

	m_OpenTrade.isBuy = pLeaf->is_buy();
	m_OpenTrade.Open = tp;
	m_OpenTrade.Chain = pLeaf->chain();
	
	m_State = m_OpenTrade.isBuy ? Long : Short;
}

void Trader::Close(const TradePoint& tp)
{
	if (m_State != Closed)
	{
		m_OpenTrade.Close = tp;
		m_Trades.push_back(std::move(m_OpenTrade));
		m_State = Closed;
	}
}

void Trader::Fill(CListCtrl& list) const
{
	constexpr auto frm{ _T("%.1f") };

	list.SetRedraw(FALSE);

	list.DeleteAllItems();

	while (list.GetHeaderCtrl()->GetItemCount())
		list.DeleteColumn(0);

	int col{ 0 };
	list.InsertColumn(col++, _T("Num"));
	list.InsertColumn(col++, _T("Chain"));
	list.InsertColumn(col++, _T("Buy"));
	list.InsertColumn(col++, _T("Sell"));
	list.InsertColumn(col++, _T("Close"));
	list.InsertColumn(col++, _T("Profit"));

	CString str;
	LVITEM item{};

	for (auto& tr : m_Trades)
	{
		item.mask = LVIF_TEXT;
		item.iSubItem = 0;
		str.Format(_T("%d"), item.iItem + 1);
		item.pszText = (LPTSTR)(LPCTSTR)str;
		list.InsertItem(&item);

		++item.iSubItem;
		str = ToString(tr.Chain);
		item.pszText = (LPTSTR)(LPCTSTR)str;
		list.SetItem(&item);

		++item.iSubItem;
		str.Format(frm, tr.Open.second);
		item.pszText = (LPTSTR)(LPCTSTR)str;
		if (tr.isBuy)
		{
			list.SetItem(&item);
			++item.iSubItem;
		}
		else
		{
			++item.iSubItem;
			list.SetItem(&item);
		}

		++item.iSubItem;
		str.Format(frm, tr.Close.second);
		item.pszText = (LPTSTR)(LPCTSTR)str;
		list.SetItem(&item);

		++item.iSubItem;
		str.Format(frm, tr.Profit());
		item.pszText = (LPTSTR)(LPCTSTR)str;
		list.SetItem(&item);

		++item.iItem;
	}

	for (int i = 0; i < list.GetHeaderCtrl()->GetItemCount(); ++i)
		list.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);

	list.SetRedraw();
}
