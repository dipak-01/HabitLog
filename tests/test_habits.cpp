#include "storage/JsonStorage.h"
#include "services/HabitManager.h"
#include <cassert>
#include <iostream>
#include <ctime>
#include <cstdlib>  // for setenv

// Use a temp dir for tests
static void setupTestEnv() {
    // Override HOME to a temp path so tests don't pollute real data
    ::setenv("HOME", "/tmp/habitlog_test_home", 1);
    ::system("mkdir -p /tmp/habitlog_test_home/.habitlog");
    // Clear any leftover test data
    ::system("rm -f /tmp/habitlog_test_home/.habitlog/habits.json");
    ::system("rm -f /tmp/habitlog_test_home/.habitlog/completions.json");
}

static std::string todayStr() {
    time_t t = time(nullptr);
    struct tm* ltm = localtime(&t);
    char buf[16];
    strftime(buf, sizeof(buf), "%Y-%m-%d", ltm);
    return std::string(buf);
}

static std::string dayOffset(const std::string& date, int n) {
    struct tm stm{};
    stm.tm_year = std::stoi(date.substr(0,4)) - 1900;
    stm.tm_mon  = std::stoi(date.substr(5,2)) - 1;
    stm.tm_mday = std::stoi(date.substr(8,2));
    stm.tm_isdst = -1;
    time_t t = mktime(&stm);
    t += static_cast<time_t>(n) * 86400;
    struct tm* r = localtime(&t);
    char buf[16];
    strftime(buf, sizeof(buf), "%Y-%m-%d", r);
    return std::string(buf);
}

void testAddHabit() {
    JsonStorage storage;
    HabitManager mgr(storage);
    mgr.load();

    // Count existing, then add
    std::size_t before = mgr.getHabits().size();
    Habit& h = mgr.addHabit("Exercise", "Daily workout", "Health");
    assert(h.id > 0);
    assert(h.name == "Exercise");
    assert(h.category == "Health");
    assert(h.active == true);
    assert(mgr.getHabits().size() == before + 1);
    std::cout << "  [PASS] testAddHabit\n";
}

void testMarkComplete() {
    JsonStorage storage;
    HabitManager mgr(storage);
    mgr.load();

    Habit& h = mgr.addHabit("Read", "Read every day", "Education");
    std::string today = todayStr();

    // Mark complete
    bool ok = mgr.markComplete(h.id, today);
    assert(ok == true);
    assert(mgr.isCompleted(h.id, today) == true);

    // Duplicate prevention
    bool dup = mgr.markComplete(h.id, today);
    assert(dup == false);

    std::cout << "  [PASS] testMarkComplete\n";
}

void testUndoCompletion() {
    JsonStorage storage;
    HabitManager mgr(storage);
    mgr.load();

    Habit& h = mgr.addHabit("Meditate", "Morning meditation", "Wellness");
    std::string today = todayStr();

    mgr.markComplete(h.id, today);
    assert(mgr.isCompleted(h.id, today) == true);

    bool undone = mgr.undoCompletion(h.id, today);
    assert(undone == true);
    assert(mgr.isCompleted(h.id, today) == false);

    // Undo again should fail
    bool undone2 = mgr.undoCompletion(h.id, today);
    assert(undone2 == false);

    std::cout << "  [PASS] testUndoCompletion\n";
}

void testStreak() {
    JsonStorage storage;
    HabitManager mgr(storage);
    mgr.load();

    Habit& h = mgr.addHabit("Journal", "Daily journaling", "Mindfulness");
    std::string today = todayStr();

    // Build 3-day streak ending today
    mgr.markComplete(h.id, dayOffset(today, -2));
    mgr.markComplete(h.id, dayOffset(today, -1));
    mgr.markComplete(h.id, today);

    int cur = mgr.getCurrentStreak(h.id);
    assert(cur == 3);

    int lon = mgr.getLongestStreak(h.id);
    assert(lon == 3);

    // Break streak: add completions from 10 days ago for 5 consecutive days
    for (int i = -14; i <= -10; ++i) {
        mgr.markComplete(h.id, dayOffset(today, i));
    }
    int lon2 = mgr.getLongestStreak(h.id);
    assert(lon2 >= 3);  // 3-day streak is at least tied

    std::cout << "  [PASS] testStreak\n";
}

void testEditAndDelete() {
    JsonStorage storage;
    HabitManager mgr(storage);
    mgr.load();

    Habit& h = mgr.addHabit("Sleep", "Sleep 8h", "Health");
    int id = h.id;

    bool edited = mgr.editHabit(id, "Sleep Early", "", "");
    assert(edited == true);
    auto opt = mgr.findHabit(id);
    assert(opt.has_value());
    assert(opt->get().name == "Sleep Early");

    bool deleted = mgr.deleteHabit(id);
    assert(deleted == true);
    assert(!mgr.findHabit(id).has_value());

    std::cout << "  [PASS] testEditAndDelete\n";
}

void testArchiveHabit() {
    JsonStorage storage;
    HabitManager mgr(storage);
    mgr.load();

    Habit& h = mgr.addHabit("Coding Practice", "Competitive programming", "Coding");
    int id = h.id;

    assert(h.active == true);
    bool archived = mgr.archiveHabit(id);
    assert(archived == true);

    auto opt = mgr.findHabit(id);
    assert(opt.has_value());
    assert(opt->get().active == false);

    // Active habits should exclude archived habit
    auto activeHabits = mgr.getActiveHabits();
    for (const auto& habit : activeHabits) {
        assert(habit.id != id);
    }

    std::cout << "  [PASS] testArchiveHabit\n";
}

int main() {
    setupTestEnv();
    std::cout << "\n  HabitLog Test Suite\n";
    std::cout << "  ===================\n";

    try {
        testAddHabit();
        testMarkComplete();
        testUndoCompletion();
        testStreak();
        testEditAndDelete();
        testArchiveHabit();
    } catch (const std::exception& e) {
        std::cerr << "\n  [FAIL] Exception: " << e.what() << "\n";
        return 1;
    }

    std::cout << "\n  All tests passed! 🎉\n\n";
    return 0;
}
