#include "FileManagerFrame.h"
#include <wx/filename.h>
#include <wx/textdlg.h>
#include <wx/msgdlg.h>
#include <wx/clipbrd.h>
#include <filesystem>
#include <sstream>

/**
 * Name: Shikha Patel
 * Description: Implementation of the main file manager GUI frame.
 * Date: jan 2026, Last Modified: feb 2026
 */


// Event table: connects menu IDs to handler functions
wxBEGIN_EVENT_TABLE(FileManagerFrame, wxFrame)
    EVT_TEXT_ENTER(FileManagerFrame::ID_PathBar, FileManagerFrame::OnPathEnter)
    EVT_LIST_ITEM_ACTIVATED(FileManagerFrame::ID_ListCtrl, FileManagerFrame::OnItemActivated)

    EVT_MENU(FileManagerFrame::ID_Open, FileManagerFrame::OnOpen)
    EVT_MENU(FileManagerFrame::ID_NewFolder, FileManagerFrame::OnNewFolder)
    EVT_MENU(FileManagerFrame::ID_Rename, FileManagerFrame::OnRename)
    EVT_MENU(FileManagerFrame::ID_Delete, FileManagerFrame::OnDelete)
    EVT_MENU(FileManagerFrame::ID_Copy, FileManagerFrame::OnCopy)
    EVT_MENU(FileManagerFrame::ID_Cut, FileManagerFrame::OnCut)
    EVT_MENU(FileManagerFrame::ID_Paste, FileManagerFrame::OnPaste)
    EVT_MENU(FileManagerFrame::ID_Refresh, FileManagerFrame::OnRefresh)
    EVT_MENU(FileManagerFrame::ID_Exit, FileManagerFrame::OnExit)
wxEND_EVENT_TABLE()

/**
 * Function: FileManagerFrame
 * Description: Constructs the main application window and initializes the UI (menu, path bar,
 *              file listing, status bar, and keyboard shortcuts). Also loads the starting directory.
 * Parameters:
 *   title: window title displayed by the OS/window manager
 * Returns: N/A
 */
    
FileManagerFrame::FileManagerFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(900, 600)),
      pathBar_(nullptr),
      listCtrl_(nullptr),
      currentDir_(".")
{
        // Status bar gives quick feedback about operations (copy/cut/paste, errors, etc.)
    //Creates the status bar at bottom of window
    CreateStatusBar();
    SetStatusText("Ready.");
    // Build UI pieces in a predictable order: menus -> widgets/layout -> keyboard shortcuts
    BuildMenu();
    BuildLayout();
    BuildAccelerators();

    // Starting in the current working directory 
    //I use error_code here to avoid exceptions
    std::error_code ec;
    currentDir_ = std::filesystem::current_path(ec).string();
    LoadDirectory(currentDir_);
}

/**
 * Function: ~FileManagerFrame
 * Description: Destructor for the main frame.
 * Parameters: None
 * Returns: N/A
 */
FileManagerFrame::~FileManagerFrame() = default;

/**
 * Function: BuildMenu
 * Description: Builds the menu bar and connects menu items (open, copy, paste, etc.) to the event table IDs.
 * Parameters: None
 * Returns: None
 */
void FileManagerFrame::BuildMenu()
{
    
    // Creating File menu
    wxMenu* fileMenu = new wxMenu;
    fileMenu->Append(ID_Open, "Open\tEnter");
    fileMenu->Append(ID_NewFolder, "New Folder...\tCtrl+N");
    fileMenu->AppendSeparator();
    fileMenu->Append(ID_Exit, "Exit\tCtrl+Q");
    //here creating Edit menu
    wxMenu* editMenu = new wxMenu;
    editMenu->Append(ID_Rename, "Rename...\tF2");
    editMenu->Append(ID_Delete, "Delete...\tDel");
    editMenu->AppendSeparator();
    editMenu->Append(ID_Copy, "Copy\tCtrl+C");
    editMenu->Append(ID_Cut, "Cut\tCtrl+X");
    editMenu->Append(ID_Paste, "Paste\tCtrl+V");
    //here Creating View menu
    wxMenu* viewMenu = new wxMenu;
    viewMenu->Append(ID_Refresh, "Refresh\tF5");
    // Putting all menus into the menu bar
    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(editMenu, "&Edit");
    menuBar->Append(viewMenu, "&View");

    SetMenuBar(menuBar);
}

