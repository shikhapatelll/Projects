#include "studyflow/StudyItem.h"

namespace studyflow {

bool StudyItem::isOverdue(const Date& today) const {
    return dueDate < today;
}

int StudyItem::daysUntilDue(const Date& today) const {
    return today.daysUntil(dueDate);
}

void StudyItem::updateProgress(double hours) {
    if (hours <= 0.0) return;
    hoursCompleted += hours;
    if (hoursCompleted > estimatedHours) {
        hoursCompleted = estimatedHours;
    }
}

double StudyItem::getRemainingHours() const {
    const double remaining = estimatedHours - hoursCompleted;
    return remaining < 0.0 ? 0.0 : remaining;
}

} // namespace studyflow
