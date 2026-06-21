#include "mainwindow.h"

#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QToolButton>
#include <QLabel>
#include <QTextEdit>
#include <QInputDialog>
#include <QMessageBox>
#include <QLineEdit>
#include <QPainter>
#include <QBrush>
#include <QGraphicsDropShadowEffect>
#include <QIcon>

static QString qs(const string& s) { return QString::fromStdString(s); }
static double rand01() { return randomInt(1000) / 1000.0; }

/*--------------------- animated star background -------------------*/
Starfield::Starfield(QWidget* parent) : QWidget(parent) {
    for (int i = 0; i < COUNT; i++) {
        stars[i].x = rand01();
        stars[i].y = rand01();
        stars[i].speed = 0.0004 + rand01() * 0.0022;
        stars[i].size = 0.8 + rand01() * 1.8;
    }
    connect(&timer, &QTimer::timeout, this, [this] {
        for (int i = 0; i < COUNT; i++) {
            stars[i].y += stars[i].speed;
            if (stars[i].y > 1.0) { stars[i].y = 0.0; stars[i].x = rand01(); }
        }
        update();
    });
    timer.start(40);
}

void Starfield::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    QLinearGradient g(0, 0, 0, height());
    g.setColorAt(0.0, QColor(13, 18, 40));
    g.setColorAt(1.0, QColor(3, 5, 14));
    p.fillRect(rect(), g);
    p.setPen(Qt::NoPen);
    for (int i = 0; i < COUNT; i++) {
        int alpha = 90 + (int)(150.0 * (stars[i].speed / 0.0026));
        p.setBrush(QColor(190, 225, 255, alpha));
        p.drawEllipse(QPointF(stars[i].x * width(), stars[i].y * height()),
                      stars[i].size, stars[i].size);
    }
}

/*--------------------------- main window --------------------------*/
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setStyleSheet(
        "#stack, #loginPage, #opsPage, #dutyPanel { background: transparent; }"
        "QLabel { background: transparent; color: #cfe8ff; font-size: 14px; }"
        "QPushButton, QToolButton {"
        "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "    stop:0 #1d4a85, stop:1 #0f2b50);"
        "  color: #eaf6ff; border: 1px solid #2e75b6; border-radius: 10px;"
        "  padding: 10px 16px; font-size: 14px; font-weight: bold; }"
        "QPushButton:hover, QToolButton:hover {"
        "  border: 2px solid #57c7ff;"
        "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "    stop:0 #2a62a8, stop:1 #16396a); }"
        "QPushButton:pressed, QToolButton:pressed { background: #2e75b6; }"
        "QTextEdit { background-color: rgba(4, 7, 16, 215); color: #bfe3ff;"
        "  border: 1px solid #2e75b6; border-radius: 10px; padding: 8px;"
        "  font-family: Consolas, 'Courier New', monospace; font-size: 13px; }"
        "QDialog, QMessageBox, QInputDialog { background-color: #0e1630;"
        "  border: 1px solid #2e75b6; }"
        "QDialog QLabel, QMessageBox QLabel, QInputDialog QLabel {"
        "  color: #eaf6ff; font-size: 14px; font-weight: bold;"
        "  background: transparent; }"
        "QLineEdit { background-color: #0a1228; color: #eaf6ff;"
        "  border: 1px solid #2e75b6; border-radius: 6px; padding: 7px;"
        "  font-size: 14px; selection-background-color: #2e75b6; }"
        "QLineEdit:focus { border: 2px solid #57c7ff; }"
        "QSpinBox, QDoubleSpinBox, QComboBox {"
        "  background-color: #0a1228; color: #eaf6ff;"
        "  border: 1px solid #2e75b6; border-radius: 6px; padding: 5px;"
        "  font-size: 14px; }"
        "QComboBox QAbstractItemView { background-color: #0e1630;"
        "  color: #eaf6ff; selection-background-color: #2e75b6; }");

    Starfield *bg = new Starfield(this);
    setCentralWidget(bg);
    QVBoxLayout *base = new QVBoxLayout(bg);
    base->setContentsMargins(0, 0, 0, 0);

    stack = new QStackedWidget;
    stack->setObjectName("stack");
    base->addWidget(stack);

    buildLoginPage();
    buildOpsPage();
    stack->setCurrentWidget(loginPage);
}

