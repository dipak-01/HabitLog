#include "services/HabitManager.h"
#include <algorithm>
#include <ctime>
#include <stdexcept>

HabitManager::HabitManager(JsonStorage& storage) : storage_(storage) {}

void HabitManager::load() {
    habits_     = storage_.loadHabits();
    completions_ = storage_.loadCompletions();
    nextId_ = 1;
    for (const auto& h : habits_) {
        if (h.id >= nextId_) nextId_ = h.id + 1;
    }
}

void HabitManager::save() {
    storage_.saveHabits(habits_);
    storage_.saveCompletions(completions_);
}

std::string HabitManager::today() const {
    std::time_t t = std::time(nullptr);
    std::tm* tm   = std::localtime(&t);
    char buf[16];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", tm);
    return std::string(buf);
}

Habit& HabitManager::addHabit(const std::string& name, const std::string& desc, const std::string& cat) {
    Habit h;
    h.id          = nextId_++;
    h.name        = name;
    h.description = desc;
    h.category    = cat;
    h.active      = true;
    h.createdAt   = today();
    habits_.push_back(h);
    save();
    return habits_.back();
}

bool HabitManager::editHabit(int id, const std::string& name, const std::string& desc, const std::string& cat) {
    auto opt = findHabit(id);
    if (!opt) return false;
    Habit& h = opt->get();
    if (!name.empty()) h.name        = name;
    if (!desc.empty()) h.description = desc;
    if (!cat.empty())  h.category    = cat;
    save();
    return true;
}

bool HabitManager::deleteHabit(int id) {
    auto it = std::remove_if(habits_.begin(), habits_.end(),
                             [id](const Habit& h){ return h.id == id; });
    if (it == habits_.end()) return false;
    habits_.erase(it, habits_.end());
    // also remove completions
    auto ci = std::remove_if(completions_.begin(), completions_.end(),
                              [id](const Completion& c){ return c.habitId == id; });
    completions_.erase(ci, completions_.end());
    save();
    return true;
}

bool HabitManager::archiveHabit(int id) {
    auto opt = findHabit(id);
    if (!opt) return false;
    opt->get().active = false;
    save();
    return true;
}

std::optional<std::reference_wrapper<Habit>> HabitManager::findHabit(int id) {
    for (auto& h : habits_) {
        if (h.id == id) return std::ref(h);
    }
    return std::nullopt;
}

bool HabitManager::markComplete(int habitId, const std::string& date) {
    if (isCompleted(habitId, date)) return false;
    completions_.push_back({habitId, date});
    save();
    return true;
}

bool HabitManager::undoCompletion(int habitId, const std::string& date) {
    auto it = std::remove_if(completions_.begin(), completions_.end(),
                              [habitId, &date](const Completion& c){
                                  return c.habitId == habitId && c.date == date;
                              });
    if (it == completions_.end()) return false;
    completions_.erase(it, completions_.end());
    save();
    return true;
}

bool HabitManager::isCompleted(int habitId, const std::string& date) const {
    for (const auto& c : completions_) {
        if (c.habitId == habitId && c.date == date) return true;
    }
    return false;
}

const std::vector<Habit>& HabitManager::getHabits() const { return habits_; }

const std::vector<Completion>& HabitManager::getCompletions() const { return completions_; }

std::vector<Habit> HabitManager::getActiveHabits() const {
    std::vector<Habit> active;
    for (const auto& h : habits_) {
        if (h.active) active.push_back(h);
    }
    return active;
}

std::vector<std::string> HabitManager::getDatesForHabit(int habitId) const {
    std::vector<std::string> dates;
    for (const auto& c : completions_) {
        if (c.habitId == habitId) dates.push_back(c.date);
    }
    std::sort(dates.begin(), dates.end());
    return dates;
}

// Helper: parse YYYY-MM-DD into time_t (midnight local)
static std::time_t parseDate(const std::string& d) {
    std::tm tm{};
    tm.tm_year = std::stoi(d.substr(0, 4)) - 1900;
    tm.tm_mon  = std::stoi(d.substr(5, 2)) - 1;
    tm.tm_mday = std::stoi(d.substr(8, 2));
    tm.tm_isdst = -1;
    return std::mktime(&tm);
}

// Helper: advance a date by N days
static std::string addDays(const std::string& date, int n) {
    std::time_t t = parseDate(date);
    t += static_cast<std::time_t>(n) * 86400;
    std::tm* tm = std::localtime(&t);
    char buf[16];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", tm);
    return std::string(buf);
}

int HabitManager::getCurrentStreak(int habitId) const {
    auto dates = getDatesForHabit(habitId);
    if (dates.empty()) return 0;

    std::string cur = today();
    int streak = 0;

    // Check if today or yesterday starts the streak
    if (std::find(dates.begin(), dates.end(), cur) == dates.end()) {
        // not done today; check if yesterday completed (streak may be live)
        std::string yesterday = addDays(cur, -1);
        if (std::find(dates.begin(), dates.end(), yesterday) == dates.end()) {
            return 0;
        }
        cur = yesterday;
    }

    while (true) {
        if (std::find(dates.begin(), dates.end(), cur) != dates.end()) {
            streak++;
            cur = addDays(cur, -1);
        } else {
            break;
        }
    }
    return streak;
}

int HabitManager::getLongestStreak(int habitId) const {
    auto dates = getDatesForHabit(habitId);
    if (dates.empty()) return 0;

    int longest = 1, current = 1;
    for (std::size_t i = 1; i < dates.size(); ++i) {
        std::string expected = addDays(dates[i-1], 1);
        if (dates[i] == expected) {
            current++;
            if (current > longest) longest = current;
        } else {
            current = 1;
        }
    }
    return longest;
}
