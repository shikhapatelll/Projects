#include "VirtualClipboard.h"

/**
 * Name: Shikha Patel
 * Description: Implementation for VirtualClipboard.
 * Date: jan 2026
 */

/**
 * Function: VirtualClipboard
 * Description: Constructs an empty clipboard with no pending operation.
 * Parameters: None
 * Returns: N/A
 */
VirtualClipboard::VirtualClipboard()
    : path_(""), mode_(Mode::None)
{ // Starting with empty clipboard
}

/**
 * Function: Set
 * Description: Stores a source path and operation mode (Copy or Cut) for a later paste.
 * Parameters:
 *   path: full path to the source item
 *   mode: Mode::Copy or Mode::Cut
 * Returns: None
 */
void VirtualClipboard::Set(const std::string& path, Mode mode)
{
        // Store both the source path and what the user intends to do with it.
    path_ = path;
    mode_ = mode;
}

/**
 * Function: Clear
 * Description: Clears the stored path and resets mode to None.
 * Parameters: None
 * Returns: None
 */
void VirtualClipboard::Clear()
{   // clearing out the clipboard
    path_.clear();
    mode_ = Mode::None;
}

/**
 * Function: IsEmpty
 * Description: Checks whether the clipboard currently has a pending Copy/Cut operation.
 * Parameters: None
 * Returns: true if empty; false otherwise.
 */
bool VirtualClipboard::IsEmpty() const
{   // Clipboard is empty if mode is None or path is empty
    return mode_ == Mode::None || path_.empty();
}

/**
 * Function: GetPath
 * Description: Returns the stored path in the clipboard.
 * Parameters: None
 * Returns: stored path string (may be empty).
 */
std::string VirtualClipboard::GetPath() const
{
    return path_;
}

/**
 * Function: GetMode
 * Description: Returns the clipboard operation mode (None/Copy/Cut).
 * Parameters: None
 * Returns: current clipboard mode.
 */
VirtualClipboard::Mode VirtualClipboard::GetMode() const
{
    return mode_;
}
