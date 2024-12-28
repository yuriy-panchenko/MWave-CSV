#pragma once
#include "trd_leaf.h"

class Trader
{
public:
	using TradePoint = std::pair<CTime, double>;

	struct TRADE_REC
	{
		BOOL isBuy;
		TradePoint Open, Close;
		seq::chain Chain;

		double Profit()const { return isBuy ? Close.second - Open.second : Open.second - Close.second; }
	};

public:

	void Open(const trd::leaf*, const TradePoint&);
	void Open(BOOL isBuy, seq::chain ch, const TradePoint&);
	void Close(const TradePoint&);
	void Fill(CListCtrl&)const;

private:
	enum Dir { Closed, Long, Short, } m_State;

	std::vector<TRADE_REC> m_Trades;
	TRADE_REC m_OpenTrade;
};
