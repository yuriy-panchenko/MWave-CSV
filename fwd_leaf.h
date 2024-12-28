#pragma once

namespace fwd
{
	using chRevIter = seq::chain::const_reverse_iterator;
	
	class leaf
	{
	public:
		leaf();
		leaf(const leaf&) = delete;
		leaf(leaf&&) = default;
		leaf(chRevIter);
		//leaf(mwave::Pattern p);
		~leaf() = default;

		leaf& operator=(const leaf&) = delete;
		leaf& operator=(leaf&&) = default;
	
		const leaf* is_tradable(chRevIter, chRevIter)const;
		bool is_buy()const;
		seq::chain get_chain()const;
		size_t depth()const;
		const leaf* find_child(mwave::Pattern p)const;
		mwave::Pattern id()const;
		chRevIter get_iter()const;
	
	private:
		//mwave::Pattern pat;
		MWINFO info;
		const leaf* pPrev;

		chRevIter itPat;
		std::vector<std::unique_ptr<leaf>> leaves;
	};
	
	class tree
	{
		leaf m_Root[32];

	public:
		//tree();
		tree() = default;

		const leaf* is_tradable(chRevIter, chRevIter);
		const leaf* add(chRevIter, chRevIter, const MWINFO&);
	};
}