void MainWindow::buildLoginPage() {
    loginPage = new QWidget;
    loginPage->setObjectName("loginPage");
    QVBoxLayout *lay = new QVBoxLayout(loginPage);
    lay->addStretch();

    QLabel *title = new QLabel("STELLAR  COMMAND");
    title->setStyleSheet("font-size: 46px; font-weight: bold; color: #57c7ff;"
                         "letter-spacing: 10px;");
    title->setAlignment(Qt::AlignCenter);
    QGraphicsDropShadowEffect *glow = new QGraphicsDropShadowEffect;
    glow->setBlurRadius(45);
    glow->setOffset(0, 0);
    glow->setColor(QColor(87, 199, 255, 220));
    title->setGraphicsEffect(glow);
    lay->addWidget(title);

    QLabel *sub = new QLabel("SPACE  MISSION  CONTROL  SYSTEM");
    sub->setStyleSheet("font-size: 15px; color: #8fb8d8; letter-spacing: 4px;");
    sub->setAlignment(Qt::AlignCenter);
    lay->addWidget(sub);
    lay->addSpacing(36);

    QLabel *who = new QLabel("Who are you, operator?  Select your station:");
    who->setStyleSheet("font-size: 16px;");
    who->setAlignment(Qt::AlignCenter);
    lay->addWidget(who);
    lay->addSpacing(14);

    const QString icons[4] = { ":/icons/rocket.svg", ":/icons/radar.svg",
                               ":/icons/wrench.svg", ":/icons/flask.svg" };
    QHBoxLayout *row = new QHBoxLayout;
    row->addStretch();
    for (int i = 0; i < 4; i++) {
        QToolButton *b = new QToolButton;
        b->setText(qs(ROLES[i]));
        b->setIcon(QIcon(icons[i]));
        b->setIconSize(QSize(44, 44));
        b->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        b->setMinimumSize(170, 120);
        b->setToolTip(qs(DUTIES[i]));
        connect(b, &QToolButton::clicked, this, [this, i] { doLogin(i); });
        row->addWidget(b);
        row->addSpacing(14);
    }
    row->addStretch();
    lay->addLayout(row);

    lay->addSpacing(26);
    QLabel *hint = new QLabel("First login?  You will create your own 4-digit PIN.");
    hint->setStyleSheet("color: #6f93b5; font-size: 12px;");
    hint->setAlignment(Qt::AlignCenter);
    lay->addWidget(hint);
    lay->addStretch();

    stack->addWidget(loginPage);
}

void MainWindow::buildOpsPage() {
    opsPage = new QWidget;
    opsPage->setObjectName("opsPage");
    QVBoxLayout *outer = new QVBoxLayout(opsPage);
    outer->setContentsMargins(14, 14, 14, 14);

    QHBoxLayout *top = new QHBoxLayout;
    roleLabel = new QLabel("-");
    roleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #57c7ff;");
    dutyLabel = new QLabel("-");
    dutyLabel->setStyleSheet("color: #ffd866; font-size: 13px;");
    top->addWidget(roleLabel);
    top->addStretch();
    top->addWidget(dutyLabel);
    outer->addLayout(top);
    outer->addSpacing(8);

    QHBoxLayout *body = new QHBoxLayout;
    QWidget *dutyPanel = new QWidget;
    dutyPanel->setObjectName("dutyPanel");
    dutyLayout = new QVBoxLayout(dutyPanel);
    dutyLayout->setSpacing(10);
    dutyPanel->setFixedWidth(250);
    body->addWidget(dutyPanel);

    console = new QTextEdit;
    console->setReadOnly(true);
    body->addWidget(console, 1);
    outer->addLayout(body, 1);

    stack->addWidget(opsPage);
}

/*----------------------------- helpers ----------------------------*/
void MainWindow::say(const QString& html) {
    console->append(html);
    console->append("");
}
void MainWindow::sayInfo(const QString& text) {
    say("<span style='color:#41d97e;'>" + text.toHtmlEscaped() + "</span>");
}
void MainWindow::sayError(const QString& text) {
    say("<span style='color:#ff6b6b; font-weight:bold;'>! "
        + text.toHtmlEscaped() + "</span>");
}

