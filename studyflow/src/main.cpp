#include <algorithm>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "studyflow/AuthService.h"
#include "studyflow/CommandParser.h"
#include "studyflow/Date.h"
#include "studyflow/JsonRepository.h"
#include "studyflow/PlannerService.h"
#include "studyflow/Types.h"

using namespace studyflow;

/**
 * @brief Read the current local calendar date from the operating system clock.
 *
 * @return Today's local date.
 *
 * @author Group 30
 */
static Date todayLocal() {
    std::time_t t = std::time(nullptr);
    std::tm localTime{};
#if defined(_WIN32)
    localtime_s(&localTime, &t);
#else
    localtime_r(&t, &localTime);
#endif
    return Date(localTime.tm_year + 1900, localTime.tm_mon + 1, localTime.tm_mday);
}

/**
 * @brief Format hour values without unnecessary trailing zeroes.
 *
 * @param hours Numeric hours value.
 * @return Compact human-readable text.
 *
 * @author Group 30
 */
static std::string formatHours(double hours) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << hours;
    std::string out = oss.str();
    while (!out.empty() && out.back() == '0') out.pop_back();
    if (!out.empty() && out.back() == '.') out.pop_back();
    if (out.empty()) out = "0";
    return out;
}

/**
 * @brief Print the in-application help screen.
 *
 * @author Group 30
 */
static void printHelp() {
    std::cout << R"(StudyFlow Help (US17)

Core Commands:
  signup <username> <password>
  login <username> <password>
  logout
  exit

Item management (US4/7/8):
  add --title "..." --type exam|assignment|reading|task|quiz --course CS3307 --due YYYY-MM-DD --hours 5 --priority 3 [--desc "..."]
  edit <itemId> [--title "..."] [--type ...] [--course ...] [--due YYYY-MM-DD] [--hours N] [--priority 1..5] [--desc "..."]
  delete <itemId>

Viewing / search / filter (US5):
  list
  search --query "text"
  filter [--type exam] [--priority 3] [--course CS3307] [--from YYYY-MM-DD] [--to YYYY-MM-DD]
  Notes:
    - list/search/filter default-sort by due date.
    - overdue items are marked [OVERDUE].

Today/Week views (US6) + completion (US9):
  history
  history [--from YYYY-MM-DD] [--to YYYY-MM-DD]
  today
  week
  start-session <sessionId>
  complete <sessionId>

Extra views (US19/US20):
  left        (show only incomplete items)
  priority    (show incomplete items sorted by priority desc)

Scheduling / settings (US13/US14/US15):
  settings
  setsession <hours>      (example: setsession 0.5)
  setmaxhours <hours>     (0..24)
  setweekends on|off
  regen

Analytics (US16):
  analytics

Reminders (US10):
  reminders   (also shown automatically after successful login)

Save/Load (US11/US12):
  save        (manual save; the app also auto-saves on changes)
  load        (manual reload; login loads automatically)

Scheduling logic ("Smarter Scheduling"):
  - Remaining item hours are split into sessions of the chosen session length.
  - Sessions are scheduled starting from today up to each item's due date.
  - Per-day total scheduled time respects your daily maximum hours.
  - If weekends are disabled, Saturday/Sunday are skipped.
  - Regenerating the plan replaces old planned sessions so duplicates are not left behind.
  - If there is not enough time before deadlines, the app warns and still builds the best plan it can.
)";
}

/**
 * @brief Print a formatted list of study items.
 *
 * @param items Items to display.
 * @param today Reference date used for overdue markers.
 *
 * @author Group 30
 */
