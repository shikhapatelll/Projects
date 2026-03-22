#include "MainApp.h"
#include "FileManagerFrame.h"

/**
 * Name: Shikha Patel
 * Description: Implements the wxWidgets application entry.
 * Date: jan 2026
 */
// This file just sets up the app and creates the main window.
//This macro creates the actual application instance
wxIMPLEMENT_APP(MainApp);

/**
 * Function: OnInit
 * Description: wxWidgets application initialization callback. Creates and shows the main frame.
 * Parameters: None
 * Returns: true if initialization succeeds; false otherwise.
 */
bool MainApp::OnInit()
{
    if (!wxApp::OnInit()) // First call the parent class OnInit
    {
        return false;
    }

    // Create the main window. wxWidgets will own and manage its lifetime after Show().
    FileManagerFrame* frame = new FileManagerFrame("CS3307 File Manager");
    frame->Show(true); // Shows the window
    return true;
}
