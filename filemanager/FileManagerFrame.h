#ifndef FILEMANAGERFRAME_H
#define FILEMANAGERFRAME_H

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <string>
#include <vector>
#include "FileSystemService.h"
#include "VirtualClipboard.h"

/**
 * Name: Shikha Patel
 * Description: Main GUI frame for the wxWidgets file manager.
 * Date: jan 2026, Last Modified: feb 2026
 */
// the main window class that contains all the UI elements and handles all the file operations.

class FileManagerFrame : public wxFrame
{
public:
    /**
     * Function: FileManagerFrame
     * Description: Constructs the main window and initializes UI.
     * Parameters:
     *   title: window title
     * Returns: N/A
     */
    explicit FileManagerFrame(const wxString& title);

    /**
     * Function: ~FileManagerFrame
     * Description: destructor, cleans up when window closes
     * Parameters: None
     * Returns: N/A
     */
    ~FileManagerFrame() override;

private:
// Menu/control IDs, to identify which button was clicked
    enum MenuIds
    {
        ID_PathBar = wxID_HIGHEST + 1,
        ID_ListCtrl,
        ID_Open,
        ID_NewFolder,
        ID_Rename,
        ID_Delete,
        ID_Copy,
        ID_Cut,
        ID_Paste,
        ID_Refresh,
        ID_Exit
    };
     // UI Components, the actual widgets displayed in the window
    wxTextCtrl* pathBar_;  // Text box showing current directory
    wxListCtrl* listCtrl_; //The main file listing
    // State tracking, what directory are we in and what files are there
    std::string currentDir_;
    std::vector<FileSystemService::FileItem> currentItems_;
    // Helper objects for file operations
    FileSystemService fs_;
    VirtualClipboard clipboard_; // Remembers copied/cut files
 
    // UI setup methods, called from constructor
    void BuildMenu();
    void BuildLayout();
    void BuildAccelerators();
    // Directory operations
    void LoadDirectory(const std::string& dirPath);
    void RefreshListing();
     //Helper methods for getting selected items
    long GetSelectedIndex() const;
    bool GetSelectedItem(FileSystemService::FileItem& outItem) const;

    void ShowError(const wxString& message);
    bool Confirm(const wxString& message);

    // Event handlers
    void OnPathEnter(wxCommandEvent& event);
    void OnItemActivated(wxListEvent& event);

    void OnOpen(wxCommandEvent& event);
    void OnNewFolder(wxCommandEvent& event);
    void OnRename(wxCommandEvent& event);
    void OnDelete(wxCommandEvent& event);
    void OnCopy(wxCommandEvent& event);
    void OnCut(wxCommandEvent& event);
    void OnPaste(wxCommandEvent& event);
    void OnRefresh(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    // This macro sets up the event table for wxWidgets.
    wxDECLARE_EVENT_TABLE();
};

#endif
