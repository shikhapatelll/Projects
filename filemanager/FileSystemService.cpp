#include "FileSystemService.h"
#include "DateUtil.h"
#include <system_error>

/**
 * Name: Shikha Patel
 * Description: Implementation of filesystem operations for the file manager.
 * Date: jan 2026, Last Modified: feb 2026
 */

/**
 * Function: ListDirectory
 * Description: Lists the contents of a directory and returns basic metadata for each entry.
 *              Uses std::filesystem with std::error_code to avoid throwing exceptions.
 * Parameters:
 *   dirPath: directory path to iterate
 * Returns: vector of FileItem entries (empty if directory is invalid or not accessible).
 */
std::vector<FileSystemService::FileItem> FileSystemService::ListDirectory(const std::string& dirPath)
{
    namespace fs = std::filesystem;
    std::vector<FileItem> items;

        // Use error_code throughout so we can fail gracefully without exceptions.
    std::error_code ec;
    fs::path p(dirPath);
    // First checking if the directory is valid
    if (!fs::exists(p, ec) || !fs::is_directory(p, ec))
    {
        return items;
    }
    // Loop through all files and folders in the directory
    // Iterate directory entries; if ec becomes set, we stop early.
    for (const auto& entry : fs::directory_iterator(p, ec))
    {
        if (ec)
        {
            break;
        }

        FileItem item{};
        // Getting the basic info
        item.fullPath = entry.path().string();
        item.name = entry.path().filename().string();
        // Checking if it's a directory
        item.isDirectory = entry.is_directory(ec);
        if (ec) { item.isDirectory = false; }

        // file_size is only defined for regular files; directories get size 0 
        if (!item.isDirectory)
        {
            std::error_code ecSize;
            item.sizeBytes = fs::file_size(entry.path(), ecSize);
            if (ecSize) { item.sizeBytes = 0; } // Setting to 0 if we can't get the size
        }
        else
        {
            item.sizeBytes = 0; // Directories don't have a size
        }

        std::error_code ecTime;
                // Getting last_write_time is platform-dependent; DateUtil wraps formatting.
         // Leaving blank if we can't get the time
        const auto ft = fs::last_write_time(entry.path(), ecTime);
        item.modified = ecTime ? "" : DateUtil::FormatFileTime(ft);

        items.push_back(item);
    }

    return items;
}

/**
 * Function: CreateDirectory
 * Description: Creates a new directory within a parent directory.
 * Parameters:
 *   parentPath: directory where the new folder should be created
 *   name: new folder name
 *   errorMsg: output parameter for a human-readable error message on failure
 * Returns: true if directory was created; false otherwise.
 */
bool FileSystemService::CreateDirectory(const std::string& parentPath, const std::string& name, std::string& errorMsg)
{
    namespace fs = std::filesystem;
    std::error_code ec;
    // Building the full path for the new directory
    fs::path newDir = fs::path(parentPath) / fs::path(name);
    //Checking if it already exists
    if (fs::exists(newDir, ec))
    {
        errorMsg = "Directory already exists.";
        return false;
    }
    // Trying to create it
    const bool ok = fs::create_directory(newDir, ec);
    if (!ok || ec)
    {   // when Something went wrong
        errorMsg = ec ? ec.message() : "Failed to create directory.";
        return false;
    }

    return true;
}

/**
 * Function: RenamePath
 * Description: Renames a file/directory by moving it from oldPath to newPath.
 * Parameters:
 *   oldPath: original full path
 *   newPath: new full path
 *   errorMsg: output parameter for a human-readable error message on failure
 * Returns: true if successful; false otherwise.
 */
bool FileSystemService::RenamePath(const std::string& oldPath, const std::string& newPath, std::string& errorMsg)
{
    namespace fs = std::filesystem;
    std::error_code ec;
    fs::rename(fs::path(oldPath), fs::path(newPath), ec);
    if (ec)
    {
        errorMsg = ec.message();
        return false;
    }
    return true;
}

/**
 * Function: DeletePath
 * Description: Deletes a file or directory. Directories are deleted recursively.
 * Parameters:
 *   targetPath: full path to delete
 *   errorMsg: output parameter for a human-readable error message on failure
 * Returns: true if deletion succeeded; false otherwise.
 */
bool FileSystemService::DeletePath(const std::string& targetPath, std::string& errorMsg)
{
    namespace fs = std::filesystem;
    std::error_code ec;
    fs::path p(targetPath);
    // Making sure it exists before trying to delete
    if (!fs::exists(p, ec))
    {
        errorMsg = "Path does not exist.";
        return false;
    }
    // For directories using remove_all (recursive delete)
    // For files using regular remove
    if (fs::is_directory(p, ec))
    {
        fs::remove_all(p, ec); // Deletes the directory and everything in i
    }
    else
    {
        fs::remove(p, ec); // Just delete the file
    }

    if (ec)
    {
        errorMsg = ec.message();
        return false;
    }

    return true;
}

