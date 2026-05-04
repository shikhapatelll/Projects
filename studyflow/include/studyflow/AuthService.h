#pragma once

#include <memory>
#include <string>

#include "IRepository.h"

namespace studyflow {

/**
 * @brief Handles signup, login, logout, and the current in-memory session.
 *
 * AuthService is responsible for validating credentials against the repository,
 * creating new accounts, hashing passwords consistently, and exposing the
 * currently logged-in user to the rest of the application.
 *
 * @author Group 30
 */
class AuthService {
public:
    /**
     * @brief Construct the authentication service.
     *
     * The service keeps a non-owning pointer to the repository used for
     * loading and storing account data.
     *
     * @param repo Repository implementation used for persistence.
     *
     * @author Group 30
     */
    explicit AuthService(IRepository* repo);

    /**
     * @brief Create a new StudyFlow account.
     *
     * This method validates the username and password, checks that the user
     * does not already exist, hashes the password, and initializes empty user
     * data in persistent storage.
     *
     * @param username Desired username.
     * @param pw Plain-text password entered by the user.
     * @param errMsg Optional output parameter for a human-readable error.
     * @return True when the account is created successfully.
     *
     * @author Group 30
     */
    bool createAccount(const std::string& username, const std::string& pw, std::string* errMsg = nullptr);

    /**
     * @brief Log in using an existing username and password.
     *
     * On success the current in-memory session is replaced with the loaded user
     * account and associated persisted planner data.
     *
     * @param username Account username.
     * @param pw Plain-text password entered by the user.
     * @param errMsg Optional output parameter for a human-readable error.
     * @return True when the credentials are correct and the account is loaded.
     *
     * @author Group 30
     */
    bool login(const std::string& username, const std::string& pw, std::string* errMsg = nullptr);

    /**
     * @brief Clear the current logged-in session.
     *
     * This only affects in-memory state. Persisted user data remains on disk.
     *
     * @author Group 30
     */
    void logout();

    /**
     * @brief Get the username of the current session.
     *
     * @return Logged-in username, or an empty string when nobody is logged in.
     *
     * @author Group 30
     */
    std::string currentUsername() const;

    /**
     * @brief Access the current logged-in account object.
     *
     * @return Pointer to the active UserAccount, or nullptr when logged out.
     *
     * @author Group 30
     */
    UserAccount* getCurrentUser();

    /**
     * @brief Hash a password into the stored representation used by StudyFlow.
     *
     * The project uses a simple deterministic hash helper appropriate for a
     * course project. It is not intended as production-grade cryptography.
     *
     * @param pw Plain-text password.
     * @return Hashed password string.
     *
     * @author Group 30
     */
    static std::string hashPassword(const std::string& pw);

private:
    IRepository* repo_{nullptr};                   /**< Repository used for persistence. */
    std::unique_ptr<UserAccount> currentUser_;     /**< Active in-memory session, if any. */
};

} // namespace studyflow
