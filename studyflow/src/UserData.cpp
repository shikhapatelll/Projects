#include "studyflow/UserData.h"

namespace studyflow {

StudyItem* UserData::findItem(int itemId) {
    for (auto& item : items) {
        if (item.id == itemId) return &item;
    }
    return nullptr;
}

StudySession* UserData::findSession(int sessionId) {
    for (auto& session : sessions) {
        if (session.id == sessionId) return &session;
    }
    return nullptr;
}

} // namespace studyflow
