#include "chartcontrol.h"
#include <wx/settings.h>
#include <wx/graphics.h>
#include <wx/dcbuffer.h>

// wxFULL_REPAINT_ON_RESIZE needed for Windows
ChartControl::ChartControl(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size) : wxWindow(parent, id, pos, size, wxFULL_REPAINT_ON_RESIZE)
{
    this->SetBackgroundStyle(wxBG_STYLE_PAINT); // needed for windows

    this->Bind(wxEVT_PAINT, &ChartControl::OnPaint, this);
}

void ChartControl::OnPaint(wxPaintEvent &evt)
{
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();
    wxGraphicsContext *gc = wxGraphicsContext::Create(dc);

    if (gc && values.size() > 0)
    {
        wxFont titleFont = wxFont(wxNORMAL_FONT->GetPointSize() * 2.0,
                                  wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);

        gc->SetFont(titleFont, wxSystemSettings::GetAppearance().IsDark() ? *wxWHITE : *wxBLACK);

        double tw, th;
        gc->GetTextExtent(this->title, &tw, &th);

        const double titleTopBottomMinimumMargin = this->FromDIP(10);

        wxRect2DDouble fullArea{0, 0, static_cast<double>(GetSize().GetWidth()), static_cast<double>(GetSize().GetHeight())};

        const double marginX = fullArea.GetSize().GetWidth() / 8.0;
        const double marginTop = std::max(fullArea.GetSize().GetHeight() / 8.0, titleTopBottomMinimumMargin * 2.0 + th);
        const double marginBottom = fullArea.GetSize().GetHeight() / 8.0;
        double labelsToChartAreaMargin = this->FromDIP(10);

        wxRect2DDouble chartArea = fullArea;
        chartArea.Inset(marginX, marginTop, marginX, marginBottom);

        gc->DrawText(this->title, (fullArea.GetSize().GetWidth() - tw) / 2.0, (marginTop - th) / 2.0);

        wxAffineMatrix2D normalizedToChartArea{};
        normalizedToChartArea.Translate(chartArea.GetLeft(), chartArea.GetTop());
        normalizedToChartArea.Scale(chartArea.m_width, chartArea.m_height);

        double lowValue = *std::min_element(values.begin(), values.end());
        double highValue = *std::max_element(values.begin(), values.end());
        double yValueSpan = highValue - lowValue;

        wxAffineMatrix2D normalizedToValue{};
        normalizedToValue.Translate(0, highValue);
        normalizedToValue.Scale(1, -1);
        normalizedToValue.Scale(static_cast<double>(values.size() - 1), yValueSpan);

        const int yLinesCount = 11;

        gc->SetPen(wxPen(wxColor(128, 128, 128)));
        gc->SetFont(*wxNORMAL_FONT, wxSystemSettings::GetAppearance().IsDark() ? *wxWHITE : *wxBLACK);

        for (int i = 0; i < yLinesCount; i++)
        {
            double normalizedLineY = static_cast<double>(i) / (yLinesCount - 1);

            auto lineStartPoint = normalizedToChartArea.TransformPoint({0, normalizedLineY});
            auto lineEndPoint = normalizedToChartArea.TransformPoint({1, normalizedLineY});

            wxPoint2DDouble linePoints[] = {lineStartPoint, lineEndPoint};
            gc->StrokeLines(2, linePoints);

            double valueAtLineY = normalizedToValue.TransformPoint({0, normalizedLineY}).m_y;

            auto text = wxString::Format("%.2f", valueAtLineY);
            text = wxControl::Ellipsize(text, dc, wxELLIPSIZE_MIDDLE, chartArea.GetLeft() - labelsToChartAreaMargin);

            double tw, th;
            gc->GetTextExtent(text, &tw, &th);
            gc->DrawText(text, chartArea.GetLeft() - labelsToChartAreaMargin - tw, lineStartPoint.m_y - th / 2.0);
        }

        wxPoint2DDouble leftHLinePoints[] = {
            normalizedToChartArea.TransformPoint({0, 0}),
            normalizedToChartArea.TransformPoint({0, 1})};

        wxPoint2DDouble rightHLinePoints[] = {
            normalizedToChartArea.TransformPoint({1, 0}),
            normalizedToChartArea.TransformPoint({1, 1})};

        gc->StrokeLines(2, leftHLinePoints);
        gc->StrokeLines(2, rightHLinePoints);

        delete gc;
    }
}