/**
 * Function: RemoveIfExists
 * Description: Helper that removes an existing file/directory at a path (recursive for directories).
 * Parameters:
 *   p: filesystem path to remove
 *   errorMsg: output parameter for a human-readable error message on failure
 * Returns: true if removed (or did not exist); false on failure.
 */
bool FileSystemService::RemoveIfExists(const std::filesystem::path& p, std::string& errorMsg)
{
    // Helper function to remove something if it exists
    // Used before copying/moving when overwrite is true
    namespace fs = std::filesystem;
    std::error_code ec;
    if (fs::exists(p, ec))
    {   // Figuring out if it's a directory or file
        if (fs::is_directory(p, ec))
        {
            fs::remove_all(p, ec); // Remove directory recursively
        }
        else
        {
            fs::remove(p, ec); // Remove file
        }
        if (ec)
        {
            errorMsg = ec.message();
            return false;
        }
    }
    return true;
}

/**
 * Function: CopyPathToDirectory
 * Description: Copies a file or directory into a destination directory. Directories are copied recursively.
 *              If overwrite is true and the destination exists, it will be removed first.
 * Parameters:
 *   sourcePath: path to file/directory to copy
 *   destDir: destination directory that will receive the copy
 *   overwrite: whether to overwrite an existing destination
 *   errorMsg: output parameter for a human-readable error message on failure
 * Returns: true if copy succeeded; false otherwise.
 */
bool FileSystemService::CopyPathToDirectory(const std::string& sourcePath, const std::string& destDir, bool overwrite, std::string& errorMsg)
{
    namespace fs = std::filesystem;
    std::error_code ec;

    fs::path src(sourcePath);
    fs::path dstDir(destDir);
    fs::path dst = dstDir / src.filename(); // Destination path will have same filename as source
    // Checking that source exists
    if (!fs::exists(src, ec))
    {
        errorMsg = "Source does not exist.";
        return false;
    }
        // If destination exists, either stop (no overwrite) or remove it first.
    if (fs::exists(dst, ec))
    {
        if (!overwrite)
        {
            errorMsg = "Destination already exists.";
            return false;
        }
        if (!RemoveIfExists(dst, errorMsg)) // Removing the existing file/directory first
        {
            return false;
        }
    }
    //Now do the actual copy
    // Have to handle directories differently than files
    if (fs::is_directory(src, ec))
    {
        fs::copy(src, dst, fs::copy_options::recursive, ec); // Copying directory recursively
    }
    else
    {
        fs::copy_file(src, dst, fs::copy_options::overwrite_existing, ec); // Simple file copy
    }

    if (ec)
    {
        errorMsg = ec.message();
        return false;
    }

    return true;
}

/**
 * Function: MovePathToDirectory
 * Description: Moves a file/directory into a destination directory. Tries std::filesystem::rename first
 *              (fast), and falls back to copy+delete when rename fails (e.g., cross-device moves).
 * Parameters:
 *   sourcePath: path to file/directory to move
 *   destDir: destination directory
 *   overwrite: whether to overwrite an existing destination
 *   errorMsg: output parameter for a human-readable error message on failure
 * Returns: true if move succeeded; false otherwise.
 */
bool FileSystemService::MovePathToDirectory(const std::string& sourcePath, const std::string& destDir, bool overwrite, std::string& errorMsg)
{
    namespace fs = std::filesystem;
    std::error_code ec;

    fs::path src(sourcePath);
    fs::path dstDir(destDir);
    fs::path dst = dstDir / src.filename();

    if (!fs::exists(src, ec)) // Checking source exists
    {
        errorMsg = "Source does not exist.";
        return false;
    }

        // If destination exists, either stop (no overwrite) or remove it first.
    if (fs::exists(dst, ec))
    {
        if (!overwrite)
        {
            errorMsg = "Destination already exists.";
            return false;
        }
        if (!RemoveIfExists(dst, errorMsg))
        {
            return false;
        }
    }

    // Try rename first (fast). Rename can fail across filesystems/devices,
    // so we fall back to copy+delete in that case.
    fs::rename(src, dst, ec);
    if (!ec)
    {
        return true;
    }
    // Rename failed (probably cross-filesystem move) Fallback
    ec.clear();
    if (!CopyPathToDirectory(sourcePath, destDir, true, errorMsg))
    {
        return false;
    }
     // Copy succeeded, now delete the source
    if (!DeletePath(sourcePath, errorMsg))
    {
        return false;
    }
    return true;
}

/**
 * Function: Exists
 * Description: Checks whether a path exists on disk.
 * Parameters:
 *   path: filesystem path string
 * Returns: true if the path exists; false otherwise.
 */
bool FileSystemService::Exists(const std::string& path) const
{
    std::error_code ec;
    return std::filesystem::exists(std::filesystem::path(path), ec);
}

/**
 * Function: IsDirectory
 * Description: Checks whether a path refers to a directory.
 * Parameters:
 *   path: filesystem path string
 * Returns: true if the path is a directory; false otherwise.
 */
bool FileSystemService::IsDirectory(const std::string& path) const
{
    std::error_code ec;
    return std::filesystem::is_directory(std::filesystem::path(path), ec);
}
