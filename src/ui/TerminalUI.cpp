#include "ui/TerminalUI.h"
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>

TerminalUI::TerminalUI(HabitManager &manager, Statistics &stats)
    : manager_(manager), stats_(stats) {}

void TerminalUI::clearScreen() const {
  std::cout << "\033[2J\033[H" << std::flush;
}

void TerminalUI::printSeparator(int width) const {
  std::cout << CYAN;
  for (int i = 0; i < width; ++i)
    std::cout << "─";
  std::cout << RESET << "\n";
}

// Returns the display column-width of a UTF-8 string.
// Each ASCII char = 1, each multi-byte UTF-8 sequence = 1 (emoji are wide=2).
static int displayWidth(const std::string &s) {
  int width = 0;
  for (std::size_t i = 0; i < s.size();) {
    unsigned char c = static_cast<unsigned char>(s[i]);
    int seqLen = 1;
    if ((c & 0x80) == 0x00)
      seqLen = 1;
    else if ((c & 0xE0) == 0xC0)
      seqLen = 2;
    else if ((c & 0xF0) == 0xE0)
      seqLen = 3;
    else if ((c & 0xF8) == 0xF0)
      seqLen = 4;
    // 4-byte sequences are emoji/symbols → display width 2
    width += (seqLen == 4) ? 2 : 1;
    i += seqLen;
  }
  return width;
}

void TerminalUI::printHeader(const std::string &title) const {
  const int width = 54;        // total box width in columns
  const int inner = width - 2; // space between ║ and ║

  std::cout << BOLD << BRIGHT_CYAN;
  // Top border
  std::cout << "╔";
  for (int i = 0; i < inner; ++i)
    std::cout << "═";
  std::cout << "╗\n";

  // Title row — use display width for correct centering
  int dw = displayWidth(title);
  int padding = (inner - dw) / 2;
  int rPad = inner - dw - padding;
  std::cout << "║";
  for (int i = 0; i < padding; ++i)
    std::cout << " ";
  std::cout << BRIGHT_WHITE << BOLD << title << BRIGHT_CYAN;
  for (int i = 0; i < rPad; ++i)
    std::cout << " ";
  std::cout << "║\n";

  // Bottom border
  std::cout << "╚";
  for (int i = 0; i < inner; ++i)
    std::cout << "═";
  std::cout << "╝" << RESET << "\n";
}

void TerminalUI::printFooter(int width) const {
  std::cout << DIM << CYAN;
  for (int i = 0; i < width; ++i)
    std::cout << "─";
  std::cout << RESET << "\n";
}

