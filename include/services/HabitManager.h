#pragma once
#include "models/Habit.h"
#include "storage/JsonStorage.h"
#include <vector>
#include <string>
#include <optional>
#include <functional>

class HabitManager {
public:
    explicit HabitManager(JsonStorage& storage);
    void load();
    void save();

    // CRUD
    Habit& addHabit(const std::string& name, const std::string& desc, const std::string& cat);
    bool editHabit(int id, const std::string& name, const std::string& desc, const std::string& cat);
    bool deleteHabit(int id);
    bool archiveHabit(int id);
    std::optional<std::reference_wrapper<Habit>> findHabit(int id);

    // Completion
    bool markComplete(int habitId, const std::string& date);
    bool undoCompletion(int habitId, const std::string& date);
    bool isCompleted(int habitId, const std::string& date) const;

    // Accessors
    const std::vector<Habit>& getHabits() const;
    const std::vector<Completion>& getCompletions() const;
    std::vector<Habit> getActiveHabits() const;

    // Streak
    int getCurrentStreak(int habitId) const;
    int getLongestStreak(int habitId) const;

private:
    JsonStorage& storage_;
    std::vector<Habit> habits_;
    std::vector<Completion> completions_;
    int nextId_ = 1;
    std::string today() const;
    std::vector<std::string> getDatesForHabit(int habitId) const;
};
