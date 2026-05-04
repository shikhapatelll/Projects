# StudyFlow console based app

StudyFlow is a console-based C++ study planner that helps students create accounts, manage study items, auto-generate study sessions, track completed work, and view reminders and analytics.

## Implemented user stories
- US1–US20

## Requirements
- C++17 compiler
- CMake 3.16+
- Doxygen (for generating HTML documentation before submission)

## Build
```bash
mkdir -p build
cd build
cmake ..
cmake --build .
```

## Run
```bash
./studyflow_app
```

## Main commands
```text
signup <username> <password>
login <username> <password>
logout
help
exit

add --title "..." --type exam|assignment|reading|task|quiz --course CS3307 --due YYYY-MM-DD --hours 5 --priority 3 [--desc "..."]
edit <itemId> [--title "..."] [--type ...] [--course ...] [--due YYYY-MM-DD] [--hours N] [--priority 1..5] [--desc "..."]
delete <itemId>
list
search --query "text"
filter [--type exam] [--priority 3] [--course CS3307] [--from YYYY-MM-DD] [--to YYYY-MM-DD]

today
week
history
history --from YYYY-MM-DD --to YYYY-MM-DD
start-session <sessionId>
complete <sessionId>
left
priority

settings
setsession <hours>
setmaxhours <hours>
setweekends on|off
regen
analytics

reminders
save
load
```

## Data storage
- Credentials: `data/credentials.json`
- Per-user data: `data/users/<username>.json`
- Atomic save uses `.tmp` and `.bak` files to protect against corrupted saves.

## Doxygen documentation
1. Make sure Doxygen is installed.
2. Generate the documentation using:
   ```bash
   doxygen dox.config
   ```
3. The HTML output will be written under `docs/html/`.

The starter `dox.config` file in this submission already includes the required project metadata and extraction settings.
