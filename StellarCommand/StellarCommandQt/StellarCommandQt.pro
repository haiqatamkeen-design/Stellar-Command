# STELLAR COMMAND - Qt GUI
# Open in Qt Creator -> Configure Project -> Run
QT       += widgets svg
CONFIG   += c++17
TARGET    = StellarCommandQt
TEMPLATE  = app

SOURCES  += main.cpp \
            mainwindow.cpp

HEADERS  += engine.h \
            mainwindow.h

RESOURCES += icons.qrc
