#include "storage/JsonStorage.h"
#include "services/HabitManager.h"
#include "services/Statistics.h"
#include "ui/TerminalUI.h"
#include <iostream>

int main() {
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
