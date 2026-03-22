#ifndef VIRTUALCLIPBOARD_H
#define VIRTUALCLIPBOARD_H

#include <string>

/**
 * Name: Shikha Patel
 * Description: Stores a single path and an operation mode (copy/cut) for paste.
 * Date: jan 2026
 */
// stores a file path and whether it's a copy or cut operation
class VirtualClipboard
{
public:
// Mode enum, if we're copying or cutting
    enum class Mode
    {
        None,   // Nothing in clipboard
        Copy,   // Copy operation
        Cut     // Cut (move) operation
    };

    /**
     * Function: VirtualClipboard
     * Description: Constructs an empty clipboard.
     * Parameters: None
     * Returns: N/A
     */
    VirtualClipboard();

    /**
     * Function: Set
     * Description: Stores a source path and mode (copy or cut).
     * Parameters:
     *   path: source filesystem path
     *   mode: copy or cut
     * Returns: None
     */
    void Set(const std::string& path, Mode mode);

    /**
     * Function: Clear
     * Description: Clears clipboard contents.
     * Parameters: None
     * Returns: None
     */
    void Clear();

    /**
     * Function: IsEmpty
     * Description: Checks if clipboard is empty.
     * Parameters: None
     * Returns: true if empty, false otherwise.
     */
    bool IsEmpty() const;

    /**
     * Function: GetPath
     * Description: Returns stored source path.
     * Parameters: None
     * Returns: stored path string.
     */
    std::string GetPath() const;

    /**
     * Function: GetMode
     * Description: Returns clipboard mode.
     * Parameters: None
     * Returns: Mode enum value.
     */
    Mode GetMode() const;

private:
    std::string path_;
    Mode mode_;
};

#endif