static void printItems(const std::vector<StudyItem>& items, const Date& today) {
    if (items.empty()) {
        std::cout << "(No items)\n";
        return;
    }

    for (const auto& item : items) {
        std::string desc = item.description.empty() ? "(no desc)" : item.description;
        const std::size_t maxDesc = 40;
        if (desc.size() > maxDesc) {
            desc = desc.substr(0, maxDesc) + "...";
        }

        std::cout << "Item " << item.id
                  << " | " << toString(item.type)
                  << " | " << item.course
                  << " | " << item.title
                  << " | due " << item.dueDate.toString()
                  << " | hours " << formatHours(item.estimatedHours)
                  << " | priority " << item.priority
                  << " | desc: " << desc;

        if (item.isOverdue(today) && item.getRemainingHours() > 0.0) {
            std::cout << "  [OVERDUE]";
        }
        std::cout << "\n";
    }
}

/**
 * @brief Print a formatted list of study sessions.
 *
 * @param sessions Sessions to display.
 * @param items Item list used to resolve session titles and metadata.
 * @param emptyMessage Message shown when no sessions are available.
 *
 * @author Group 30
 */
static void printSessions(const std::vector<StudySession>& sessions,
                          const std::vector<StudyItem>& items,
                          const std::string& emptyMessage = "No tasks for today!") {
    auto findItem = [&](int id) -> const StudyItem* {
        for (const auto& item : items) {
            if (item.id == id) return &item;
        }
        return nullptr;
    };

    if (sessions.empty()) {
        std::cout << emptyMessage << "\n";
        return;
    }

    for (const auto& session : sessions) {
        const StudyItem* item = findItem(session.itemId);
        if (!item) continue;

        std::cout << "Session " << session.id
                  << " | " << session.scheduledDate.toString()
                  << " | " << item->title
                  << " | duration " << formatHours(session.durationHours) << "h"
                  << " | due " << item->dueDate.toString()
                  << " | priority " << item->priority
                  << " | status " << toString(session.status)
                  << "\n";
    }
}

/**
 * @brief Require that a user is logged in before a command proceeds.
 *
 * @param auth Authentication service.
 * @return True when a session is active.
 *
 * @author Group 30
 */
static bool requireLogin(AuthService& auth) {
    if (!auth.getCurrentUser()) {
        std::cout << "You must login first.\n";
        return false;
    }
    return true;
}

/**
 * @brief Build a StudyItem from parsed command options.
 *
 * Missing fields are left in their default state so the same helper can be
 * used for full inserts and partial edits.
 *
 * @param opts Parsed named command options.
 * @param err Optional output parameter for validation or parsing errors.
 * @return Partially or fully populated StudyItem.
 *
 * @author Group 30
 */
static StudyItem buildItemFromOpts(const std::map<std::string, std::string>& opts, std::string* err) {
    StudyItem item;
    item.priority = 0;

    auto get = [&](const std::string& key) -> std::string {
        const auto it = opts.find(key);
        return (it == opts.end()) ? "" : it->second;
    };

    item.title = get("title");
    item.course = get("course");
    item.description = get("desc");
    item.type = itemTypeFromString(get("type"));

    const std::string due = get("due");
    if (!due.empty()) {
        try {
            item.dueDate = Date::parse(due);
        } catch (const std::exception& e) {
            if (err) *err = e.what();
        }
    } else {
        item.dueDate = Date();
    }

    const std::string hours = get("hours");
    if (!hours.empty()) {
        try {
            item.estimatedHours = std::stod(hours);
        } catch (...) {
            if (err) *err = "Invalid hours";
        }
        if (item.estimatedHours <= 0.0) {
            if (err) *err = "Validation error: hours must be positive.";
        }
    }

    const std::string priority = get("priority");
    if (!priority.empty()) {
        try {
            item.priority = std::stoi(priority);
        } catch (...) {
            if (err) *err = "Invalid priority";
        }
        if (item.priority < 1 || item.priority > 5) {
            if (err) *err = "Validation error: priority must be 1..5.";
        }
    }

    return item;
}

/**
 * @brief Print the current scheduling settings for the active user.
 *
 * @param planner Planner service bound to the logged-in user.
 *
 * @author Group 30
 */
