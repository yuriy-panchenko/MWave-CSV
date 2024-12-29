#include "pch.h"
#include "chart.h"

namespace chart
{
	chart::line::line(std::vector<double>&& arr)
		:color{ RGB(50,220,50) }
		, iWidth{ 1 }
		, iStyle{ PS_SOLID }
		, data{ std::move(arr) }
	{
		pen.CreatePen(iStyle, iWidth, color);

		for (auto db : data)
			dMax = max(dMax, db),
			dMin = min(dMin, db);

		const auto extra{ (dMax - dMin) / 40 };
		dMax += extra, dMin -= extra;
	}

	void line::render()
	{
		const auto elCount{ data.size() };
		CArray<CPoint> pnts;
		pnts.SetSize(elCount);
		const auto c{ get_canvas() };
		const auto cx{ c.Width() }, cy{ c.Height() };
		const auto diff{ dMax - dMin };

		auto to_point = [&](int ind, double val)->CPoint
			{
				CPoint ret;
				ret.x = c.left + int((ind + .5) * cx / elCount);
				ret.y = c.top + int((dMax - val) * cy / diff);
				return ret;
			};

		int index{ 0 };

		for (auto db : data)
		{
			pnts.SetAt(index, to_point(index, db));
			++index;
		}

		auto& dc{ get_dc() };
		const auto iSave{ dc.SaveDC() };
		dc.SelectObject(pen);
		auto pnt{ to_point(0, .0) };
		dc.MoveTo(pnt);

		for (INT_PTR i = 0; i < pnts.GetSize(); ++i)
			if (pnts[i] != pnt)
				dc.LineTo(pnt = pnts[i]);

		dc.RestoreDC(iSave);
	}

	base::~base()
	{
		clear();
	}

	void base::prepare_context(CDC& dc, const CRect& rect)
	{
		clear();

		canvas = rect;
		if (bmp.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height())
			&& memDC.CreateCompatibleDC(&dc))
		{
			iInitialState = memDC.SaveDC();
			memDC.SelectObject(bmp);

			render();
		}
	}

	void base::draw(CDC& dc, CPoint pnt)
	{
		dc.BitBlt(pnt.x, pnt.y, canvas.Width(), canvas.Height(), &memDC, 0, 0, SRCCOPY);
	}

	CDC& base::get_dc()
	{
		return memDC;
	}

	CRect base::get_canvas() const
	{
		return canvas;
	}

	void base::clear()
	{
		if (memDC.GetSafeHdc())
		{
			memDC.RestoreDC(iInitialState);
			memDC.DeleteDC();
		}

		bmp.DeleteObject();

		canvas = {};
	}
}
