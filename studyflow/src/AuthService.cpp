#include "studyflow/AuthService.h"

#include <cstdint>
#include <iomanip>
#include <sstream>

namespace studyflow {

AuthService::AuthService(IRepository* repo) : repo_(repo) {}

std::string AuthService::hashPassword(const std::string& pw) {
    static const std::string salt = "studyflow_salt_v1";
    const std::string input = pw + salt;

    std::uint64_t h = 1469598103934665603ULL;
    for (unsigned char c : input) {
        h ^= static_cast<std::uint64_t>(c);
        h *= 1099511628211ULL;
    }

    std::ostringstream oss;
    oss << std::hex << std::setw(16) << std::setfill('0') << h;
    return oss.str();
}

bool AuthService::createAccount(const std::string& username, const std::string& pw, std::string* errMsg) {
    if (username.empty() || pw.empty()) {
        if (errMsg) *errMsg = "Username and password cannot be empty.";
        return false;
    }
    if (repo_->userExists(username)) {
        if (errMsg) *errMsg = "Error: username already exists. Nothing was overwritten.";
        return false;
    }

    repo_->storeCredentials(username, hashPassword(pw));
    UserData empty;
    repo_->saveUser(username, empty);
    return true;
}

bool AuthService::login(const std::string& username, const std::string& pw, std::string* errMsg) {
    if (!repo_->userExists(username)) {
        if (errMsg) *errMsg = "Login failed: incorrect username or password.";
        return false;
    }

    UserAccount user = repo_->loadUserAccount(username);
    if (user.passwordHash.empty()) {
        if (errMsg) *errMsg = "Login failed: credentials database error.";
        return false;
    }
    if (!user.checkPassword(pw)) {
        if (errMsg) *errMsg = "Login failed: incorrect username or password.";
        return false;
    }

    currentUser_ = std::make_unique<UserAccount>(std::move(user));
    return true;
}

void AuthService::logout() {
    currentUser_.reset();
}

std::string AuthService::currentUsername() const {
    return currentUser_ ? currentUser_->username : "";
}

UserAccount* AuthService::getCurrentUser() {
    return currentUser_.get();
}

} // namespace studyflow
