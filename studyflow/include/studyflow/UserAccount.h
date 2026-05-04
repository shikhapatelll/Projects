#pragma once

#include <string>

#include "UserData.h"

namespace studyflow {

/**
 * @brief Represents one user account loaded from the repository.
 *
 * The account stores the username, hashed password, and all associated study
 * data. The optional load warning is used by the UI to tell the user that a
 * corrupted JSON file was restored from backup or reset.
 *
 * @author Group 30
 */
class UserAccount {
public:
    std::string username;      /**< Username entered at signup and login. */
    std::string passwordHash;  /**< Persisted password hash. */
    UserData data;             /**< Full per-user planner data. */
    std::string loadWarning;   /**< Warning message to surface after recovery/reset. */

    /**
     * @brief Verify a plain-text password against the stored hash.
     *
     * @param pw Password entered by the user.
     * @return True when the password matches this account.
     *
     * @author Group 30
     */
    bool checkPassword(const std::string& pw) const;
};

} // namespace studyflow