static void printSettings(const PlannerService& planner) {
    std::string err;
    const auto* settings = planner.getSettings(&err);
    if (!settings) {
        std::cout << err << "\n";
        return;
    }

    std::cout << "Daily max hours: " << formatHours(settings->dailyMaxHours) << "\n"
              << "Session length: " << formatHours(settings->sessionLengthHours) << "h\n"
              << "Include weekends: " << (settings->includeWeekends ? "Yes" : "No") << "\n";
}

/**
 * @brief Print streak, weekly summary, and per-course completion analytics.
 *
 * @param planner Planner service bound to the logged-in user.
 * @param today Reference date used for rolling-window analytics.
 *
 * @author Group 30
 */
static void printAnalytics(const PlannerService& planner, const Date& today) {
    const int streak = planner.getStreakDays(today);
    const WeeklySummary summary = planner.getWeeklySummary(today, 7);
    const auto courseRates = planner.getCourseCompletionRates();

    std::cout << "Streak Days: " << streak << "\n";
    std::cout << "Weekly Summary (last 7 days): completed "
              << formatHours(summary.hoursCompleted)
              << "h / planned "
              << formatHours(summary.hoursPlanned)
              << "h\n";

    if (courseRates.empty()) {
        std::cout << "Course Completion Rates: (no items)\n";
        return;
    }

    std::cout << "Course Completion Rates:\n";
    for (const auto& row : courseRates) {
        std::cout << "  - " << row.course
                  << ": " << formatHours(row.completionRate) << "%"
                  << " (" << formatHours(row.completedHours)
                  << "h / " << formatHours(row.estimatedHours) << "h)\n";
    }
}

/**
 * @brief Entry point for the console version of StudyFlow.
 *
 * The main loop reads text commands, dispatches them to the appropriate
 * services, and prints results back to the terminal.
 *
 * @return Exit status code.
 *
 * @author Group 30
 */
