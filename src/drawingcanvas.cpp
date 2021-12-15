#include "drawingcanvas.h"
#include <wx/graphics.h>
#include <wx/dcbuffer.h>
#include <iostream>

wxDEFINE_EVENT(CANVAS_RECT_ADDED, wxCommandEvent);
wxDEFINE_EVENT(CANVAS_RECT_REMOVED, wxCommandEvent);

DrawingCanvas::DrawingCanvas(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size) : wxWindow(parent, id, pos, size)
{
    this->SetBackgroundStyle(wxBG_STYLE_PAINT); // needed for windows

    this->Bind(wxEVT_PAINT, &DrawingCanvas::OnPaint, this);
    this->Bind(wxEVT_LEFT_DOWN, &DrawingCanvas::OnMouseDown, this);
    this->Bind(wxEVT_MOTION, &DrawingCanvas::OnMouseMove, this);
    this->Bind(wxEVT_LEFT_UP, &DrawingCanvas::OnMouseUp, this);
    this->Bind(wxEVT_LEAVE_WINDOW, &DrawingCanvas::OnMouseLeave, this);

    addRect(this->FromDIP(100), this->FromDIP(80), this->FromDIP(210), this->FromDIP(140), 0, *wxRED, "Rect #1");
    addRect(this->FromDIP(130), this->FromDIP(110), this->FromDIP(280), this->FromDIP(210), M_PI / 3.0, *wxBLUE, "Rect #2");
    addRect(this->FromDIP(110), this->FromDIP(110), this->FromDIP(300), this->FromDIP(120), -M_PI / 4.0, wxColor(255, 0, 255, 128), "Rect #3");

    this->draggedObj = nullptr;
    this->shouldRotate = false;
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

    this->objectList.push_back(obj);

    sendRectAddedEvent(text);
    Refresh();
}

void DrawingCanvas::removeTopRect()
{
    if (!this->objectList.empty() && draggedObj == nullptr)
    {
        auto text = this->objectList.back().text;
        this->objectList.pop_back();

        sendRectRemovedEvent(text);
        Refresh();
    }
}

void DrawingCanvas::OnPaint(wxPaintEvent &evt)
{
    // needed for windows
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();

    wxGraphicsContext *gc = wxGraphicsContext::Create(dc);

    if (gc)
    {
        for (const auto &object : objectList)
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

void DrawingCanvas::OnMouseDown(wxMouseEvent &event)
{
    auto clickedObjectIter = std::find_if(objectList.rbegin(), objectList.rend(), [event](const GraphicObject &o)
                                          {
                                              wxPoint2DDouble clickPos = event.GetPosition();
                                              auto inv = o.transform;
                                              inv.Invert();
                                              clickPos = inv.TransformPoint(clickPos);
                                              return o.rect.Contains(clickPos);
                                          });

    if (clickedObjectIter != objectList.rend())
    {
        auto forwardIt = std::prev(clickedObjectIter.base());

        objectList.push_back(*forwardIt);
        objectList.erase(forwardIt);

        draggedObj = &(*std::prev(objectList.end()));

        lastDragOrigin = event.GetPosition();
        shouldRotate = wxGetKeyState(WXK_ALT);

        Refresh(); // for z order reversal
    }
}

void DrawingCanvas::OnMouseMove(wxMouseEvent &event)
{
    if (draggedObj != nullptr)
    {
        if (shouldRotate)
        {
            double absoluteYDiff = event.GetPosition().y - lastDragOrigin.m_y;
            draggedObj->transform.Rotate(absoluteYDiff / 100.0 * M_PI);
        }
        else
        {
            auto dragVector = event.GetPosition() - lastDragOrigin;

            auto inv = draggedObj->transform;
            inv.Invert();
            dragVector = inv.TransformDistance(dragVector);

            draggedObj->transform.Translate(dragVector.m_x, dragVector.m_y);
        }

        lastDragOrigin = event.GetPosition();
        Refresh();
    }
}

void DrawingCanvas::OnMouseUp(wxMouseEvent &event)
{
    finishDrag();
    finishRotation();
}

void DrawingCanvas::OnMouseLeave(wxMouseEvent &event)
{
    finishDrag();
    finishRotation();
}

void DrawingCanvas::finishDrag()
{
    draggedObj = nullptr;
}

void DrawingCanvas::finishRotation()
{
    shouldRotate = false;
}

void DrawingCanvas::sendRectAddedEvent(const wxString &rectTitle)
{
    wxCommandEvent event(CANVAS_RECT_ADDED, GetId());
    event.SetEventObject(this);
    event.SetString(rectTitle);

    ProcessWindowEvent(event);
}

void DrawingCanvas::sendRectRemovedEvent(const wxString &rectTitle)
{
    wxCommandEvent event(CANVAS_RECT_REMOVED, GetId());
    event.SetEventObject(this);
    event.SetString(rectTitle);

    ProcessWindowEvent(event);
}