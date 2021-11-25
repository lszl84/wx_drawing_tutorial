#include "drawingcanvas.h"
#include <wx/graphics.h>
#include <wx/dcbuffer.h>
#include <iostream>

DrawingCanvas::DrawingCanvas(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size) : wxWindow(parent, id, pos, size)
{
    this->SetBackgroundStyle(wxBG_STYLE_PAINT); // needed for windows

    this->Bind(wxEVT_PAINT, &DrawingCanvas::OnPaint, this);
}

void DrawingCanvas::OnPaint(wxPaintEvent &evt)
{
    // needed for windows
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();

    wxGraphicsContext *gc = wxGraphicsContext::Create(dc);

    if (gc)
    {
        wxSize rectSize = this->FromDIP(wxSize(100, 80));
        wxPoint rectOrigin = {-rectSize.GetWidth() / 2, -rectSize.GetHeight() / 2};

        wxAffineMatrix2D transform{};
        transform.Translate(100, 130);
        transform.Rotate(M_PI / 3.0);
        transform.Scale(3, 3);

        gc->SetTransform(gc->CreateMatrix(transform));

        gc->SetBrush(*wxRED_BRUSH);
        gc->DrawRectangle(rectOrigin.x, rectOrigin.y, rectSize.GetWidth(), rectSize.GetHeight());

        gc->SetFont(*wxNORMAL_FONT, *wxWHITE);
        wxString text = "Text";

        double textWidth, textHeight;
        gc->GetTextExtent(text, &textWidth, &textHeight);

        gc->DrawText(text, rectOrigin.x + rectSize.GetWidth() / 2.0 - textWidth / 2.0, rectOrigin.y + rectSize.GetHeight() / 2.0 - textHeight / 2.0);

        delete gc;
    }
}
