#include "studyflow/UserAccount.h"

#include "studyflow/AuthService.h"

namespace studyflow {

bool UserAccount::checkPassword(const std::string& pw) const {
    return AuthService::hashPassword(pw) == passwordHash;
}

} // namespace studyflow
