# STELLAR COMMAND — Qt GUI (Visual Edition)

A windowed version of the console project with attention-grabbing
visuals, built on the SAME simple engine classes as main.cpp.

## Visual features
- Animated drifting starfield background (custom painted widget, 25 fps)
- Glowing "STELLAR COMMAND" title (drop-shadow glow effect)
- Role selection cards with hand-drawn SVG icons (no emojis, no images
  from the internet - 13 small .svg files in icons/)
- Gradient buttons with hover glow
- Fleet table with live color-coded fuel bars (green / yellow / red)
- Mission progress bars while monitoring
- Red alert styling for emergencies, green for successes

## How to run (one-time setup)
1. Install Qt (free): https://www.qt.io/download-qt-installer
   - In the installer pick the default "Qt for desktop" MinGW kit.
   - The standard install includes the Qt SVG module this project uses.
2. Open `StellarCommandQt.pro` in Qt Creator.
3. Click "Configure Project", then press the green Run button (Ctrl+R).

## Files
| File | Purpose |
|---|---|
| engine.h | Same game classes as ../main.cpp (pure C++, no Qt) |
| mainwindow.h/.cpp | The window: starfield, login cards, duty panels |
| icons/*.svg | 13 hand-written vector icons |
| icons.qrc | Qt resource list bundling the icons into the .exe |
| StellarCommandQt.pro / CMakeLists.txt | Project files (qmake / CMake) |

## Flow (same as console version)
Pick your role card -> create your 4-digit PIN on first login ->
3-attempt PIN check -> your duties appear as icon buttons ->
results display in the mission console panel.

## For the viva
- engine.h has zero Qt in it: the OOP design is independent of the
  interface ("separation of concerns") - same engine, two front ends.
- The console main.cpp stays the primary submission; this is bonus.
