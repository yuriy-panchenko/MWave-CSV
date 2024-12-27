#include "pch.h"
#include "trd_leaf.h"

namespace trd
{
	//////////////////////////////////////////////////////////////////////////////////////////
	leaf::leaf()
		:pat{}
		, pPrev{ nullptr }
		, info{}
	{
	}

	leaf::leaf(const seq::leaf& l, const MWInfo& i, const leaf* parent)
		:pat{ l.get_pattern() }
		, pPrev{ parent }
		, isSelected{ l.is_selected() }
		, info{ i }
	{
	}

	leaf::~leaf()
	{
		for (auto p : leaves)
			delete p;
	}

	mwave::Pattern leaf::id() const
	{
		return pat;
	}

	bool leaf::is_valid() const
	{
		return pat.get_id() > -1 && pat.get_id() < 32;
	}

	bool leaf::is_buy() const
	{
		ASSERT(is_valid());

		return diff() >= .0 ? pat.is_m() : pat.is_w();
	}

	double leaf::diff() const
	{
		return info.Profit - info.Loss;
	}

	void leaf::add(leaf* p)
	{
		leaves.push_back(p);
	}

	const leaf* leaf::find_child(mwave::Pattern pattern) const
	{
		for (auto p : leaves)
			if (p->id() == pattern)
				return p;

		return nullptr;
	}

	const leaf* leaf::is_tradable(const seq::chain::const_reverse_iterator itFrom, const seq::chain::const_reverse_iterator itTo) const
	{
		if (itFrom == itTo)
			return this;
		
		ASSERT(*itFrom == id());

		if (!isSelected)
			return nullptr;

		if (leaves.empty())
			return this;

		auto nxt{ itFrom + 1 };

		if (auto pLeaf{ find_child(*nxt) })
			return pLeaf->is_tradable(nxt, itTo);

		return this;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	void tree::set(trd::leaf&& l)
	{
		assert(l.is_valid());
		m_Root[(char)l.id()] = std::move(l);
	}

	const leaf* tree::is_tradable(const seq::chain::const_reverse_iterator itFrom, const seq::chain::const_reverse_iterator itTo) const
	{
		ASSERT(itFrom != itTo);

		if (0 > (char)*itFrom || 31 < (char)*itFrom)
			return nullptr;

		return m_Root[(char)*itFrom].is_tradable(itFrom, itTo);
	}
}