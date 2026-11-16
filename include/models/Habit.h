#pragma once
#include <string>
#include <vector>

struct Habit {
    int id = 0;
    std::string name;
    std::string description;
    std::string category;
    bool active = true;
    std::string createdAt;  // YYYY-MM-DD
};

struct Completion {
    int habitId = 0;
    std::string date;  // YYYY-MM-DD
};
