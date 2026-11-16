#pragma once
#include "models/Habit.h"
#include "services/HabitManager.h"
#include <vector>
#include <string>
#include <map>

struct WeeklyReport {
    int habitId;
    std::string habitName;
    std::vector<bool> days;  // last 7 days (oldest first)
    std::vector<std::string> dayLabels;
    int completedCount;
};

struct MonthlyReport {
    int habitId;
    std::string habitName;
    int completedDays;
    int totalDays;
    double percentage;
};

class Statistics {
public:
    explicit Statistics(const HabitManager& manager);
    std::vector<WeeklyReport> getWeeklyReports() const;
    std::vector<MonthlyReport> getMonthlyReports() const;
    std::vector<std::string> getHistory(int habitId, int days = 30) const;
    std::vector<std::pair<std::string, double>> getBestHabits() const;
    std::vector<std::string> getMissedHabits() const;
private:
    const HabitManager& manager_;
    std::string today() const;
    std::string dateOffset(const std::string& date, int offsetDays) const;
    std::vector<std::string> getLast7Days() const;
    std::vector<std::string> getThisMonthDays() const;
};
