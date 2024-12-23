#include "pch.h"
#include "seq.h"

namespace seq
{
	leaf::leaf(mwave::Pattern mw)
		:pat{ mw }
		, hItem{ NULL }
		, pPrev{ nullptr }
	{
	}

	leaf::leaf(mwave::Pattern mw, const leaf* pParent)
		:pat{ mw }
		, hItem{ NULL }
		, pPrev{ pParent }
	{
	}

	leaf::~leaf()
	{
		for (auto pL : leaves)
			delete pL;
	}

	void leaf::set_indexes(std::vector<INT_PTR>&& v)
	{
		indexes = std::move(v);
	}

	leaf& leaf::operator=(leaf&& oth)
	{
		pat = oth.pat;
		hItem = oth.hItem;
		pPrev = oth.pPrev;
		for (auto pL : leaves)
			delete pL;
		leaves = std::move(oth.leaves);
		indexes = std::move(oth.indexes);

		oth.leaves.clear();

		return *this;
	}

	mwave::Pattern leaf::get_pattern()const
	{
		return pat;
	}

	bool leaf::grow(const chain& mws)
	{
		indexes.clear();
		leaves.clear();

		const auto id{ get_chain() };

		if (pPrev)
			indexes.reserve(pPrev->get_indexes().size());
		else
			indexes.reserve(mws.size() / 32);

		//auto is_same_chain = [&](INT_PTR index)->bool
		//	{
		//		if (index < INT_PTR(id.size() - 1))
		//			return false;

		//		auto const irFrom{ mws.crbegin() + (mws.size() - index - 1) },
		//			irTo{ irFrom + id.size() };

		//		ASSERT(irFrom < mws.crend());

		//		return chain{ irFrom, irTo } == id;
		//	};

		if (pPrev)
			for (auto ind : pPrev->get_indexes())
			{
				if (ind
					&& mws[ind - 1] == pat
					//&& is_same_chain((ind - 1) + id.size() - 1)
					)
					indexes.push_back(ind - 1);
			}
		else
			for (size_t i = 0; i < mws.size(); ++i)
				if (mws[i] == id.front()/* && is_same_chain(i)*/)
					indexes.push_back(i);

		indexes.shrink_to_fit();

		if (indexes.size() < 2)
			return false;

		for (char ch = 0; ch < 32; ++ch)
		{
			const mwave::Pattern oth{ ch };
			if (pat.is_m() ^ oth.is_m())
			{
				auto pL{ new leaf{ oth, this } };
				if (pL->grow(mws))
					leaves.push_back(pL);
				else
					delete pL;
			}
		}

		return true;
	}

	std::vector<leaf*>& leaf::get_leaves()
	{
		return leaves;
	}

	std::vector<INT_PTR>& leaf::get_indexes()
	{
		return indexes;
	}

	void leaf::set_handle(HTREEITEM h)
	{
		hItem = h;
	}

	chain leaf::get_chain()const
	{
		if (pPrev)
		{
			auto ch{ pPrev->get_chain() };
			ch.push_back(pat);
			return ch;
		}
		else
			return { pat };
	}

	int leaf::get_level() const
	{
		if (pPrev)
			return pPrev->get_level() + 1;
		else
			return 0;
	}

	const leaf* leaf::head() const
	{
		const leaf* pRet{ this };

		while (pRet)
			if (pRet->pPrev)
				pRet = pRet->pPrev;
			else
				break;

		return pRet;
	}

	const std::vector<leaf*>& leaf::get_leaves() const
	{
		return leaves;
	}

	const std::vector<INT_PTR>& leaf::get_indexes() const
	{
		return indexes;
	}

	size_t leaf::get_max_depth() const
	{
		size_t count{};

		for (auto pL : leaves)
			count = max(count, pL->get_max_depth());

		return count + 1;
	}

	HTREEITEM leaf::get_handle() const
	{
		return hItem;
	}

	const leaf* leaf::find(HTREEITEM h) const
	{
		if (hItem == h)
			return this;

		for (auto pL : leaves)
			if (auto pLeaf{ pL->find(h) })
				return pLeaf;

		return nullptr;
	}
}