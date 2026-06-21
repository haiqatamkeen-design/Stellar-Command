/* STELLAR COMMAND - Qt GUI entry point */
#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow w;
    w.resize(1024, 660);
    w.setWindowTitle("STELLAR COMMAND - Mission Control");
    w.show();
    return app.exec();
}
