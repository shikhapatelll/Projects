#include "studyflow/StudySession.h"

namespace studyflow {

void StudySession::start() {
    if (status == SessionStatus::Planned) {
        status = SessionStatus::InProgress;
    }
}

void StudySession::complete() {
    status = SessionStatus::Completed;
}

} // namespace studyflow