void TerminalUI::pressEnterToContinue() const {
  std::cout << DIM << "\n  Press Enter to continue..." << RESET;
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string TerminalUI::getToday() const {
  std::time_t t = std::time(nullptr);
  std::tm *tm = std::localtime(&t);
  char buf[16];
  std::strftime(buf, sizeof(buf), "%Y-%m-%d", tm);
  return std::string(buf);
}

std::string TerminalUI::progressBar(double pct, int width) const {
  int filled = static_cast<int>(pct / 100.0 * width);
  std::string bar = "[";
  for (int i = 0; i < width; ++i) {
    bar += (i < filled) ? "█" : "░";
  }
  bar += "]";
  return bar;
}

int TerminalUI::promptInt(const std::string &msg) const {
  std::cout << BRIGHT_YELLOW << msg << RESET;
  int v;
  while (!(std::cin >> v)) {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << RED << "  Please enter a number: " << RESET;
  }
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  return v;
}

std::string TerminalUI::promptStr(const std::string &msg) const {
  std::cout << BRIGHT_YELLOW << msg << RESET;
  std::string s;
  std::getline(std::cin, s);
  return s;
}

void TerminalUI::showMenu() {
  std::cout << "\n" << BOLD << BRIGHT_BLUE << "  📋  MENU\n" << RESET;
  printSeparator(54);
  std::cout << "  " << BRIGHT_CYAN << " 1" << RESET << "  Dashboard\n";
  std::cout << "  " << BRIGHT_CYAN << " 2" << RESET << "  List All Habits\n";
  std::cout << "  " << BRIGHT_GREEN << " 3" << RESET
            << "  Mark Habit Complete\n";
  std::cout << "  " << BRIGHT_RED << " 4" << RESET << "  Undo Completion\n";
  std::cout << "  " << YELLOW << " 5" << RESET << "  Add New Habit\n";
  std::cout << "  " << YELLOW << " 6" << RESET << "  Edit Habit\n";
  std::cout << "  " << RED << " 7" << RESET << "  Delete Habit\n";
  std::cout << "  " << DIM << " 8" << RESET << "  Archive Habit\n";
  std::cout << "  " << MAGENTA << " 9" << RESET << "  Weekly Report\n";
  std::cout << "  " << MAGENTA << "10" << RESET << "  Monthly Report\n";
  std::cout << "  " << CYAN << "11" << RESET << "  History\n";
  std::cout << "  " << DIM << " 0" << RESET << "  Quit\n";
  printSeparator(54);
}

void TerminalUI::run() {
  while (true) {
    clearScreen();
    printHeader("🌿  HabitLog  v2.0");
    showMenu();
    int choice = promptInt("  Your choice: ");
    clearScreen();
    switch (choice) {
    case 1:
      showDashboard();
      break;
    case 2:
      listHabits();
      break;
    case 3:
      completeHabit();
      break;
    case 4:
      undoCompletion();
      break;
    case 5:
      addHabit();
      break;
    case 6:
      editHabit();
      break;
    case 7:
      deleteHabit();
      break;
    case 8:
      archiveHabit();
      break;
    case 9:
      showWeeklyReport();
      break;
    case 10:
      showMonthlyReport();
      break;
    case 11:
      showHistory();
      break;
    case 0:
      std::cout << BRIGHT_GREEN
                << "\n  Goodbye! Keep building great habits! 🌟\n\n"
                << RESET;
      return;
    default:
      std::cout << RED << "\n  Invalid option. Try again.\n" << RESET;
      pressEnterToContinue();
    }
  }
}

void TerminalUI::showDashboard() {
  printHeader("📊  Today's Dashboard");
  std::string today = getToday();
  std::cout << "  " << DIM << "Date: " << today << RESET << "\n\n";

  auto habits = manager_.getActiveHabits();
  if (habits.empty()) {
    std::cout << BRIGHT_YELLOW << "\n  No habits yet — add one with option 5!\n"
              << RESET;
    pressEnterToContinue();
    return;
  }

  int done = 0;
  for (const auto &h : habits) {
    bool completed = manager_.isCompleted(h.id, today);
    int streak = manager_.getCurrentStreak(h.id);
    if (completed)
      done++;

    std::string status = completed ? std::string(GREEN) + "✅ DONE  " + RESET
                                   : std::string(RED) + "⬜ TODO  " + RESET;

    std::cout << "  " << status << BOLD << std::left << std::setw(22) << h.name
              << RESET;

    if (streak > 0) {
      std::cout << " 🔥 " << BRIGHT_YELLOW << streak << " day"
                << (streak > 1 ? "s" : "") << RESET;
    }
    if (!h.category.empty()) {
      std::cout << "  " << DIM << "[" << h.category << "]" << RESET;
    }
    std::cout << "\n";
  }

  // summary bar
  double pct =
      habits.empty() ? 0.0 : 100.0 * done / static_cast<int>(habits.size());
  std::cout << "\n";
  printSeparator(54);
  std::cout << "  Progress: " << GREEN << progressBar(pct, 24) << RESET << " "
            << BRIGHT_WHITE << static_cast<int>(pct) << "%" << RESET << "  ("
            << done << "/" << habits.size() << " done)\n";

  // missed habits
  auto missed = stats_.getMissedHabits();
  if (!missed.empty()) {
    std::cout << "\n  " << RED << "⚠  Missed this week: " << RESET;
    for (std::size_t i = 0; i < missed.size(); ++i) {
      if (i)
        std::cout << ", ";
      std::cout << missed[i];
    }
    std::cout << "\n";
  }
  pressEnterToContinue();
}

void TerminalUI::listHabits() {
  printHeader("📋  All Habits");
  auto habits = manager_.getHabits();
  if (habits.empty()) {
    std::cout << BRIGHT_YELLOW << "\n  No habits found.\n" << RESET;
    pressEnterToContinue();
    return;
  }
  std::cout << "  " << BOLD << std::left << std::setw(5) << "ID"
            << std::setw(22) << "Name" << std::setw(14) << "Category"
            << std::setw(8) << "Status"
            << "Streak\n"
            << RESET;
  printSeparator(54);
  for (const auto &h : habits) {
    int streak = manager_.getCurrentStreak(h.id);
    std::string status = h.active ? std::string(GREEN) + "Active " + RESET
                                  : std::string(DIM) + "Archived" + RESET;

    // Color-code category badge
    std::string catColor;
    std::string cat = h.category;
    if      (cat == "Health")   catColor = BRIGHT_GREEN;
    else if (cat == "Coding")   catColor = BRIGHT_CYAN;
    else if (cat == "Learning") catColor = BRIGHT_YELLOW;
    else if (cat == "Wellness") catColor = BRIGHT_MAGENTA;
    else                        catColor = WHITE;

    std::cout << "  " << BRIGHT_CYAN << std::setw(5) << h.id << RESET
              << std::left << std::setw(22) << h.name
              << catColor << std::setw(14) << cat << RESET
              << status;
    if (streak > 0)
      std::cout << " 🔥 " << BRIGHT_YELLOW << streak << RESET;
    std::cout << "\n";
  }
  pressEnterToContinue();
}

void TerminalUI::completeHabit() {
  printHeader("✅  Mark Habit Complete");
  auto habits = manager_.getActiveHabits();
  if (habits.empty()) {
    std::cout << BRIGHT_YELLOW << "\n  No active habits.\n" << RESET;
    pressEnterToContinue();
    return;
  }
  std::string today = getToday();
  std::cout << "\n  Active habits:\n";
  for (const auto &h : habits) {
    bool done = manager_.isCompleted(h.id, today);
    std::cout << "  [" << BRIGHT_CYAN << h.id << RESET << "] " << h.name
              << (done ? std::string("  ") + GREEN + "✅ done" + RESET : "")
              << "\n";
  }
  int id = promptInt("\n  Enter habit ID (0 to cancel): ");
  if (id == 0)
    return;
  if (manager_.markComplete(id, today)) {
    std::cout << BRIGHT_GREEN << "\n  ✅ Marked complete!\n" << RESET;
    int streak = manager_.getCurrentStreak(id);
    if (streak > 1)
      std::cout << "  🔥 Streak: " << BRIGHT_YELLOW << streak << " days!\n"
                << RESET;
  } else {
    std::cout << YELLOW
              << "\n  Already marked complete today, or habit not found.\n"
              << RESET;
  }
  pressEnterToContinue();
}

void TerminalUI::undoCompletion() {
  printHeader("↩  Undo Completion");
  std::string today = getToday();
  auto habits = manager_.getActiveHabits();
  std::cout << "\n  Habits completed today:\n";
  bool any = false;
  for (const auto &h : habits) {
    if (manager_.isCompleted(h.id, today)) {
      std::cout << "  [" << BRIGHT_CYAN << h.id << RESET << "] " << h.name
                << "\n";
      any = true;
    }
  }
  if (!any) {
    std::cout << BRIGHT_YELLOW << "\n  No completions to undo today.\n"
              << RESET;
    pressEnterToContinue();
    return;
  }
  int id = promptInt("\n  Enter habit ID to undo (0 to cancel): ");
  if (id == 0)
    return;
  if (manager_.undoCompletion(id, today)) {
    std::cout << BRIGHT_GREEN << "\n  Completion removed.\n" << RESET;
  } else {
    std::cout << RED << "\n  Habit not completed today, or not found.\n"
              << RESET;
  }
  pressEnterToContinue();
}

void TerminalUI::addHabit() {
  printHeader("➕  Add New Habit");
  std::string name = promptStr("\n  Name: ");
  if (name.empty()) {
    std::cout << RED << "  Name cannot be empty.\n" << RESET;
    pressEnterToContinue();
    return;
  }
  std::string desc = promptStr("  Description: ");
  std::string cat = promptStr("  Category (e.g. Health, Work): ");
  Habit &h = manager_.addHabit(name, desc, cat);
  std::cout << BRIGHT_GREEN << "\n  ✅ Habit \"" << h.name
            << "\" added with ID " << h.id << "!\n"
            << RESET;
  pressEnterToContinue();
}

void TerminalUI::editHabit() {
  printHeader("✏️   Edit Habit");
  listHabits();
  int id = promptInt("  Enter habit ID to edit (0 to cancel): ");
  if (id == 0)
    return;
  auto opt = manager_.findHabit(id);
  if (!opt) {
    std::cout << RED << "\n  Habit not found.\n" << RESET;
    pressEnterToContinue();
    return;
  }
  Habit &h = opt->get();
  std::cout << "  Leave blank to keep current value.\n\n";
  std::string name = promptStr("  Name [" + h.name + "]: ");
  std::string desc = promptStr("  Description [" + h.description + "]: ");
  std::string cat = promptStr("  Category [" + h.category + "]: ");
  manager_.editHabit(id, name, desc, cat);
  std::cout << BRIGHT_GREEN << "\n  Habit updated!\n" << RESET;
  pressEnterToContinue();
}

void TerminalUI::deleteHabit() {
  printHeader("🗑️   Delete Habit");
  listHabits();
  int id = promptInt("  Enter habit ID to delete (0 to cancel): ");
  if (id == 0)
    return;
  std::string confirm = promptStr("  Type YES to confirm deletion: ");
  if (confirm != "YES") {
    std::cout << YELLOW << "\n  Cancelled.\n" << RESET;
    pressEnterToContinue();
    return;
  }
  if (manager_.deleteHabit(id)) {
    std::cout << BRIGHT_GREEN << "\n  Habit deleted.\n" << RESET;
  } else {
    std::cout << RED << "\n  Habit not found.\n" << RESET;
  }
  pressEnterToContinue();
}

void TerminalUI::archiveHabit() {
  printHeader("📦  Archive Habit");
  auto habits = manager_.getActiveHabits();
  for (const auto &h : habits) {
    std::cout << "  [" << BRIGHT_CYAN << h.id << RESET << "] " << h.name
              << "\n";
  }
  int id = promptInt("\n  Enter habit ID to archive (0 to cancel): ");
  if (id == 0)
    return;
  if (manager_.archiveHabit(id)) {
    std::cout << BRIGHT_GREEN << "\n  Habit archived.\n" << RESET;
  } else {
    std::cout << RED << "\n  Habit not found.\n" << RESET;
  }
  pressEnterToContinue();
}

void TerminalUI::showWeeklyReport() {
  printHeader("📅  Weekly Report (Last 7 Days)");
  auto reports = stats_.getWeeklyReports();
  if (reports.empty()) {
    std::cout << BRIGHT_YELLOW << "\n  No active habits.\n" << RESET;
    pressEnterToContinue();
    return;
  }
  for (const auto &r : reports) {
    std::cout << "\n  " << BOLD << BRIGHT_WHITE << r.habitName << RESET << "\n";
    std::cout << "  ";
    for (const auto &label : r.dayLabels) {
      std::cout << std::setw(5) << label;
    }
    std::cout << "\n  ";
    for (bool done : r.days) {
      std::cout << std::setw(5) << (done ? "✅" : "❌");
    }
    std::cout << "\n  " << DIM << r.completedCount << "/7 days" << RESET
              << "\n";
    printSeparator(40);
  }
  pressEnterToContinue();
}

void TerminalUI::showMonthlyReport() {
  printHeader("📆  Monthly Report");
  auto reports = stats_.getMonthlyReports();
  if (reports.empty()) {
    std::cout << BRIGHT_YELLOW << "\n  No active habits.\n" << RESET;
    pressEnterToContinue();
    return;
  }
  for (const auto &r : reports) {
    std::cout << "\n  " << BOLD << BRIGHT_WHITE << std::left << std::setw(22)
              << r.habitName << RESET;
    std::cout << " " << GREEN << progressBar(r.percentage, 20) << RESET;
    std::cout << " " << BRIGHT_YELLOW << std::fixed << std::setprecision(0)
              << r.percentage << "%" << RESET;
    std::cout << "  " << DIM << "(" << r.completedDays << "/" << r.totalDays
              << ")" << RESET << "\n";
  }

  // best habits
  auto best = stats_.getBestHabits();
  if (!best.empty()) {
    std::cout << "\n  " << BOLD << "🏆 Best Habits This Month:\n" << RESET;
    int rank = 1;
    for (const auto &b : best) {
      if (rank > 3)
        break;
      std::cout << "  " << BRIGHT_YELLOW << rank++ << ". " << RESET << b.first
                << "  " << DIM << std::fixed << std::setprecision(0) << b.second
                << "%" << RESET << "\n";
    }
  }
  pressEnterToContinue();
}

void TerminalUI::showHistory() {
  printHeader("📜  Habit History (30 Days)");
  auto habits = manager_.getActiveHabits();
  if (habits.empty()) {
    std::cout << BRIGHT_YELLOW << "\n  No active habits.\n" << RESET;
    pressEnterToContinue();
    return;
  }
  std::cout << "\n  Select habit (0 = all):\n";
  for (const auto &h : habits) {
    std::cout << "  [" << BRIGHT_CYAN << h.id << RESET << "] " << h.name
              << "\n";
  }
  int id = promptInt("\n  Habit ID: ");

  if (id == 0) {
    for (const auto &h : habits) {
      std::cout << "\n  " << BOLD << BRIGHT_WHITE << h.name << RESET << "\n";
      auto lines = stats_.getHistory(h.id, 30);
      // Show in columns of 3
      for (std::size_t i = 0; i < lines.size(); ++i) {
        if (i % 3 == 0)
          std::cout << "  ";
        std::cout << std::setw(26) << lines[i];
        if ((i + 1) % 3 == 0)
          std::cout << "\n";
      }
      std::cout << "\n";
    }
  } else {
    auto opt = manager_.findHabit(id);
    if (!opt) {
      std::cout << RED << "\n  Habit not found.\n" << RESET;
      pressEnterToContinue();
      return;
    }
    std::cout << "\n  " << BOLD << BRIGHT_WHITE << opt->get().name << RESET
              << "\n";
    auto lines = stats_.getHistory(id, 30);
    for (std::size_t i = 0; i < lines.size(); ++i) {
      if (i % 3 == 0)
        std::cout << "  ";
      std::cout << std::setw(26) << lines[i];
      if ((i + 1) % 3 == 0)
        std::cout << "\n";
    }
    std::cout << "\n";
  }
  pressEnterToContinue();
}
