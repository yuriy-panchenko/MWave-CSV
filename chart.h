#pragma once

namespace chart
{
	class base abstract
	{

	public:
		virtual ~base();
		void prepare_context(CDC&, const CRect&);
		void draw(CDC&, CPoint pnt = {});

	protected:
		virtual void render()abstract;
		CDC& get_dc();
		CRect get_canvas()const;

	protected:
		double dMax{ DBL_MIN }, dMin{ DBL_MAX };

	private:
		void clear();

	private:
		CRect canvas;
		CBitmap bmp;
		CDC memDC;
		int iInitialState;
	};

	class line :public base
	{
		COLORREF color;
		int iWidth,iStyle;
		CPen pen;
		std::vector<double> data;

	public:
		line(std::vector<double>&&);

		// Inherited via base
		void render() override;
	};
}