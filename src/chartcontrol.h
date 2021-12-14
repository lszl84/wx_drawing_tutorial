#pragma once

#include <wx/wx.h>
#include <vector>
#include <string>

class ChartControl : public wxWindow
{
public:
    ChartControl(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size);

    std::vector<double> values;
    std::string title;

private:
    void OnPaint(wxPaintEvent &evt);
    std::tuple<int, double, double> calculateChartSegmentCountAndRange(double origLow, double origHigh);
};