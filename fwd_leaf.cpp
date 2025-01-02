#include "pch.h"
#include "fwd_leaf.h"

namespace fwd
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	static const mwave::Pattern invalid_pattern{ -1 };
	static const seq::chain invalid_chain{ invalid_pattern };
	static const chRevIter invalid_iter{ invalid_chain.crbegin() };

	leaf::leaf(chRevIter iter)
		:info{}
		, pPrev{ nullptr }
		, itPat{ iter }
	{
	}

	leaf::leaf(chRevIter it, const MWINFO& i, leaf* p)
		:info{ i }
		, pPrev{ p }
		, itPat{ it }
	{
	}

	const leaf* leaf::is_tradable(const chRevIter itBegin, const chRevIter itEnd) const
	{
		if (itBegin == itEnd)
			return nullptr;

		if (*itPat != *itBegin)
			return nullptr;

		if (leaves.empty())
			return this;

		auto nxt{ itBegin + 1 };

		if (auto pLeaf{ find_child(*nxt) })
			return pLeaf->is_tradable(nxt, itEnd);

		return this;
	}

	leaf* leaf::find_child(mwave::Pattern p)const
	{
		for (auto& l : leaves)
			if (l->id() == p)
				return l.get();

		return nullptr;
	}

	mwave::Pattern leaf::id()const
	{
		return *itPat;
	}

	chRevIter leaf::get_iter() const
	{
		return itPat;
	}

	bool leaf::is_buy()const
	{
		return info.Net() >= .0 ? head().id().is_m() : head().id().is_w();
	}

	size_t leaf::depth()const
	{
		auto ret{ 0ull };
		auto p{ this };
		do ++ret;
		while (p = p->pPrev);
		return ret;
	}

	seq::chain leaf::get_chain()const
	{
		seq::chain ret;
		ret.reserve(depth());

		auto p{ this };

		do ret.push_back(p->id());
		while (p = p->pPrev);

		std::reverse(ret.begin(), ret.end());

		return ret;
	}

	const leaf* leaf::add(const chRevIter itBeg, const chRevIter itEnd, const MWINFO& i)
	{
		ASSERT(id() == *itBeg);

		auto find_split = [itEnd](auto itMy, auto itOth)
			{
				while (itMy != itEnd)
					if (*itMy++ != *itOth++)
						break;

				return itMy;
			};

		ASSERT(id() == *itBeg);
		ASSERT(itBeg != itPat);


		if (leaves.empty())
		{
			auto itSplit{ find_split(itPat + 1, itBeg + 1) };
			if (itSplit == itEnd)
				leaves.push_back(std::make_unique<leaf>(invalid_iter, info, this));
			else
			{
				auto p{ this };
				for (auto iter{ itPat + 1 }; iter != itSplit; ++iter)
				{
					p->leaves.push_back(std::make_unique<leaf>(iter, info, p));
					p = p->leaves.back().get();
				}
			}

			return add(itBeg, itEnd, i);
		}

		if (auto pLeaf{ find_child((char)*(itBeg + 1)) })
			return pLeaf->add(itBeg + 1, itEnd, i);

		leaves.push_back(std::make_unique<leaf>(itBeg + 1, i, this));

		head().update_info();

		return leaves.back().get();
	}

	const MWINFO& leaf::update_info()
	{
		if (!leaves.empty())
		{
			info = {};

			for (auto& l : leaves)
				info += l->update_info();
		}

		return info;
	}

	const leaf& leaf::head()const
	{
		return pPrev ? pPrev->head() : *this;
	}

	const MWINFO& leaf::get_info() const
	{
		return info;
	}

	leaf& leaf::head()
	{
		return pPrev ? pPrev->head() : *this;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	const leaf* tree::is_tradable(const chRevIter itFrom, const chRevIter itTo)const
	{
		if (itFrom == itTo)
			return nullptr;

		auto ind{ (char)*itFrom };

		auto& l{ m_Root[ind] };

		if (!l)
			return nullptr;

		return l->is_tradable(itFrom, itTo);
	}

	const leaf* tree::add(const chRevIter itBeg, const chRevIter itEnd, const MWINFO& i)
	{
		if (auto pRoot{ get_root(*itBeg) })
		{
			if (*itBeg == 10)
			{
				int y = 0;
			}
			return pRoot->add(itBeg, itEnd, i);
		}
		//return pRoot->add(itBeg, itEnd, i);

		m_Root[(char)*itBeg] = std::make_unique<leaf>(itBeg, i, nullptr);
		return m_Root[(char)*itBeg].get();
	}

	leaf* tree::get_root(mwave::Pattern p)
	{
		auto val{ (char)p };

		if (val < 0 || val>31)
			return nullptr;

		return m_Root[val].get();
	}
}