/**
 * Function: BuildLayout
 * Description: Creates the visible widgets (editable path bar + file list) and arranges them using sizers.
 * Parameters: None
 * Returns: None
 */
void FileManagerFrame::BuildLayout()
{
     // Creating a panel to hold our controls
    wxPanel* panel = new wxPanel(this);
     //Creating the path bar
    //The wxTE_PROCESS_ENTER flag lets us catch when user hits Enter
    pathBar_ = new wxTextCtrl(panel, ID_PathBar, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    // Creating the list control (main file listing)
    // wxLC_REPORT gives us the detailed list view with columns
    listCtrl_ = new wxListCtrl(panel, ID_ListCtrl, wxDefaultPosition, wxDefaultSize,
                              wxLC_REPORT | wxLC_SINGLE_SEL); // wxLC_SINGLE_SEL, only one item can be selected at a time
//Adding columns to the list
    listCtrl_->InsertColumn(0, "Name", wxLIST_FORMAT_LEFT, 320);
    listCtrl_->InsertColumn(1, "Type", wxLIST_FORMAT_LEFT, 100);
    listCtrl_->InsertColumn(2, "Size", wxLIST_FORMAT_RIGHT, 120);
    listCtrl_->InsertColumn(3, "Modified", wxLIST_FORMAT_LEFT, 180);
    // Using a sizer to layout the controls vertically
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(pathBar_, 0, wxEXPAND | wxALL, 6); // 0 = don't expand vertically
    sizer->Add(listCtrl_, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 6); // 1 = expand to fill space

    panel->SetSizer(sizer);
}

/**
 * Function: BuildAccelerators
 * Description: Defines keyboard shortcuts (accelerators) that trigger the same menu commands as clicking.
 * Parameters: None
 * Returns: None
 */
void FileManagerFrame::BuildAccelerators()
{
    // Keyboard shortcuts, this lets users press keys instead of clicking menus
    std::vector<wxAcceleratorEntry> entries;
    entries.emplace_back(wxACCEL_CTRL, (int)'N', ID_NewFolder);
    entries.emplace_back(wxACCEL_CTRL, (int)'Q', ID_Exit);
    entries.emplace_back(wxACCEL_CTRL, (int)'C', ID_Copy);
    entries.emplace_back(wxACCEL_CTRL, (int)'X', ID_Cut);
    entries.emplace_back(wxACCEL_CTRL, (int)'V', ID_Paste);
    entries.emplace_back(wxACCEL_NORMAL, WXK_F5, ID_Refresh);
    entries.emplace_back(wxACCEL_NORMAL, WXK_F2, ID_Rename);
    entries.emplace_back(wxACCEL_NORMAL, WXK_DELETE, ID_Delete);

    // Enter key can trigger Open via menu; double-click also works.
    entries.emplace_back(wxACCEL_NORMAL, WXK_RETURN, ID_Open);

    wxAcceleratorTable accel((int)entries.size(), entries.data());
    SetAcceleratorTable(accel);
}

/**
 * Function: LoadDirectory
 * Description: Changes the current directory (if valid), updates the path bar text, and refreshes the listing.
 * Parameters:
 *   dirPath, directory path to load (absolute or relative)
 * Returns: None
 */

void FileManagerFrame::LoadDirectory(const std::string& dirPath)
{
    if (!fs_.Exists(dirPath) || !fs_.IsDirectory(dirPath)) // Checking if the path is valid before trying to load it
    {
        ShowError("Not a valid directory:\n" + wxString(dirPath));
        return;
    }
    // Update our current directory and the path bar
    currentDir_ = dirPath;
    pathBar_->ChangeValue(wxString(currentDir_));
    RefreshListing(); // Reloading the file listing
    SetStatusText("Listing: " + wxString(currentDir_));  // Updates status bar to show what we're looking at
}

/**
 * Function: RefreshListing
 * Description: Reads the current directory contents and repopulates the list control (name/type/size/modified).
 * Parameters: None
 * Returns: None
 */
void FileManagerFrame::RefreshListing()
{
    listCtrl_->DeleteAllItems(); //Clearing out the old list
    currentItems_.clear();

    // Add ".." manually when not at filesystem root
    std::filesystem::path cur(currentDir_);
    std::filesystem::path root = cur.root_path();
    if (!root.empty() && cur != root)
    {
        FileSystemService::FileItem up{};
        up.name = "..";
        up.fullPath = cur.parent_path().string();
        up.isDirectory = true;
        up.sizeBytes = 0;
        up.modified = "";
        currentItems_.push_back(up);
    }
    // Get the actual files and directories from the filesystem
    auto items = fs_.ListDirectory(currentDir_);
    // Append actual items, Adding them to our item list
    for (const auto& it : items)
    {
        currentItems_.push_back(it);
    }
    // Now populate the list control with all items
    for (long z = 0; z < (long)currentItems_.size(); ++z)
    {
        const auto& item = currentItems_[z];
        //helps with figuring out if it's a file or directory
        const wxString typeStr = item.isDirectory ? "Directory" : "File";
        //Formating the size, only showing for files, not directories
        wxString sizeStr = "-";
        if (!item.isDirectory)
        {
            std::ostringstream oss;
            oss << item.sizeBytes;
            sizeStr = wxString(oss.str());
        }
        //Adding the row to the list
        const long idx = listCtrl_->InsertItem(z, wxString(item.name));
        listCtrl_->SetItem(idx, 1, typeStr);
        listCtrl_->SetItem(idx, 2, sizeStr);
        listCtrl_->SetItem(idx, 3, wxString(item.modified));
    }
}

/**
 * Function: GetSelectedIndex
 * Description: Finds the currently selected row in the list control.
 * Parameters: None
 * Returns: The selected row index, or -1 if nothing is selected.
 */
long FileManagerFrame::GetSelectedIndex() const
{  // Returning -1 if nothing is selected
    return listCtrl_->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
}

/**
 * Function: GetSelectedItem
 * Description: Retrieves the FileItem corresponding to the selected row.
 * Parameters:
 *   outItem - output parameter that receives the selected item if one exists
 * Returns: true if an item is selected; false otherwise.
 */
bool FileManagerFrame::GetSelectedItem(FileSystemService::FileItem& outItem) const
{  
    const long idx = GetSelectedIndex();
    if (idx < 0 || idx >= (long)currentItems_.size()) // Checking if have a valid selection
    {
        return false;
    }
    outItem = currentItems_[(size_t)idx];
    return true; 
    // Return the selected item
}

/**
 * Function: ShowError
 * Description: Shows an error message dialog to the user.
 * Parameters:
 *   message: message to show
 * Returns: None
 */
void FileManagerFrame::ShowError(const wxString& message)
{   //Shows an error dialog box
    wxMessageBox(message, "Error", wxOK | wxICON_ERROR, const_cast<FileManagerFrame*>(this));
}

/**
 * Function: Confirm
 * Description: Shows a Yes/No confirmation dialog.
 * Parameters:
 *   message: prompt text to display
 * Returns: true if user clicks Yes; false otherwise.
 */
bool FileManagerFrame::Confirm(const wxString& message)
{
    return wxMessageBox(message, "Confirm", wxYES_NO | wxICON_QUESTION, this) == wxYES;
    // Shows a yes/no confirmation dialog
    // Returns true if clicked Yes
}

/**
 * Function: OnPathEnter
 * Description: Handles Enter on the path bar; attempts to navigate to the typed directory.
 * Parameters:
 *   event: text control event (required by wxWidgets signature)
 * Returns: None
 */
void FileManagerFrame::OnPathEnter(wxCommandEvent& event)
{  // when user typed a path and hit Enter in the path bar
    const wxString entered = pathBar_->GetValue();
    LoadDirectory(std::string(entered.mb_str()));
    event.Skip();
}

/**
 * Function: OnItemActivated
 * Description: Handles double-click on a list item; triggers the same behavior as Open.
 * Parameters:
 *   event: list activation event containing the activated row index
 * Returns: None
 */
void FileManagerFrame::OnItemActivated(wxListEvent& event)
{  // when user double-clicked an item
    wxCommandEvent dummy;
    OnOpen(dummy);
    event.Skip();
}

/**
 * Function: OnOpen
 * Description: Opens the selected item. Directories are entered; files are opened with the OS default app.
 * Parameters:
 *   event: command event (unused except for wxWidgets signature)
 * Returns: None
 */
void FileManagerFrame::OnOpen(wxCommandEvent& event)
{
    wxUnusedVar(event); // event not used

    FileSystemService::FileItem item{};
    if (!GetSelectedItem(item))
    {
        SetStatusText("Select a file or directory first.");
        return;
    }
    // If it's a directory, navigating into it
    if (item.isDirectory)
    {
        LoadDirectory(item.fullPath);
        return;
    }
    // If it's a file, trying to open it with the default application
    const wxString pathWx(item.fullPath);
    if (!wxLaunchDefaultApplication(pathWx))
    {
        ShowError("Could not open file with default application:\n" + pathWx);
    }
}
/**
 * Function: OnNewFolder
 * Description: Prompts for a folder name and creates a new directory in the current directory.
 * Parameters:
 *   event: command event
 * Returns: None
 */
void FileManagerFrame::OnNewFolder(wxCommandEvent& event)
{
    wxUnusedVar(event); // event not used
  // Showing a dialog asking for the folder name
    wxTextEntryDialog dlg(this, "Enter new folder name:", "New Folder");
    if (dlg.ShowModal() != wxID_OK)
    {
        return; // User cancelled
    }

    const std::string name = std::string(dlg.GetValue().mb_str());
    std::string err;
    if (!fs_.CreateDirectory(currentDir_, name, err)) // Trying to create the folder
    {
        ShowError("Failed to create folder:\n" + wxString(err));
        return;
    }

    SetStatusText("Created folder: " + wxString(name));
    RefreshListing(); // Updates the display
}

/**
 * Function: OnRename
 * Description: Prompts for a new name and renames the selected file/directory.
 * Parameters:
 *   event: command event 
 * Returns: None
 */
void FileManagerFrame::OnRename(wxCommandEvent& event)
{
    wxUnusedVar(event); // event not used

    FileSystemService::FileItem item{};
    if (!GetSelectedItem(item))
    {
        SetStatusText("Select an item to rename.");
        return;
    }
    if (item.name == "..") // Don't allow renaming the parent directory entry
    {
        SetStatusText("Cannot rename '..'.");
        return;
    }
    // Showing dialog with current name pre filled
    wxTextEntryDialog dlg(this, "Enter new name:", "Rename", wxString(item.name));
    if (dlg.ShowModal() != wxID_OK)
    {
        return; // User cancelled
    }

    const std::string newName = std::string(dlg.GetValue().mb_str());
    const std::filesystem::path oldP(item.fullPath); // Building the new full path
    const std::filesystem::path newP = oldP.parent_path() / std::filesystem::path(newName);

    std::string err;
    if (!fs_.RenamePath(item.fullPath, newP.string(), err))
    {
        ShowError("Rename failed:\n" + wxString(err));
        return;
    }

    SetStatusText("Renamed to: " + wxString(newName));
    RefreshListing();
}

/**
 * Function: OnDelete
 * Description: Prompts for confirmation and deletes the selected file/directory (directories are recursive).
 * Parameters:
 *   event: command event 
 * Returns: None
 */
void FileManagerFrame::OnDelete(wxCommandEvent& event)
{
    wxUnusedVar(event); // event not used

    FileSystemService::FileItem item{};
    if (!GetSelectedItem(item))
    {
        SetStatusText("Select an item to delete.");
        return;
    }
    // Can't delete the parent directory entry
    if (item.name == "..")
    {
        SetStatusText("Cannot delete '..'.");
        return;
    }
    if (!Confirm("Delete this item?\n" + wxString(item.fullPath))) // Asking for confirmation before deleting
    {
        return; // said no
    }

    std::string err;
    if (!fs_.DeletePath(item.fullPath, err))
    {
        ShowError("Delete failed:\n" + wxString(err));
        return;
    }

    SetStatusText("Deleted: " + wxString(item.name));
    RefreshListing();
}

/**
 * Function: OnCopy
 * Description: Marks the selected item in the virtual clipboard for a later Copy-Paste operation.
 * Parameters:
 *   event: command event 
 * Returns: None
 */
void FileManagerFrame::OnCopy(wxCommandEvent& event)
{
    wxUnusedVar(event); // event not used

    FileSystemService::FileItem item{};
    if (!GetSelectedItem(item) || item.name == "..")
    {
        SetStatusText("Select an item to copy.");
        return;
    }

    // Virtual clipboard is just an internal record of the source path and operation type.
    clipboard_.Set(item.fullPath, VirtualClipboard::Mode::Copy);
    SetStatusText("Copied to clipboard: " + wxString(item.fullPath));
}

/**
 * Function: OnCut
 * Description: Marks the selected item in the virtual clipboard for a later Cut-Paste (move) operation.
 * Parameters:
 *   event: command event 
 * Returns: None
 */
void FileManagerFrame::OnCut(wxCommandEvent& event)
{
    wxUnusedVar(event); // event not used

    FileSystemService::FileItem item{};
    if (!GetSelectedItem(item) || item.name == "..")
    {
        SetStatusText("Select an item to cut.");
        return;
    }

    // Save the file path for moving later.
    clipboard_.Set(item.fullPath, VirtualClipboard::Mode::Cut);
    SetStatusText("Cut to clipboard: " + wxString(item.fullPath));
}

/**
 * Function: OnPaste
 * Description: Completes a copy or cut operation by copying/moving the clipboard item into the current directory.
 *              If the destination already exists, prompts for overwrite.
 * Parameters:
 *   event: command event
 * Returns: None
 */
void FileManagerFrame::OnPaste(wxCommandEvent& event)
{
    wxUnusedVar(event); // event not used
    
    if (clipboard_.IsEmpty()) // Checking if clipboard has anything
    {
        SetStatusText("Clipboard is empty.");
        return;
    }

    const std::string source = clipboard_.GetPath();
    const std::filesystem::path srcP(source); // Figuring out the destination path
    const std::filesystem::path dstP = std::filesystem::path(currentDir_) / srcP.filename();
    // Checking if destination already exists
    const bool exists = fs_.Exists(dstP.string());
    bool overwrite = false;

    if (exists)
    {       //Asking if user wants to overwrite
        overwrite = Confirm("Destination exists:\n" + wxString(dstP.string()) + "\nOverwrite?");
        if (!overwrite)
        {
            SetStatusText("Paste cancelled.");
            return;
        }
    }

    std::string err;
    bool ok = false;
    // Do copy or move depending on what was in clipboard
    if (clipboard_.GetMode() == VirtualClipboard::Mode::Copy)
    {
        ok = fs_.CopyPathToDirectory(source, currentDir_, overwrite, err);
    }
    else if (clipboard_.GetMode() == VirtualClipboard::Mode::Cut)
    {
        ok = fs_.MovePathToDirectory(source, currentDir_, overwrite, err);
    }

    if (!ok)
    {
        ShowError("Paste failed:\n" + wxString(err));
        return;
    }
    // Clear the clipboard after successful paste
    clipboard_.Clear();
    SetStatusText("Paste complete. Clipboard is now empty.");
    RefreshListing();
}

/**
 * Function: OnRefresh
 * Description: Refreshes the directory listing from disk.
 * Parameters:
 *   event: command event 
 * Returns: None
 */
void FileManagerFrame::OnRefresh(wxCommandEvent& event)
{
    wxUnusedVar(event); // event not used

    RefreshListing(); // Just reload the current directory
    SetStatusText("Refreshed.");
}

/**
 * Function: OnExit
 * Description: Exits the application (closes the main window).
 * Parameters:
 *   event: command event 
 * Returns: None
 */
void FileManagerFrame::OnExit(wxCommandEvent& event)
{
    wxUnusedVar(event); // event not used

    Close(true);  // Close the window (and exit the app) 
}
