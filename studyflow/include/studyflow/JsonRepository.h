#pragma once

#include <string>

#include "IRepository.h"
#include "SimpleJson.h"

namespace studyflow {

/**
 * @brief JSON-backed file repository for StudyFlow accounts and planner data.
 *
 * The repository stores credentials in `data/credentials.json` and each user's
 * planner data in `data/users/<username>.json`. It uses atomic writes plus
 * backup files to avoid overwriting good data with corrupted saves.
 *
 * @author Group 30
 */
class JsonRepository : public IRepository {
public:
    /**
     * @brief Construct a JSON repository rooted at a base data directory.
     *
     * @param baseDir Base directory that contains credentials and user files.
     *
     * @author Group 30
     */
    explicit JsonRepository(std::string baseDir = "data");

    /**
     * @brief Check whether a username already exists.
     *
     * @param username Username to look up.
     * @return True if the account exists in the credentials file.
     *
     * @author Group 30
     */
    bool userExists(const std::string& username) const override;

    /**
     * @brief Save a username and password hash into the credentials store.
     *
     * @param username Username to save.
     * @param passwordHash Hashed password string.
     *
     * @author Group 30
     */
    void storeCredentials(const std::string& username, const std::string& passwordHash) override;

    /**
     * @brief Load a stored password hash.
     *
     * @param username Username to load.
     * @return Stored password hash or an empty string when absent.
     *
     * @author Group 30
     */
    std::string loadCredentials(const std::string& username) const override;

    /**
     * @brief Save one user's StudyFlow planner data.
     *
     * @param username Username whose file should be written.
     * @param data Planner data to serialize.
     *
     * @author Group 30
     */
    void saveUser(const std::string& username, const UserData& data) override;

    /**
     * @brief Load only the persisted UserData payload for a username.
     *
     * @param username Username whose data should be loaded.
     * @return Loaded planner state.
     *
     * @author Group 30
     */
    UserData loadUser(const std::string& username) const override;

    /**
     * @brief Load the full user account object.
     *
     * @param username Username to load.
     * @return User account including credentials, data, and recovery warning.
     *
     * @author Group 30
     */
    UserAccount loadUserAccount(const std::string& username) const override;

private:
    std::string baseDir_; /**< Root data directory used by the repository. */

    /**
     * @brief Build the credentials file path.
     *
     * @return Full path to `credentials.json`.
     *
     * @author Group 30
     */
    std::string credentialsPath_() const;

    /**
     * @brief Build the file path for one user's JSON data.
     *
     * @param username Username whose path is needed.
     * @return Full path to the user's JSON file.
     *
     * @author Group 30
     */
    std::string userPath_(const std::string& username) const;

    /**
     * @brief Convert a username into a safe filename component.
     *
     * @param username Raw username.
     * @return Sanitized filename-safe string.
     *
     * @author Group 30
     */
    static std::string sanitizeUsername_(const std::string& username);

    /**
     * @brief Read an entire text file into memory.
     *
     * @param path Path to read.
     * @return File contents as a string.
     *
     * @author Group 30
     */
    static std::string readFile_(const std::string& path);

    /**
     * @brief Write a file atomically using a temporary file and backup.
     *
     * @param path Final destination path.
     * @param content Text to write.
     *
     * @author Group 30
     */
    static void writeAtomic_(const std::string& path, const std::string& content);

    /**
     * @brief Load JSON from a file while reporting parse success.
     *
     * @param path File path to parse.
     * @param ok Output flag set to true on successful parse.
     * @return Parsed Json value or a null/empty fallback.
     *
     * @author Group 30
     */
    static Json loadJsonSafe_(const std::string& path, bool& ok);

    /**
     * @brief Ensure that the repository directory structure exists.
     *
     * @param baseDir Base data directory.
     *
     * @author Group 30
     */
    static void ensureDirs_(const std::string& baseDir);

    /**
     * @brief Serialize UserData into the internal Json tree format.
     *
     * @param data Planner state to serialize.
     * @return Json object representation.
     *
     * @author Group 30
     */
    static Json userDataToJson_(const UserData& data);

    /**
     * @brief Deserialize UserData from the internal Json tree format.
     *
     * @param j Json value to interpret.
     * @return Reconstructed UserData object.
     *
     * @author Group 30
     */
    static UserData userDataFromJson_(const Json& j);

    /**
     * @brief Load the credentials JSON file with success reporting.
     *
     * @param path Credentials file path.
     * @param ok Output flag set to true on successful parse.
     * @return Parsed credentials Json object.
     *
     * @author Group 30
     */
    static Json loadCredsJson_(const std::string& path, bool& ok);

    /**
     * @brief Extract one user's password hash from the credentials object.
     *
     * @param creds Parsed credentials object.
     * @param username Username to look up.
     * @return Stored hash or an empty string when missing.
     *
     * @author Group 30
     */
    static std::string getCredHash_(const Json& creds, const std::string& username);

    /**
     * @brief Insert or update one user's password hash in the credentials object.
     *
     * @param creds Existing credentials object.
     * @param username Username to update.
     * @param hash Password hash to store.
     * @return Updated credentials Json object.
     *
     * @author Group 30
     */
    static Json setCredHash_(Json creds, const std::string& username, const std::string& hash);

    /**
     * @brief Load user data and capture any recovery warning message.
     *
     * @param username Username to load.
     * @param warning Output string for recovery/reset warnings.
     * @return Loaded user data.
     *
     * @author Group 30
     */
    UserData loadUserWithWarning_(const std::string& username, std::string* warning) const;
};

} // namespace studyflow