QString MainWindow::fleetHtml() {
    if (game.count() == 0)
        return "<i style='color:#8fb8d8;'>Fleet is empty. "
               "Ask the Engineer to build a spacecraft.</i>";
    QString h = "<table cellpadding='6'>"
        "<tr style='color:#57c7ff;'><td><b>ID</b></td><td><b>Name</b></td>"
        "<td><b>Type</b></td><td><b>Status</b></td><td><b>Fuel</b></td>"
        "<td><b>Details</b></td></tr>";
    for (int i = 0; i < game.count(); i++) {
        SpaceCraft* sc = game.at(i);
        int pct = (int)sc->getFuel().getPercent();
        int blocks = pct / 10;
        QString barColor = (pct > 60) ? "#41d97e" : (pct > 30) ? "#ffd866" : "#ff6b6b";
        QString bar = "<span style='color:" + barColor + ";'>";
        for (int b = 0; b < blocks; b++) bar += "&#9608;";
        bar += "</span><span style='color:#1d2c49;'>";
        for (int b = blocks; b < 10; b++) bar += "&#9608;";
        bar += "</span> " + QString::number(pct) + "%";

        QString type = qs(sc->getType());
        QString tag = (type == "Satellite") ? "SAT" : (type == "Rover") ? "ROV" : "PRB";
        QString badge = sc->isOnMission()
            ? "<b style='color:#ffd866;'>ON MISSION</b>"
            : "<b style='color:#41d97e;'>DOCKED</b>";
        h += "<tr style='color:#cfe8ff;'><td>" + QString::number(sc->getId())
           + "</td><td><b>" + qs(sc->getName()) + "</b></td>"
           + "<td><span style='color:#57c7ff;'>[" + tag + "]</span> " + type + "</td>"
           + "<td>" + badge + "</td>"
           + "<td style='font-family:Consolas;'>" + bar + "</td>"
           + "<td style='color:#8fb8d8;'>" + qs(sc->detailLine()) + "</td></tr>";
    }
    return h + "</table>";
}

QString MainWindow::askPin(const QString& title, const QString& label, bool* okOut) {
    while (true) {
        bool ok = false;
        QString p = QInputDialog::getText(this, title, label,
                                          QLineEdit::Password, "", &ok);
        if (!ok) { *okOut = false; return ""; }
        bool valid = (p.size() == 4);
        for (QChar c : p) if (!c.isDigit()) valid = false;
        if (valid) { *okOut = true; return p; }
        QMessageBox::warning(this, "Invalid PIN", "PIN must be exactly 4 digits.");
    }
}

/*------------------------------ login -----------------------------*/
void MainWindow::doLogin(int roleIdx) {
    Worker* w = &game.getWorker(roleIdx);
    try {
        if (!w->hasPin()) {
            bool ok = false;
            QString p1 = askPin("Create PIN",
                "First login for the " + qs(w->getRole())
                + ".\nCreate your 4-digit PIN:", &ok);
            if (!ok) return;
            QString p2 = askPin("Confirm PIN", "Enter the same PIN again:", &ok);
            if (!ok) return;
            if (p1 != p2) {
                QMessageBox::warning(this, "PIN mismatch", "PINs did not match. Try again.");
                return;
            }
            w->setPin(p1.toStdString());
            QMessageBox::information(this, "PIN saved",
                "PIN saved for the " + qs(w->getRole()) + ".");
        }
        bool granted = false;
        for (int t = 0; t < 3 && !granted; t++) {
            bool ok = false;
            QString p = askPin("Crew Login",
                "Enter PIN for the " + qs(w->getRole()) + ":", &ok);
            if (!ok) return;
            granted = w->checkPin(p.toStdString());
            if (!granted && t < 2)
                QMessageBox::warning(this, "Wrong PIN",
                    QString("Wrong PIN (%1 attempts left)").arg(2 - t));
        }
        if (!granted)
            throw AccessDeniedException("3 wrong PIN attempts for the " + w->getRole() + ".");

        current = w;
        currentIdx = roleIdx;
        populateDuties();
        console->clear();
        roleLabel->setText(qs(w->getRole()) + "'s Console");
        dutyLabel->setText("Duty: " + qs(DUTIES[roleIdx]));
        say("<span style='color:#57c7ff; font-size:15px;'><b>Access granted. "
            "Welcome aboard, " + qs(w->getRole()) + ".</b></span>");
        stack->setCurrentWidget(opsPage);
    } catch (StellarException& e) {
        QMessageBox::critical(this, "Security", qs(e.what()));
    }
}

