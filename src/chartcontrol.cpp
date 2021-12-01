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

    if (gc)
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

        wxRect2DDouble chartArea = fullArea;
        chartArea.Inset(marginX, marginTop, marginX, marginBottom);

        gc->DrawText(this->title, (fullArea.GetSize().GetWidth() - tw) / 2.0, (marginTop - th) / 2.0);

        gc->SetBrush(*wxRED_BRUSH);
        gc->DrawRectangle(chartArea.m_x, chartArea.m_y, chartArea.m_width, chartArea.m_height);

        delete gc;
    }
}