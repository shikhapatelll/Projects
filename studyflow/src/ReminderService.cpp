#include "studyflow/ReminderService.h"

#include <sstream>

namespace studyflow {

std::vector<std::string> ReminderService::buildReminders(const std::vector<StudyItem>& items,
                                                         const Date& today) const {
    std::vector<std::string> out;

    for (const auto& item : items) {
        if (item.getRemainingHours() <= 0.0) continue;

        const int days = item.daysUntilDue(today);
        std::ostringstream oss;
        if (days < 0) {
            oss << "!!! OVERDUE !!!  " << item.course << " | " << item.title
                << " | due " << item.dueDate.toString();
            out.push_back(oss.str());
        } else if (days == 1 || days == 3 || days == 7) {
            oss << "Due in " << days << " day(s): " << item.course << " | "
                << item.title << " | due " << item.dueDate.toString();
            out.push_back(oss.str());
        }
    }

    return out;
}

} // namespace studyflow
