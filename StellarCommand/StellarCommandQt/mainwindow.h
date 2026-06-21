#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QTimer>
#include "engine.h"

class QTextEdit;
class QLabel;
class QStackedWidget;
class QVBoxLayout;

class Starfield : public QWidget {
public:
    explicit Starfield(QWidget* parent = nullptr);
protected:
    void paintEvent(QPaintEvent*) override;
private:
    struct Star { double x; double y; double speed; double size; };
    static const int COUNT = 150;
    Star stars[COUNT];
    QTimer timer;
};

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    MissionControl game;
    Worker* current = nullptr;
    int currentIdx = -1;

    QStackedWidget *stack;
    QWidget *loginPage;
    QWidget *opsPage;
    QTextEdit *console;
    QLabel *roleLabel;
    QLabel *dutyLabel;
    QVBoxLayout *dutyLayout;

    void buildLoginPage();
    void buildOpsPage();
    void say(const QString& html);
    void sayInfo(const QString& text);
    void sayError(const QString& text);
    QString fleetHtml();
    QString askPin(const QString& title, const QString& label, bool* okOut);
    void doLogin(int roleIdx);
    void populateDuties();

    void onLaunch();
    void onEmergency();
    void onFleet();
    void onMonitor();
    void onAdvance();
    void onBuild();
    void onScrap();
    void onRefuel();
    void onLog();
    void onNote();
    void onLogout();
};

#endif
