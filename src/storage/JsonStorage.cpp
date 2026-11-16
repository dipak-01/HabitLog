#include "storage/JsonStorage.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <stdexcept>
#include <sys/stat.h>
#include <cstdlib>

using json = nlohmann::json;

JsonStorage::JsonStorage() {
    const char* home = std::getenv("HOME");
    if (!home) throw std::runtime_error("HOME environment variable not set");
    dataDir_ = std::string(home) + "/.habitlog/";
    habitFile_ = dataDir_ + "habits.json";
    completionFile_ = dataDir_ + "completions.json";
    ensureDir();
}

void JsonStorage::ensureDir() const {
    struct stat st{};
    if (stat(dataDir_.c_str(), &st) != 0) {
        ::mkdir(dataDir_.c_str(), 0755);
    }
}

std::string JsonStorage::getDataDir() const {
    return dataDir_;
}

std::vector<Habit> JsonStorage::loadHabits() const {
    std::vector<Habit> habits;
    std::ifstream f(habitFile_);
    if (!f.is_open()) return habits;
    try {
        json j;
        f >> j;
        for (const auto& item : j) {
            Habit h;
            h.id          = item.value("id", 0);
            h.name        = item.value("name", "");
            h.description = item.value("description", "");
            h.category    = item.value("category", "");
            h.active      = item.value("active", true);
            h.createdAt   = item.value("createdAt", "");
            habits.push_back(h);
        }
    } catch (...) {}
    return habits;
}

void JsonStorage::saveHabits(const std::vector<Habit>& habits) const {
    json j = json::array();
    for (const auto& h : habits) {
        j.push_back({
            {"id",          h.id},
            {"name",        h.name},
            {"description", h.description},
            {"category",    h.category},
            {"active",      h.active},
            {"createdAt",   h.createdAt}
        });
    }
    std::ofstream f(habitFile_);
    if (!f.is_open()) throw std::runtime_error("Cannot write " + habitFile_);
    f << j.dump(4);
}

std::vector<Completion> JsonStorage::loadCompletions() const {
    std::vector<Completion> completions;
    std::ifstream f(completionFile_);
    if (!f.is_open()) return completions;
    try {
        json j;
        f >> j;
        for (const auto& item : j) {
            Completion c;
            c.habitId = item.value("habitId", 0);
            c.date    = item.value("date", "");
            completions.push_back(c);
        }
    } catch (...) {}
    return completions;
}

void JsonStorage::saveCompletions(const std::vector<Completion>& completions) const {
    json j = json::array();
    for (const auto& c : completions) {
        j.push_back({
            {"habitId", c.habitId},
            {"date",    c.date}
        });
    }
    std::ofstream f(completionFile_);
    if (!f.is_open()) throw std::runtime_error("Cannot write " + completionFile_);
    f << j.dump(4);
}