int main() {
    JsonRepository repo("data");
    AuthService auth(&repo);
    PlannerService planner(&repo);
    CommandParser parser;

    std::cout << "StudyFlow (console) — type 'help' for commands.\n";

    while (true) {
        std::cout << "\n> ";
        std::string line;
        if (!std::getline(std::cin, line)) break;

        const Command cmd = parser.parse(line);
        if (cmd.name.empty()) continue;

        const Date today = todayLocal();

        if (cmd.name == "exit" || cmd.name == "quit") {
            std::cout << "Goodbye.\n";
            break;
        }
        if (cmd.name == "help") {
            printHelp();
            continue;
        }

        if (cmd.name == "signup") {
            if (cmd.args.size() < 2) {
                std::cout << "Usage: signup <username> <password>\n";
                continue;
            }
            std::string err;
            if (auth.createAccount(cmd.args[0], cmd.args[1], &err)) {
                std::cout << "Account created.\n";
            } else {
                std::cout << err << "\n";
            }
            continue;
        }

        if (cmd.name == "login") {
            if (cmd.args.size() < 2) {
                std::cout << "Usage: login <username> <password>\n";
                continue;
            }
            std::string err;
            if (!auth.login(cmd.args[0], cmd.args[1], &err)) {
                std::cout << err << "\n";
                continue;
            }

            std::cout << "Login successful.\n";
            planner.bindUser(auth.currentUsername(), &auth.getCurrentUser()->data);

            if (!auth.getCurrentUser()->loadWarning.empty()) {
                std::cout << auth.getCurrentUser()->loadWarning << "\n";
            }

            const auto reminders = planner.getReminders(today);
            if (!reminders.empty()) {
                std::cout << "\nReminders:\n";
                for (const auto& reminder : reminders) {
                    std::cout << "  - " << reminder << "\n";
                }
            }

            std::vector<std::string> warnings;
            planner.regeneratePlan(today, &warnings);
            for (const auto& warning : warnings) {
                std::cout << warning << "\n";
            }
            continue;
        }

        if (cmd.name == "logout") {
            if (!auth.getCurrentUser()) {
                std::cout << "Not logged in.\n";
                continue;
            }
            planner.unbindUser();
            auth.logout();
            std::cout << "Logged out successfully.\n";
            continue;
        }

        if (!requireLogin(auth)) continue;

        if (cmd.name == "add") {
            std::string err;
            StudyItem item = buildItemFromOpts(cmd.opts, &err);
            if (!err.empty()) {
                std::cout << "Error: " << err << "\n";
                continue;
            }
            if (item.dueDate == Date()) {
                std::cout << "Validation error: due date is required.\n";
                continue;
            }

            std::string addErr;
            const int id = planner.addItem(item, &addErr);
            if (id < 0) {
                std::cout << addErr << "\n";
            } else {
                std::cout << "Item added with id " << id << ".\n";
                std::vector<std::string> warnings;
                planner.regeneratePlan(today, &warnings);
                for (const auto& warning : warnings) {
                    std::cout << warning << "\n";
                }
            }
            continue;
        }

        if (cmd.name == "list") {
            printItems(planner.getAllItemsSortedByDue(), today);
            continue;
        }

        if (cmd.name == "search") {
            const auto it = cmd.opts.find("query");
            if (it == cmd.opts.end()) {
                std::cout << "Usage: search --query \"text\"\n";
                continue;
            }
            printItems(planner.searchItems(it->second), today);
            continue;
        }

        if (cmd.name == "filter") {
            FilterCriteria criteria;
            if (cmd.opts.count("type")) criteria.type = itemTypeFromString(cmd.opts.at("type"));
            if (cmd.opts.count("priority")) {
                try {
                    criteria.priority = std::stoi(cmd.opts.at("priority"));
                } catch (...) {
                    std::cout << "Validation error: priority must be a number.\n";
                    continue;
                }
            }
            if (cmd.opts.count("course")) criteria.course = cmd.opts.at("course");
            if (cmd.opts.count("from")) {
                try {
                    criteria.dueFrom = Date::parse(cmd.opts.at("from"));
                } catch (...) {
                    std::cout << "Validation error: --from must be a valid date YYYY-MM-DD.\n";
                    continue;
                }
            }
            if (cmd.opts.count("to")) {
                try {
                    criteria.dueTo = Date::parse(cmd.opts.at("to"));
                } catch (...) {
                    std::cout << "Validation error: --to must be a valid date YYYY-MM-DD.\n";
                    continue;
                }
            }
            printItems(planner.filterItems(criteria), today);
            continue;
        }

        if (cmd.name == "edit") {
            if (cmd.args.empty()) {
                std::cout << "Usage: edit <itemId> [--fields]\n";
                continue;
            }

            int id = 0;
            try {
                id = std::stoi(cmd.args[0]);
            } catch (...) {
                std::cout << "Usage: edit <itemId> [--fields]  (itemId must be a number)\n";
                continue;
            }

            const auto titleIt = cmd.opts.find("title");
            if (titleIt != cmd.opts.end() && titleIt->second.empty()) {
                std::cout << "Validation error: title cannot be empty.\n";
                continue;
            }
            const auto courseIt = cmd.opts.find("course");
            if (courseIt != cmd.opts.end() && courseIt->second.empty()) {
                std::cout << "Validation error: course cannot be empty.\n";
                continue;
            }

            std::string err;
            const StudyItem updates = buildItemFromOpts(cmd.opts, &err);
            if (!err.empty()) {
                std::cout << "Error: " << err << "\n";
                continue;
            }

            std::string msg;
            const bool ok = planner.editItem(id, updates, &msg);
            if (!ok) {
                std::cout << msg << "\n";
            } else {
                if (!msg.empty()) std::cout << msg << "\n";
                std::cout << "Item updated.\n";
                std::vector<std::string> warnings;
                planner.regeneratePlan(today, &warnings);
                for (const auto& warning : warnings) {
                    std::cout << warning << "\n";
                }
            }
            continue;
        }

        if (cmd.name == "delete") {
            if (cmd.args.empty()) {
                std::cout << "Usage: delete <itemId>\n";
                continue;
            }

            int id = 0;
            try {
                id = std::stoi(cmd.args[0]);
            } catch (...) {
                std::cout << "Usage: delete <itemId>  (itemId must be a number)\n";
                continue;
            }

            std::string err;
            if (planner.deleteItem(id, &err)) {
                std::cout << "Item deleted.\n";
                std::vector<std::string> warnings;
                planner.regeneratePlan(today, &warnings);
                for (const auto& warning : warnings) {
                    std::cout << warning << "\n";
                }
            } else {
                std::cout << err << "\n";
            }
            continue;
        }

        if (cmd.name == "history") {
            Date from = today.addDays(-6);
            Date to = today;

            const auto fromIt = cmd.opts.find("from");
            if (fromIt != cmd.opts.end() && !fromIt->second.empty()) {
                try {
                    from = Date::parse(fromIt->second);
                } catch (...) {
                    std::cout << "Usage: history [--from YYYY-MM-DD] [--to YYYY-MM-DD]\n";
                    continue;
                }
            }

            const auto toIt = cmd.opts.find("to");
            if (toIt != cmd.opts.end() && !toIt->second.empty()) {
                try {
                    to = Date::parse(toIt->second);
                } catch (...) {
                    std::cout << "Usage: history [--from YYYY-MM-DD] [--to YYYY-MM-DD]\n";
                    continue;
                }
            }

            if (to < from) {
                std::cout << "Error: --to must be on/after --from.\n";
                continue;
            }

            const auto sessions = planner.getHistory(from, to);
            printSessions(sessions, planner.getAllItemsSortedByDue(), "(No completed sessions)");
            continue;
        }

        if (cmd.name == "today") {
            printSessions(planner.getTodayView(today), planner.getAllItemsSortedByDue());
            continue;
        }

        if (cmd.name == "week") {
            printSessions(planner.getWeekView(today, 7), planner.getAllItemsSortedByDue());
            continue;
        }

        if (cmd.name == "left") {
            const auto all = planner.getAllItemsSortedByDue();
            std::vector<StudyItem> left;
            for (const auto& item : all) {
                if (item.getRemainingHours() > 0.0) left.push_back(item);
            }
            if (left.empty()) {
                std::cout << "(No tasks left to complete.)\n";
            } else {
                printItems(left, today);
            }
            continue;
        }

        if (cmd.name == "priority") {
            const auto all = planner.getAllItemsSortedByDue();
            std::vector<StudyItem> left;
            for (const auto& item : all) {
                if (item.getRemainingHours() > 0.0) left.push_back(item);
            }
            std::sort(left.begin(), left.end(), [](const StudyItem& a, const StudyItem& b) {
                if (a.priority != b.priority) return a.priority > b.priority;
                return a.dueDate < b.dueDate;
            });
            if (left.empty()) {
                std::cout << "(No tasks left to complete.)\n";
            } else {
                printItems(left, today);
            }
            continue;
        }

        if (cmd.name == "settings") {
            printSettings(planner);
            continue;
        }

        if (cmd.name == "setsession") {
            if (cmd.args.empty()) {
                std::cout << "Usage: setsession <hours>\n";
                continue;
            }
            double hours = 0.0;
            try {
                hours = std::stod(cmd.args[0]);
            } catch (...) {
                std::cout << "Validation error: session length must be a number.\n";
                continue;
            }

            std::string msg;
            if (!planner.setSessionLength(hours, &msg)) {
                std::cout << msg << "\n";
                continue;
            }

            std::cout << "Session length set to " << formatHours(hours) << "h. Regenerating plan...\n";
            std::vector<std::string> warnings;
            planner.regeneratePlan(today, &warnings);
            for (const auto& warning : warnings) {
                std::cout << warning << "\n";
            }
            continue;
        }

        if (cmd.name == "setmaxhours") {
            if (cmd.args.empty()) {
                std::cout << "Usage: setmaxhours <hours>\n";
                continue;
            }
            double hours = 0.0;
            try {
                hours = std::stod(cmd.args[0]);
            } catch (...) {
                std::cout << "Validation error: daily max hours must be a number.\n";
                continue;
            }

            std::string msg;
            if (!planner.setDailyMaxHours(hours, &msg)) {
                std::cout << msg << "\n";
                continue;
            }

            std::cout << "Daily max hours set to " << formatHours(hours) << ". Regenerating plan...\n";
            std::vector<std::string> warnings;
            planner.regeneratePlan(today, &warnings);
            for (const auto& warning : warnings) {
                std::cout << warning << "\n";
            }
            continue;
        }

        if (cmd.name == "setweekends") {
            if (cmd.args.empty()) {
                std::cout << "Usage: setweekends on|off\n";
                continue;
            }
            std::string value = cmd.args[0];
            std::transform(value.begin(), value.end(), value.begin(),
                           [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
            if (value != "on" && value != "off") {
                std::cout << "Usage: setweekends on|off\n";
                continue;
            }

            std::string msg;
            if (!planner.setIncludeWeekends(value == "on", &msg)) {
                std::cout << msg << "\n";
                continue;
            }

            std::cout << "Include weekends set to " << (value == "on" ? "ON" : "OFF")
                      << ". Regenerating plan...\n";
            std::vector<std::string> warnings;
            planner.regeneratePlan(today, &warnings);
            for (const auto& warning : warnings) {
                std::cout << warning << "\n";
            }
            continue;
        }

        if (cmd.name == "start-session") {
            if (cmd.args.empty()) {
                std::cout << "Usage: start-session <sessionId>\n";
                continue;
            }
            int sessionId = 0;
            try {
                sessionId = std::stoi(cmd.args[0]);
            } catch (...) {
                std::cout << "Usage: start-session <sessionId>  (sessionId must be a number)\n";
                continue;
            }

            std::string err;
            if (planner.startSession(sessionId, &err)) {
                std::cout << "Session marked as InProgress.\n";
            } else {
                std::cout << err << "\n";
            }
            continue;
        }

        if (cmd.name == "complete") {
            if (cmd.args.empty()) {
                std::cout << "Usage: complete <sessionId>\n";
                continue;
            }
            int sessionId = 0;
            try {
                sessionId = std::stoi(cmd.args[0]);
            } catch (...) {
                std::cout << "Usage: complete <sessionId>  (sessionId must be a number)\n";
                continue;
            }

            std::string err;
            if (planner.markSessionDone(sessionId, &err)) {
                std::cout << "Session marked completed.\n";
            } else {
                std::cout << err << "\n";
            }
            continue;
        }

        if (cmd.name == "analytics") {
            printAnalytics(planner, today);
            continue;
        }

        if (cmd.name == "reminders") {
            const auto reminders = planner.getReminders(today);
            if (reminders.empty()) {
                std::cout << "(No reminders)\n";
            } else {
                for (const auto& reminder : reminders) {
                    std::cout << "  - " << reminder << "\n";
                }
            }
            continue;
        }

        if (cmd.name == "save") {
            std::string err;
            if (planner.save(&err)) {
                std::cout << "Saved.\n";
            } else {
                std::cout << err << "\n";
            }
            continue;
        }

        if (cmd.name == "load") {
            std::string err;
            if (planner.load(&err)) {
                std::cout << "Loaded.\n";
            } else {
                std::cout << err << "\n";
            }
            continue;
        }

        if (cmd.name == "regen") {
            std::vector<std::string> warnings;
            planner.regeneratePlan(today, &warnings);
            std::cout << "Plan regenerated.\n";
            for (const auto& warning : warnings) {
                std::cout << warning << "\n";
            }
            continue;
        }

        std::cout << "Unknown command. Type 'help' for instructions.\n";
    }

    return 0;
}
