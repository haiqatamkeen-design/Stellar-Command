# Stellar Command — Spacecraft Maintenance System

A spacecraft maintenance management system built in C++ with a graphical user interface (GUI) developed using the Qt framework. It tracks spacecraft components, schedules maintenance tasks, and logs operational status, applying object-oriented programming principles for a clean, modular design.

## Features

- Add, view, and manage spacecraft components
- Schedule and track maintenance activities
- Log and update operational status of systems
- File-based data persistence so records are saved between sessions
- User-friendly graphical interface built with Qt

## Tech Stack

- **Language:** C++
- **GUI Framework:** Qt
- **Concepts:** Object-oriented programming (classes, encapsulation), file handling

## How to Run (GUI — Qt)

**Option 1: Using Qt Creator (easiest)**

1. Install [Qt](https://www.qt.io/download) and Qt Creator.
2. Open Qt Creator and choose **File → Open File or Project**, then select the project file (`.pro` or `CMakeLists.txt`).
3. Select a build kit (compiler) when prompted.
4. Click the green **Run** button (or press `Ctrl + R`) to build and launch the application.

**Option 2: From the command line**

1. Make sure Qt is installed and added to your system PATH.
2. From the project folder, build and run:
   ```
   qmake
   make
   ./StellarCommand        # On Windows: StellarCommand.exe
   ```

> Note: Replace the project/executable names above with your actual file names if they differ.

## Future Improvements

- Database integration in place of file storage
- Automated maintenance reminders and reporting
- Search and filtering of components and maintenance logs

## Author

**Haiqa Tamkeen** — BS Computer Science, Institute of Space Technology, Islamabad
GitHub: [haiqatamkeen-design](https://github.com/haiqatamkeen-design)
