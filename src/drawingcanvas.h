#pragma once

#include <wx/wx.h>
#include <list>

#include "graphicobject.h"

wxDECLARE_EVENT(CANVAS_RECT_ADDED, wxCommandEvent);
wxDECLARE_EVENT(CANVAS_RECT_REMOVED, wxCommandEvent);

class DrawingCanvas : public wxWindow
{
public:
    DrawingCanvas(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size);
    virtual ~DrawingCanvas() {}

    void addRect(int width, int height, int centerX, int centerY, double angle, wxColor color, const std::string &text);
    void removeTopRect();

    int getObjectCount() { return objectList.size(); }

private:
    void OnPaint(wxPaintEvent &evt);
    void OnMouseDown(wxMouseEvent &event);
    void OnMouseMove(wxMouseEvent &event);
    void OnMouseUp(wxMouseEvent &event);
    void OnMouseLeave(wxMouseEvent &event);

    void finishDrag();
    void finishRotation();

    void sendRectAddedEvent(const wxString &rectTitle);
    void sendRectRemovedEvent(const wxString &rectTitle);

    std::list<GraphicObject> objectList;

    GraphicObject *draggedObj;
    bool shouldRotate;

    wxPoint2DDouble lastDragOrigin;
};