void MainWindow::populateDuties() {
    QLayoutItem* it;
    while ((it = dutyLayout->takeAt(0)) != nullptr) {
        if (it->widget()) delete it->widget();
        delete it;
    }
    auto add = [this](const QString& text, const QString& icon,
                      void (MainWindow::*slot)()) {
        QPushButton *b = new QPushButton("  " + text);
        b->setIcon(QIcon(icon));
        b->setIconSize(QSize(22, 22));
        b->setMinimumHeight(40);
        connect(b, &QPushButton::clicked, this, slot);
        dutyLayout->addWidget(b);
    };

    QLabel *cap = new QLabel("YOUR DUTIES");
    cap->setStyleSheet("color: #57c7ff; font-weight: bold; letter-spacing: 3px;");
    dutyLayout->addWidget(cap);

    string r = current->getRole();
    if (r == "Commander") {
        add("Launch Mission",   ":/icons/rocket.svg",  &MainWindow::onLaunch);
        add("Handle Emergency", ":/icons/alert.svg",   &MainWindow::onEmergency);
    } else if (r == "Pilot") {
        add("Monitor Missions", ":/icons/radar.svg",   &MainWindow::onMonitor);
        add("Advance One Day",  ":/icons/advance.svg", &MainWindow::onAdvance);
    } else if (r == "Engineer") {
        add("Build Spacecraft", ":/icons/wrench.svg",  &MainWindow::onBuild);
        add("Scrap Spacecraft", ":/icons/trash.svg",   &MainWindow::onScrap);
        add("Refuel Spacecraft",":/icons/fuel.svg",    &MainWindow::onRefuel);
    } else {                           // Scientist
        add("View Mission Log", ":/icons/book.svg",    &MainWindow::onLog);
        add("Add Science Note", ":/icons/pencil.svg",  &MainWindow::onNote);
    }
    add("View Fleet", ":/icons/fleet.svg", &MainWindow::onFleet);
    dutyLayout->addStretch();
    add("Log Out", ":/icons/logout.svg", &MainWindow::onLogout);
}

/*--------------------------- duty actions -------------------------*/
void MainWindow::onFleet() { say(fleetHtml()); }

void MainWindow::onBuild() {
    try {
        bool ok = false;
        QStringList types = { "Satellite", "Rover", "Probe" };
        QString t = QInputDialog::getItem(this, "Shipyard",
            "Choose spacecraft type:", types, 0, false, &ok);
        if (!ok) return;
        QString name = QInputDialog::getText(this, "Shipyard",
            "Name of the craft:", QLineEdit::Normal, "", &ok);
        if (!ok || name.isEmpty()) return;

        if (t == "Satellite") {
            double alt = QInputDialog::getDouble(this, "Satellite",
                "Orbit altitude (100-40000 km):", 550, 100, 40000, 0, &ok);
            if (!ok) return;
            sayInfo(qs(game.buildSatellite(current, name.toStdString(), alt)));
        } else if (t == "Rover") {
            double sp = QInputDialog::getDouble(this, "Rover",
                "Speed (1-50 km/h):", 15, 1, 50, 0, &ok);
            if (!ok) return;
            QString terrain = QInputDialog::getText(this, "Rover",
                "Terrain type:", QLineEdit::Normal, "rocky", &ok);
            if (!ok || terrain.isEmpty()) return;
            sayInfo(qs(game.buildRover(current, name.toStdString(),
                                       sp, terrain.toStdString())));
        } else {
            QString target = QInputDialog::getText(this, "Probe",
                "Target body:", QLineEdit::Normal, "Europa", &ok);
            if (!ok || target.isEmpty()) return;
            sayInfo(qs(game.buildProbe(current, name.toStdString(),
                                       target.toStdString())));
        }
        say(fleetHtml());
    } catch (StellarException& e) { sayError(qs(e.what())); }
}

void MainWindow::onScrap() {
    try {
        say(fleetHtml());
        bool ok = false;
        int id = QInputDialog::getInt(this, "Scrap Spacecraft",
            "Spacecraft ID:", 1, 1, 999, 1, &ok);
        if (!ok) return;
        sayInfo(qs(game.scrap(current, id)));
    } catch (StellarException& e) { sayError(qs(e.what())); }
}

