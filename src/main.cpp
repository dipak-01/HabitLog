#include "storage/JsonStorage.h"
#include "services/HabitManager.h"
#include "services/Statistics.h"
#include "ui/TerminalUI.h"
#include <iostream>
#include <string>

void printHelp() {
    std::cout << "Usage: habitlog [OPTIONS]\n\n"
              << "A full-featured personal habit tracker for your Linux terminal.\n\n"
              << "Options:\n"
              << "  -h, --help       Show this help message and exit\n"
              << "  -v, --version    Show version information and exit\n\n"
              << "Interactive Commands:\n"
              << "  Run without arguments to launch the interactive terminal UI.\n\n"
              << "Storage:\n"
              << "  Data is automatically saved to ~/.habitlog/habits.json\n"
              << "  and ~/.habitlog/completions.json\n";
}

void printVersion() {
    std::cout << "HabitLog v2.0.0 (C++17)\n"
              << "Personal Habit Tracker for Linux Terminal\n"
              << "License: MIT\n";
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "-h" || arg == "--help") {
            printHelp();
            return 0;
        }
        if (arg == "-v" || arg == "--version") {
            printVersion();
            return 0;
        }
        std::cerr << "Unknown option: " << arg << "\n"
                  << "Try 'habitlog --help' for more information.\n";
        return 1;
    }

    try {
        JsonStorage storage;
        HabitManager manager(storage);
        manager.load();
        Statistics stats(manager);
        TerminalUI ui(manager, stats);
        ui.run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
