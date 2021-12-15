#include "drawingcanvas.h"
#include <wx/graphics.h>
#include <wx/dcbuffer.h>
#include <iostream>

DrawingCanvas::DrawingCanvas(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size) : wxWindow(parent, id, pos, size)
{
    this->SetBackgroundStyle(wxBG_STYLE_PAINT); // needed for windows

    this->Bind(wxEVT_PAINT, &DrawingCanvas::OnPaint, this);

    addRect(this->FromDIP(100), this->FromDIP(80), this->FromDIP(210), this->FromDIP(140), 0, *wxRED, "Rect #1");
    addRect(this->FromDIP(130), this->FromDIP(110), this->FromDIP(280), this->FromDIP(210), M_PI / 3.0, *wxBLUE, "Rect #2");
    addRect(this->FromDIP(110), this->FromDIP(110), this->FromDIP(300), this->FromDIP(120), -M_PI / 4.0, wxColor(255, 0, 255, 128), "Rect #3");
}

void DrawingCanvas::addRect(int width, int height, int centerX, int centerY, double angle, wxColor color, const std::string &text)
{
    GraphicObject obj{
        {-width / 2.0,
         -height / 2.0,
         static_cast<double>(width),
         static_cast<double>(height)},
        color,
        text,
        {}};

    obj.transform.Translate(
        static_cast<double>(centerX),
        static_cast<double>(centerY));

    obj.transform.Rotate(angle);

    this->objectArray.push_back(obj);

    Refresh();
}

void DrawingCanvas::OnPaint(wxPaintEvent &evt)
{
    // needed for windows
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();

    wxGraphicsContext *gc = wxGraphicsContext::Create(dc);

    if (gc)
    {
        for (const auto &object : objectArray)
        {
            gc->SetTransform(gc->CreateMatrix(object.transform));

            gc->SetBrush(wxBrush(object.color));
            gc->DrawRectangle(object.rect.m_x, object.rect.m_y, object.rect.m_width, object.rect.m_height);

            gc->SetFont(*wxNORMAL_FONT, *wxWHITE);

            double textWidth, textHeight;
            gc->GetTextExtent(object.text, &textWidth, &textHeight);

            gc->DrawText(object.text, object.rect.m_x + object.rect.m_width / 2.0 - textWidth / 2.0, object.rect.m_y + object.rect.m_height / 2.0 - textHeight / 2.0);
        }

        delete gc;
    }
}