void MainWindow::onRefuel() {
    try {
        say(fleetHtml());
        bool ok = false;
        int id = QInputDialog::getInt(this, "Refuel Spacecraft",
            "Spacecraft ID:", 1, 1, 999, 1, &ok);
        if (!ok) return;
        sayInfo(qs(game.refuel(current, id)));
        say(fleetHtml());
    } catch (StellarException& e) { sayError(qs(e.what())); }
}

void MainWindow::onLaunch() {
    try {
        say(fleetHtml());
        bool ok = false;
        int id = QInputDialog::getInt(this, "Launch Mission",
            "Spacecraft ID:", 1, 1, 999, 1, &ok);
        if (!ok) return;
        QString mname = QInputDialog::getText(this, "Launch Mission",
            "Mission name:", QLineEdit::Normal, "", &ok);
        if (!ok || mname.isEmpty()) return;
        int days = QInputDialog::getInt(this, "Launch Mission",
            "Duration in days (1-30):", 3, 1, 30, 1, &ok);
        if (!ok) return;
        say("<span style='color:#57c7ff; font-size:15px;'><b>"
            + qs(game.launch(current, id, mname.toStdString(), days))
            + "</b></span>");
    } catch (StellarException& e) { sayError(qs(e.what())); }
}

void MainWindow::onEmergency() {
    try {
        QString report = qs(game.emergency(current));
        if (report.startsWith("EMERGENCY"))
            say("<span style='color:#ff6b6b; font-size:15px;'><b>"
                + report.toHtmlEscaped().replace("\n", "<br>") + "</b></span>");
        else
            sayInfo(report);
    } catch (StellarException& e) { sayError(qs(e.what())); }
}

void MainWindow::onMonitor() {
    try {
        game.checkRole(current, "Pilot");
        QString h = "<b style='color:#57c7ff;'>Active Missions</b><br>";
        bool any = false;
        for (int i = 0; i < game.count(); i++) {
            SpaceCraft* sc = game.at(i);
            if (!sc->isOnMission()) continue;
            any = true;
            Mission& m = sc->getMission();
            int done = (m.getDuration() > 0)
                       ? (m.getDaysPassed() * 10) / m.getDuration() : 0;
            QString bar = "<span style='color:#57c7ff;'>";
            for (int b = 0; b < done; b++) bar += "&#9608;";
            bar += "</span><span style='color:#1d2c49;'>";
            for (int b = done; b < 10; b++) bar += "&#9608;";
            bar += "</span>";
            h += "<b>" + qs(m.getName()) + "</b> on " + qs(sc->getName())
               + " &nbsp; Day " + QString::number(m.getDaysPassed())
               + " of " + QString::number(m.getDuration())
               + " &nbsp; <span style='font-family:Consolas;'>" + bar + "</span>"
               + " &nbsp; Fuel: " + QString::number((int)sc->getFuel().getLevel())
               + "<br>";
        }
        if (!any) h += "<i style='color:#8fb8d8;'>No active missions.</i>";
        say(h);
    } catch (StellarException& e) { sayError(qs(e.what())); }
}

void MainWindow::onAdvance() {
    try {
        QString r = qs(game.advanceDay(current)).toHtmlEscaped();
        r.replace("\n", "<br>");
        r.replace("completed successfully!",
                  "<b style='color:#41d97e;'>completed successfully!</b>");
        r.replace("failed.", "<b style='color:#ff6b6b;'>failed.</b>");
        say(r);
    } catch (StellarException& e) { sayError(qs(e.what())); }
}

void MainWindow::onLog() {
    try {
        QString r = qs(game.logText(current)).toHtmlEscaped();
        say("<span style='color:#bfe3ff;'>" + r.replace("\n", "<br>") + "</span>");
    } catch (StellarException& e) { sayError(qs(e.what())); }
}

void MainWindow::onNote() {
    try {
        bool ok = false;
        QString note = QInputDialog::getText(this, "Science Note",
            "Your note for the mission log:", QLineEdit::Normal, "", &ok);
        if (!ok || note.isEmpty()) return;
        sayInfo(qs(game.addNote(current, note.toStdString())));
    } catch (StellarException& e) { sayError(qs(e.what())); }
}

void MainWindow::onLogout() {
    current = nullptr;
    currentIdx = -1;
    stack->setCurrentWidget(loginPage);
}
