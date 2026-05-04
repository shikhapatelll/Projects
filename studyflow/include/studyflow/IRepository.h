#pragma once

#include <string>

#include "UserAccount.h"

namespace studyflow {

/**
 * @brief Abstract persistence interface for credentials and per-user data.
 *
 * The rest of the application depends on this interface rather than on a
 * specific storage implementation. For this project, JsonRepository is the
 * concrete implementation used by the console application.
 *
 * @author Group 30
 */
class IRepository {
public:
    /**
     * @brief Destroy the repository interface polymorphically.
     *
     * @author Group 30
     */
    virtual ~IRepository() = default;

    /**
     * @brief Check whether a username already exists in persistent storage.
     *
     * @param username Username to look up.
     * @return True if the account exists.
     *
     * @author Group 30
     */
    virtual bool userExists(const std::string& username) const = 0;

    /**
     * @brief Store hashed credentials for a user.
     *
     * @param username Username whose credentials are being saved.
     * @param passwordHash Hashed password string.
     *
     * @author Group 30
     */
    virtual void storeCredentials(const std::string& username, const std::string& passwordHash) = 0;

    /**
     * @brief Load the stored password hash for a user.
     *
     * @param username Username to load.
     * @return Stored password hash, or an empty string when missing.
     *
     * @author Group 30
     */
    virtual std::string loadCredentials(const std::string& username) const = 0;

    /**
     * @brief Save all planner data for one user.
     *
     * @param username Username whose planner state is being written.
     * @param data Planner data to persist.
     *
     * @author Group 30
     */
    virtual void saveUser(const std::string& username, const UserData& data) = 0;

    /**
     * @brief Load persisted planner data for one user.
     *
     * @param username Username whose planner state is being loaded.
     * @return Loaded user data.
     *
     * @author Group 30
     */
    virtual UserData loadUser(const std::string& username) const = 0;

    /**
     * @brief Load the full account object for one user.
     *
     * This includes username, hashed password, planner data, and any recovery
     * warning that should be displayed by the UI.
     *
     * @param username Username to load.
     * @return Fully populated user account object.
     *
     * @author Group 30
     */
    virtual UserAccount loadUserAccount(const std::string& username) const = 0;
};

} // namespace studyflow
