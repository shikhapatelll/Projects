#include "studyflow/JsonRepository.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace fs = std::filesystem;

namespace studyflow {

JsonRepository::JsonRepository(std::string baseDir) : baseDir_(std::move(baseDir)) {
    ensureDirs_(baseDir_);
    const auto path = credentialsPath_();
    if (!fs::exists(path)) {
        writeAtomic_(path, Json(Json::object{}).dump(2) + "\n");
    }
}

std::string JsonRepository::sanitizeUsername_(const std::string& username) {
    std::string out;
    for (char c : username) {
        if (std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '-') {
            out.push_back(c);
        }
    }
    return out;
}

std::string JsonRepository::credentialsPath_() const {
    return (fs::path(baseDir_) / "credentials.json").string();
}

std::string JsonRepository::userPath_(const std::string& username) const {
    const std::string safe = sanitizeUsername_(username);
    return (fs::path(baseDir_) / "users" / (safe + ".json")).string();
}

void JsonRepository::ensureDirs_(const std::string& baseDir) {
    fs::create_directories(fs::path(baseDir) / "users");
}

std::string JsonRepository::readFile_(const std::string& path) {
    std::ifstream in(path);
    if (!in) return "";
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

void JsonRepository::writeAtomic_(const std::string& path, const std::string& content) {
    fs::path p(path);
    fs::create_directories(p.parent_path());

    fs::path tmp = p;
    tmp += ".tmp";
    fs::path bak = p;
    bak += ".bak";

    {
        std::ofstream out(tmp.string(), std::ios::trunc);
        if (!out) throw std::runtime_error("Failed to open temp file for writing");
        out << content;
        out.flush();
        if (!out) throw std::runtime_error("Failed writing temp file");
    }

    try {
        (void)Json::parse(readFile_(tmp.string()));
    } catch (...) {
        fs::remove(tmp);
        throw std::runtime_error("Refusing to overwrite with invalid JSON (tmp parse failed)");
    }

    if (fs::exists(p)) {
        std::error_code ec;
        fs::copy_file(p, bak, fs::copy_options::overwrite_existing, ec);
    }

    std::error_code ec;
    fs::rename(tmp, p, ec);
    if (ec) {
        std::error_code ec2;
        fs::remove(p, ec2);
        fs::rename(tmp, p, ec2);
        if (ec2) {
            throw std::runtime_error("Failed to atomically save file: " + ec2.message());
        }
    }
}

Json JsonRepository::loadJsonSafe_(const std::string& path, bool& ok) {
    ok = false;
    if (!fs::exists(path)) return Json(Json::object{});
    try {
        Json j = Json::parse(readFile_(path));
        ok = true;
        return j;
    } catch (...) {
        ok = false;
        return Json(Json::object{});
    }
}

Json JsonRepository::loadCredsJson_(const std::string& path, bool& ok) {
    return loadJsonSafe_(path, ok);
}

std::string JsonRepository::getCredHash_(const Json& creds, const std::string& username) {
    if (!creds.isObject()) return "";
    const auto& object = creds.asObject();
    const auto it = object.find(username);
    if (it == object.end()) return "";
    if (!it->second.isString()) return "";
    return it->second.asString();
}

Json JsonRepository::setCredHash_(Json creds, const std::string& username, const std::string& hash) {
    if (!creds.isObject()) creds = Json(Json::object{});
    creds.asObject()[username] = Json(hash);
    return creds;
}

bool JsonRepository::userExists(const std::string& username) const {
    bool ok = false;
    const Json creds = loadCredsJson_(credentialsPath_(), ok);
    if (!ok || !creds.isObject()) return false;
    return creds.asObject().count(username) > 0;
}

void JsonRepository::storeCredentials(const std::string& username, const std::string& passwordHash) {
    bool ok = false;
    Json creds = loadCredsJson_(credentialsPath_(), ok);
    if (!ok) creds = Json(Json::object{});
    creds = setCredHash_(creds, username, passwordHash);
    writeAtomic_(credentialsPath_(), creds.dump(2) + "\n");
}

std::string JsonRepository::loadCredentials(const std::string& username) const {
    bool ok = false;
    const Json creds = loadCredsJson_(credentialsPath_(), ok);
    if (!ok) return "";
    return getCredHash_(creds, username);
}

/**
 * @brief Convert a study item into a JSON object.
 *
 * @param item Study item to serialize.
 * @return JSON object representation of the item.
 *
 * @author Group 30
 */
static Json itemToJson(const StudyItem& item) {
    Json::object object;
    object["id"] = item.id;
    object["type"] = toString(item.type);
    object["course"] = item.course;
    object["title"] = item.title;
    object["description"] = item.description;
    object["dueDate"] = item.dueDate.toString();
    object["estimatedHours"] = item.estimatedHours;
    object["priority"] = item.priority;
    object["hoursCompleted"] = item.hoursCompleted;
    return Json(object);
}

/**
 * @brief Convert a JSON value into a StudyItem instance.
 *
 * @param j JSON object storing an item.
 * @return Parsed StudyItem, or a default item when the input is invalid.
 *
 * @author Group 30
 */
static StudyItem itemFromJson(const Json& j) {
    StudyItem item;
    if (!j.isObject()) return item;
    const auto& object = j.asObject();

    auto getString = [&](const char* key) -> std::string {
        const auto it = object.find(key);
        if (it == object.end() || !it->second.isString()) return "";
        return it->second.asString();
    };
    auto getNumber = [&](const char* key) -> double {
        const auto it = object.find(key);
        if (it == object.end() || !it->second.isNumber()) return 0.0;
        return it->second.asNumber();
    };

    item.id = static_cast<int>(getNumber("id"));
    item.type = itemTypeFromString(getString("type"));
    item.course = getString("course");
    item.title = getString("title");
    item.description = getString("description");
    try {
        item.dueDate = Date::parse(getString("dueDate"));
    } catch (...) {
        item.dueDate = Date();
    }
    item.estimatedHours = getNumber("estimatedHours");
    item.priority = static_cast<int>(getNumber("priority"));
    item.hoursCompleted = getNumber("hoursCompleted");
    return item;
}

/**
 * @brief Convert a study session into a JSON object.
 *
 * @param session Session to serialize.
 * @return JSON object representation of the session.
 *
 * @author Group 30
 */
static Json sessionToJson(const StudySession& session) {
    Json::object object;
    object["id"] = session.id;
    object["itemId"] = session.itemId;
    object["scheduledDate"] = session.scheduledDate.toString();
    object["durationHours"] = session.durationHours;
    object["status"] = toString(session.status);
    return Json(object);
}

/**
 * @brief Convert a JSON value into a StudySession instance.
 *
 * @param j JSON object storing a session.
 * @return Parsed StudySession, or a default session when the input is invalid.
 *
 * @author Group 30
 */
static StudySession sessionFromJson(const Json& j) {
    StudySession session;
    if (!j.isObject()) return session;
    const auto& object = j.asObject();

    auto getString = [&](const char* key) -> std::string {
        const auto it = object.find(key);
        if (it == object.end() || !it->second.isString()) return "";
        return it->second.asString();
    };
    auto getNumber = [&](const char* key) -> double {
        const auto it = object.find(key);
        if (it == object.end() || !it->second.isNumber()) return 0.0;
        return it->second.asNumber();
    };

    session.id = static_cast<int>(getNumber("id"));
    session.itemId = static_cast<int>(getNumber("itemId"));
    try {
        session.scheduledDate = Date::parse(getString("scheduledDate"));
    } catch (...) {
        session.scheduledDate = Date();
    }
    session.durationHours = getNumber("durationHours");
    std::string statusText = getString("status");
    if (statusText.empty()) statusText = "Planned";
    session.status = sessionStatusFromString(statusText);
    return session;
}

Json JsonRepository::userDataToJson_(const UserData& data) {
    Json::object object;

    Json::array items;
    for (const auto& item : data.items) {
        items.push_back(itemToJson(item));
    }

    Json::array sessions;
    for (const auto& session : data.sessions) {
        sessions.push_back(sessionToJson(session));
    }

    Json::object settings;
    settings["dailyMaxHours"] = data.settings.dailyMaxHours;
    settings["sessionLengthHours"] = data.settings.sessionLengthHours;
    settings["includeWeekends"] = data.settings.includeWeekends;

    object["items"] = Json(items);
    object["sessions"] = Json(sessions);
    object["settings"] = Json(settings);
    object["nextItemId"] = data.nextItemId;
    object["nextSessionId"] = data.nextSessionId;
    return Json(object);
}

UserData JsonRepository::userDataFromJson_(const Json& j) {
    UserData data;
    if (!j.isObject()) return data;
    const auto& object = j.asObject();

    auto getNumber = [&](const char* key, double fallback) -> double {
        const auto it = object.find(key);
        if (it == object.end() || !it->second.isNumber()) return fallback;
        return it->second.asNumber();
    };

    const auto itemsIt = object.find("items");
    if (itemsIt != object.end() && itemsIt->second.isArray()) {
        for (const auto& ji : itemsIt->second.asArray()) {
            data.items.push_back(itemFromJson(ji));
        }
    }

    const auto sessionsIt = object.find("sessions");
    if (sessionsIt != object.end() && sessionsIt->second.isArray()) {
        for (const auto& js : sessionsIt->second.asArray()) {
            data.sessions.push_back(sessionFromJson(js));
        }
    }

    const auto settingsIt = object.find("settings");
    if (settingsIt != object.end() && settingsIt->second.isObject()) {
        const auto& settings = settingsIt->second.asObject();

        const auto hoursIt = settings.find("dailyMaxHours");
        if (hoursIt != settings.end() && hoursIt->second.isNumber()) {
            data.settings.dailyMaxHours = hoursIt->second.asNumber();
        }

        const auto sessionLenIt = settings.find("sessionLengthHours");
        if (sessionLenIt != settings.end() && sessionLenIt->second.isNumber()) {
            data.settings.sessionLengthHours = sessionLenIt->second.asNumber();
        }

        const auto weekendsIt = settings.find("includeWeekends");
        if (weekendsIt != settings.end() && weekendsIt->second.isBool()) {
            data.settings.includeWeekends = weekendsIt->second.asBool();
        }

        if (data.settings.dailyMaxHours < 0.0) data.settings.dailyMaxHours = 0.0;
        if (data.settings.dailyMaxHours > 24.0) data.settings.dailyMaxHours = 24.0;
        if (data.settings.sessionLengthHours <= 0.0) data.settings.sessionLengthHours = 1.0;
    }

    data.nextItemId = static_cast<int>(getNumber("nextItemId", 1.0));
    data.nextSessionId = static_cast<int>(getNumber("nextSessionId", 1.0));
    if (data.nextItemId <= 0) data.nextItemId = 1;
    if (data.nextSessionId <= 0) data.nextSessionId = 1;

    return data;
}

void JsonRepository::saveUser(const std::string& username, const UserData& data) {
    ensureDirs_(baseDir_);
    const auto path = userPath_(username);
    const Json j = userDataToJson_(data);
    writeAtomic_(path, j.dump(2) + "\n");
}

UserData JsonRepository::loadUserWithWarning_(const std::string& username, std::string* warning) const {
    ensureDirs_(baseDir_);
    const auto path = userPath_(username);

    if (!fs::exists(path)) {
        UserData empty;
        const_cast<JsonRepository*>(this)->saveUser(username, empty);
        return empty;
    }

    const std::string content = readFile_(path);
    try {
        const Json j = Json::parse(content);
        return userDataFromJson_(j);
    } catch (...) {
        fs::path backup = fs::path(path);
        backup += ".bak";
        if (fs::exists(backup)) {
            try {
                const std::string backupContent = readFile_(backup.string());
                const Json recovered = Json::parse(backupContent);
                const_cast<JsonRepository*>(this)->writeAtomic_(path, backupContent);
                if (warning) {
                    *warning = "Warning: Your saved data file was corrupted. Restored from backup.";
                }
                return userDataFromJson_(recovered);
            } catch (...) {
            }
        }

        UserData empty;
        const_cast<JsonRepository*>(this)->saveUser(username, empty);
        if (warning) {
            *warning = "Warning: Your saved data file was corrupted. It has been reset to empty.";
        }
        return empty;
    }
}

UserData JsonRepository::loadUser(const std::string& username) const {
    return loadUserWithWarning_(username, nullptr);
}

UserAccount JsonRepository::loadUserAccount(const std::string& username) const {
    UserAccount account;
    account.username = username;
    account.passwordHash = loadCredentials(username);
    account.data = loadUserWithWarning_(username, &account.loadWarning);
    return account;
}

} // namespace studyflow
