#ifndef FILESYSTEMSERVICE_H
#define FILESYSTEMSERVICE_H

#include <string>
#include <vector>
#include <filesystem>

/**
 * Name: Shikha Patel
 * Description: Provides directory listing and file operations via C++17 filesystem.
 * Date: Jan 2026, Last Modified: Feb 2026
 */
// This class wraps all the filesystem operations that the file manager needs.

class FileSystemService
{
public:
    // Simple struct to hold info about a file or directory
    struct FileItem
    {
        std::string name;           // Just the filename
        std::string fullPath;       // Complete path to the file
        bool isDirectory;           // True if it's a folder
        std::uintmax_t sizeBytes;   // File size (0 for directories)
        std::string modified;       // Last modified time as string
    };

    /**
     * Function: ListDirectory
     * Description: Lists items in a directory.
     * Parameters:
     *   dirPath: directory to list
     * Returns: vector of FileItem entries (excluding "."; may include ".." if requested by caller).
     */
    std::vector<FileItem> ListDirectory(const std::string& dirPath);

    /**
     * Function: CreateDirectory
     * Description: Creates a new directory inside parentPath.
     * Parameters:
     *   parentPath: parent directory
     *   name: new directory name
     * Returns: true on success, false otherwise.
     */
    bool CreateDirectory(const std::string& parentPath, const std::string& name, std::string& errorMsg);

    /**
     * Function: RenamePath
     * Description: Renames a file/directory within the same parent folder.
     * Parameters:
     *   oldPath: existing full path
     *   newPath: new full path
     * Returns: true on success, false otherwise.
     */
    bool RenamePath(const std::string& oldPath, const std::string& newPath, std::string& errorMsg);

    /**
     * Function: DeletePath
     * Description: Deletes file or directory (recursive for directory).
     * Parameters:
     *   targetPath: path to remove
     * Returns: true on success, false otherwise.
     */
    bool DeletePath(const std::string& targetPath, std::string& errorMsg);

    /**
     * Function: CopyPathToDirectory
     * Description: Copies a file or directory into destination directory.
     * Parameters:
     *   sourcePath: file/dir to copy
     *   destDir: destination directory
     *   overwrite: whether to overwrite existing
     * Returns: true on success, false otherwise.
     */
    bool CopyPathToDirectory(const std::string& sourcePath, const std::string& destDir, bool overwrite, std::string& errorMsg);

    /**
     * Function: MovePathToDirectory
     * Description: Moves a file or directory into destination directory.
     * Parameters:
     *   sourcePath: file/dir to move
     *   destDir: destination directory
     *   overwrite: whether to overwrite existing
     * Returns: true on success, false otherwise.
     */
    bool MovePathToDirectory(const std::string& sourcePath, const std::string& destDir, bool overwrite, std::string& errorMsg);

    /**
     * Function: Exists
     * Description: Checks if a path exists.
     * Parameters:
     *   path: filesystem path
     * Returns: true if exists, false otherwise.
     */
    bool Exists(const std::string& path) const;

    /**
     * Function: IsDirectory
     * Description: Checks if path is a directory.
     * Parameters:
     *   path: filesystem path
     * Returns: true if directory, false otherwise.
     */
    bool IsDirectory(const std::string& path) const;

private:
// Helper method to remove file/dir if it exists
    bool RemoveIfExists(const std::filesystem::path& p, std::string& errorMsg);
};

#endif
