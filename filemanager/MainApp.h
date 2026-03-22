#ifndef MAINAPP_H
#define MAINAPP_H

#include <wx/wx.h>

/**
 * Name: Shikha Patel
 * Description: wxWidgets application entry point class.
 * Date: jan 2026
 */
// This is the main application class that wxWidgets needs to start up.
class MainApp : public wxApp
{
public:
    /**
     * Function: OnInit
     * Description: Initializes the application and shows the main window.
     * Parameters: None
     * Returns: true on success, false on failure.
     */
     // This gets called automatically when the app starts.
    bool OnInit() override;
};
// This macro tells wxWidgets to use our MainApp class as the entry point
wxDECLARE_APP(MainApp);

#endif
