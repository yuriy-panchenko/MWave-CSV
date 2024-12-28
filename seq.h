#pragma once

namespace seq
{
	using chain = std::vector<mwave::Pattern>;

	class leaf
	{
		enum class cbstate { unselected = 1, selected, mix, };
		
		HTREEITEM hItem;
		leaf* pPrev;
		
		mwave::Pattern pat;
		bool isSelected{ false };
		std::vector<INT_PTR> indexes;
		std::vector<leaf*> leaves;

	public:
		leaf() = default;
		leaf(const leaf&) = delete;
		leaf(leaf&&);
		leaf(mwave::Pattern mw);
		leaf(mwave::Pattern mw, leaf* pParent);
		~leaf();

		leaf& operator=(const leaf&) = delete;
		leaf& operator=(leaf&&);

		mwave::Pattern get_pattern()const;
		chain get_chain()const;
		int get_level()const;
		const leaf* head()const;
		const leaf* parent()const;
		const std::vector<leaf*>& get_leaves()const;
		const std::vector<INT_PTR>& get_indexes()const;
		size_t get_max_depth()const;
		HTREEITEM get_handle()const;
		const leaf* find(HTREEITEM)const;
		bool is_selected()const;
		cbstate get_icon_state()const;
		size_t get_siblings_count()const;
		bool is_valid()const;

		void set_indexes(std::vector<INT_PTR>&& v);
		void set_indexes(const std::vector<INT_PTR>& v);
		bool grow(const chain& mws);
		std::vector<leaf*>& get_leaves();
		std::vector<INT_PTR>& get_indexes();
		void set_handle(HTREEITEM);
		void select(bool b = true);
		leaf* find(HTREEITEM);
		void Serialize(CArchive&);
		void LoadChildren(CArchive& ar, const leaf* pParent);
		leaf* parent();
		bool select_by_children();
	};
}

CString ToString(const seq::chain&);