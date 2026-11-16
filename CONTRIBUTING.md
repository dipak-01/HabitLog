# Contributing to HabitLog

Thank you for considering contributing! Here's how to get started.

## 🛠 Development Setup

```bash
git clone https://github.com/dipak-01/HabitLog.git
cd HabitLog
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
```

## 🧪 Running Tests

```bash
cd build
./test_habits
```

## 📐 Code Style

This project uses Google C++ style with 4-space indentation. Format before committing:

```bash
clang-format -i src/**/*.cpp include/**/*.h
```

## 🌿 Branching

- `main` — stable, always builds
- `feat/<name>` — new features
- `fix/<name>` — bug fixes

## 📬 Pull Requests

1. Fork the repo
2. Create a branch: `git checkout -b feat/my-feature`
3. Commit your changes with clear messages
4. Push and open a PR against `main`

## 💡 Ideas for Contributions

- [ ] Habit frequency support (weekly, monthly)
- [ ] Export to CSV
- [ ] Colour themes
- [ ] Reminder notifications via cron
- [ ] Multi-user support
