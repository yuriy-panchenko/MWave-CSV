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
	}

	void line::render()
	{
		auto& dc{ get_dc() };
		const auto iSave{ dc.SaveDC() };

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
