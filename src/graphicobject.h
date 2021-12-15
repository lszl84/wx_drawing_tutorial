#pragma once

#include <wx/wx.h>

struct GraphicObject
{
    wxRect2DDouble rect;
    wxColor color;
    wxString text;
    wxAffineMatrix2D transform;
};