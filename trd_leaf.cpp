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

	const leaf* leaf::head()const
	{
		auto ret{ this };
		ASSERT(ret == this);

		while (ret->pPrev)
		{
			ASSERT_NULL_OR_POINTER(ret->pPrev, trd::leaf*);
			ret = ret->pPrev;
		}

		return ret;
	}

	bool leaf::is_buy() const
	{
		ASSERT(is_valid());

		return diff() >= .0 ? head()->pat.is_m() : head()->pat.is_w();
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

	seq::chain leaf::chain() const
	{
		seq::chain ret;
		ret.reserve(depth());

		auto p{ this };

		do
		{
			ret.push_back(p->pat);
			p = p->pPrev;
		} while (p);

		std::reverse(ret.begin(), ret.end());

		return ret;
	}

	size_t leaf::depth() const
	{
		size_t ret{ 0ull };

		auto p{ this };

		do
		{
			++ret;
			p = p->pPrev;
		} while (p);

		return ret;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	void tree::set(std::unique_ptr<leaf>&& l)
	{
		assert(l->is_valid());
		m_Root[(char)l->id()] = std::move(l);
	}

	const leaf* tree::is_tradable(const seq::chain::const_reverse_iterator itFrom, const seq::chain::const_reverse_iterator itTo) const
	{
		ASSERT(itFrom != itTo);

		if (0 > (char)*itFrom || 31 < (char)*itFrom)
			return nullptr;

		return m_Root[(char)*itFrom]->is_tradable(itFrom, itTo);
	}
}