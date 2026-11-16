#pragma once
#include "models/Habit.h"
#include <vector>
#include <string>

class JsonStorage {
public:
    JsonStorage();
    std::vector<Habit> loadHabits() const;
    void saveHabits(const std::vector<Habit>& habits) const;
    std::vector<Completion> loadCompletions() const;
    void saveCompletions(const std::vector<Completion>& completions) const;
    std::string getDataDir() const;
private:
    std::string dataDir_;
    std::string habitFile_;
    std::string completionFile_;
    void ensureDir() const;
};
