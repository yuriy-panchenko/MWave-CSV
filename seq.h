#pragma once

namespace seq
{
	using chain = std::vector<mwave::Pattern>;
	//class chain {};

	class leaf
	{
		mwave::Pattern pat;
		HTREEITEM hItem;
		const leaf* pPrev;
		std::vector<leaf> leaves;
		std::vector<INT_PTR> indexes;

	public:
		leaf() = default;
		leaf(mwave::Pattern mw);
		leaf(mwave::Pattern mw, const leaf* pParent);

		mwave::Pattern get_pattern()const;
		chain get_chain()const;
		int get_level()const;
		const leaf* head()const;
		const std::vector<leaf>& get_leaves()const;
		const std::vector<INT_PTR>& get_indexes()const;
		size_t get_max_depth()const;

		void set_indexes(std::vector<INT_PTR>&& v);
		bool grow(const CArray<mwave::Pattern>& mws);
		std::vector<leaf>& get_leaves();
		std::vector<INT_PTR>& get_indexes();
		void set_handle(HTREEITEM);
	};
}