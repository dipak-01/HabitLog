#pragma once
#include "services/HabitManager.h"
#include "services/Statistics.h"
#include <string>

// ANSI colors
#define RESET         "\033[0m"
#define BOLD          "\033[1m"
#define DIM           "\033[2m"
#define RED           "\033[31m"
#define GREEN         "\033[32m"
#define YELLOW        "\033[33m"
#define BLUE          "\033[34m"
#define MAGENTA       "\033[35m"
#define CYAN          "\033[36m"
#define WHITE         "\033[37m"
#define BRIGHT_RED    "\033[91m"
#define BRIGHT_GREEN  "\033[92m"
#define BRIGHT_YELLOW "\033[93m"
#define BRIGHT_BLUE   "\033[94m"
#define BRIGHT_MAGENTA "\033[95m"
#define BRIGHT_CYAN   "\033[96m"
#define BRIGHT_WHITE  "\033[97m"
#define BG_BLUE       "\033[44m"
#define BG_GREEN      "\033[42m"
#define BG_RED        "\033[41m"

class TerminalUI {
public:
    TerminalUI(HabitManager& manager, Statistics& stats);
    void run();

private:
    HabitManager& manager_;
    Statistics& stats_;

    void showMenu();
    void showDashboard();
    void completeHabit();
    void undoCompletion();
    void addHabit();
    void editHabit();
    void deleteHabit();
    void archiveHabit();
    void showWeeklyReport();
    void showMonthlyReport();
    void showHistory();
    void listHabits();

    // helpers
    void printHeader(const std::string& title) const;
    void printSeparator(int width = 54) const;
    void printFooter(int width = 54) const;
    void pressEnterToContinue() const;
    std::string getToday() const;
    std::string progressBar(double pct, int width = 30) const;
    int promptInt(const std::string& msg) const;
    std::string promptStr(const std::string& msg) const;
    void clearScreen() const;
};
