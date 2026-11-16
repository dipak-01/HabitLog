#include "services/Statistics.h"
#include <algorithm>
#include <ctime>
#include <sstream>
#include <iomanip>

Statistics::Statistics(const HabitManager& manager) : manager_(manager) {}

std::string Statistics::today() const {
    std::time_t t = std::time(nullptr);
    std::tm* tm   = std::localtime(&t);
    char buf[16];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", tm);
    return std::string(buf);
}

std::string Statistics::dateOffset(const std::string& date, int offsetDays) const {
    std::tm tm{};
    tm.tm_year = std::stoi(date.substr(0, 4)) - 1900;
    tm.tm_mon  = std::stoi(date.substr(5, 2)) - 1;
    tm.tm_mday = std::stoi(date.substr(8, 2));
    tm.tm_isdst = -1;
    std::time_t t = std::mktime(&tm);
    t += static_cast<std::time_t>(offsetDays) * 86400;
    std::tm* result = std::localtime(&t);
    char buf[16];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", result);
    return std::string(buf);
}

std::vector<std::string> Statistics::getLast7Days() const {
    std::vector<std::string> days;
    std::string t = today();
    for (int i = -6; i <= 0; ++i) {
        days.push_back(dateOffset(t, i));
    }
    return days;
}

std::vector<std::string> Statistics::getThisMonthDays() const {
    std::string t = today();
    std::string firstDay = t.substr(0, 7) + "-01";
    std::vector<std::string> days;
    std::string cur = firstDay;
    while (cur <= t) {
        days.push_back(cur);
        cur = dateOffset(cur, 1);
    }
    return days;
}

std::vector<WeeklyReport> Statistics::getWeeklyReports() const {
    auto habits    = manager_.getActiveHabits();
    auto last7     = getLast7Days();

    // Day-of-week labels
    static const char* dayNames[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
    std::vector<std::string> labels;
    for (const auto& d : last7) {
        std::tm tm{};
        tm.tm_year = std::stoi(d.substr(0,4)) - 1900;
        tm.tm_mon  = std::stoi(d.substr(5,2)) - 1;
        tm.tm_mday = std::stoi(d.substr(8,2));
        tm.tm_isdst = -1;
        std::mktime(&tm);
        labels.push_back(dayNames[tm.tm_wday]);
    }

    std::vector<WeeklyReport> reports;
    for (const auto& h : habits) {
        WeeklyReport r;
        r.habitId        = h.id;
        r.habitName      = h.name;
        r.dayLabels      = labels;
        r.completedCount = 0;
        for (const auto& d : last7) {
            bool done = manager_.isCompleted(h.id, d);
            r.days.push_back(done);
            if (done) r.completedCount++;
        }
        reports.push_back(r);
    }
    return reports;
}

std::vector<MonthlyReport> Statistics::getMonthlyReports() const {
    auto habits    = manager_.getActiveHabits();
    auto monthDays = getThisMonthDays();
    int total      = static_cast<int>(monthDays.size());

    std::vector<MonthlyReport> reports;
    for (const auto& h : habits) {
        MonthlyReport r;
        r.habitId    = h.id;
        r.habitName  = h.name;
        r.totalDays  = total;
        r.completedDays = 0;
        for (const auto& d : monthDays) {
            if (manager_.isCompleted(h.id, d)) r.completedDays++;
        }
        r.percentage = (total > 0) ? 100.0 * r.completedDays / total : 0.0;
        reports.push_back(r);
    }
    return reports;
}

std::vector<std::string> Statistics::getHistory(int habitId, int days) const {
    std::vector<std::string> result;
    std::string t = today();
    for (int i = -(days - 1); i <= 0; ++i) {
        std::string d = dateOffset(t, i);
        bool done = manager_.isCompleted(habitId, d);
        result.push_back(d + (done ? " ✅" : " ❌"));
    }
    return result;
}

std::vector<std::pair<std::string, double>> Statistics::getBestHabits() const {
    auto reports = getMonthlyReports();
    std::sort(reports.begin(), reports.end(),
              [](const MonthlyReport& a, const MonthlyReport& b){
                  return a.percentage > b.percentage;
              });
    std::vector<std::pair<std::string, double>> result;
    for (const auto& r : reports) {
        result.emplace_back(r.habitName, r.percentage);
    }
    return result;
}

std::vector<std::string> Statistics::getMissedHabits() const {
    auto habits = manager_.getActiveHabits();
    auto last7  = getLast7Days();
    std::vector<std::string> missed;
    for (const auto& h : habits) {
        bool anyDone = false;
        for (const auto& d : last7) {
            if (manager_.isCompleted(h.id, d)) { anyDone = true; break; }
        }
        if (!anyDone) missed.push_back(h.name);
    }
    return missed;
}
