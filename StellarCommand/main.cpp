#include <iostream>
#include <string>
using namespace std;

unsigned long seed = 12345;
int randomInt(int n) {
    seed = seed * 1103515245 + 12345;
    return (int)((seed / 65536) % n);
}

class StellarException {
protected:
    string message;
public:
    StellarException(string msg) { message = msg; }
    string what() { return message; }
};

class FuelException : public StellarException {
public:
    FuelException(string msg) : StellarException("Fuel Error: " + msg) {}
};

class AccessDeniedException : public StellarException {
public:
    AccessDeniedException(string msg) : StellarException("Access Denied: " + msg) {}
};

template <typename T>
T readNumber(string prompt, T minVal, T maxVal) {
    T value;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= minVal && value <= maxVal) {
            cin.ignore(1000, '\n');
            return value;
        }
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "  Please enter a number between " << minVal << " and " << maxVal << ".\n";
    }
}

string readLine(string prompt) {
    string text;
    while (true) {
        cout << prompt;
        getline(cin, text);
        if (text != "") return text;
        cout << "  Input cannot be empty.\n";
    }
}

string readPin(string prompt) {
    while (true) {
        string p = readLine(prompt);
        bool ok = (p.size() == 4);
        for (char c : p)
            if (c < '0' || c > '9') ok = false;
        if (ok) return p;
        cout << "  PIN must be exactly 4 digits.\n";
    }
}

const string ROLES[4] = { "Commander", "Pilot", "Engineer", "Scientist" };
const string DUTIES[4] = {
    "Launches missions and handles emergencies",
    "Monitors missions and advances the day",
    "Builds and refuels spacecraft",
    "Keeps the mission log" };

class Worker {
private:
    string role;
    string pin;
public:
    Worker(string r = "?") { role = r; pin = ""; }
    string getRole() { return role; }
    bool hasPin() { return pin != ""; }
    void setPin(string p) { pin = p; }
    bool checkPin(string p) { return p == pin; }
};

class FuelSystem {
private:
    double level;
    double capacity;
    double burnRate;
public:
    FuelSystem(double cap = 100, double rate = 5) {
        capacity = cap;
        level = cap;
        burnRate = rate;
    }
    double getLevel() { return level; }
    void burnOneDay() {
        if (burnRate > level)
            throw FuelException("tank is empty!");
        level = level - burnRate;
    }
    void refuel() { level = capacity; }
    double neededFor(int days) { return burnRate * days; }
};

class Mission {
private:
    string name;
    int duration;
    int daysPassed;
public:
    Mission() { name = "-"; duration = 0; daysPassed = 0; }
    Mission(string n, int d) { name = n; duration = d; daysPassed = 0; }
    string getName() { return name; }
    int getDuration() { return duration; }
    int getDaysPassed() { return daysPassed; }
    void passDay() { daysPassed++; }
    bool isFinished() { return daysPassed >= duration; }
};

class SpaceCraft {
protected:
    int id;
    string name;
    bool onMission;
    FuelSystem fuel;
    Mission mission;
public:
    SpaceCraft(int i, string n, double cap, double rate) : fuel(cap, rate) {
        id = i;
        name = n;
        onMission = false;
    }
    virtual ~SpaceCraft() {}

    virtual string getType() = 0;
    virtual void showDetails() = 0;
    virtual string emergencyAction() = 0;

    int getId() { return id; }
    string getName() { return name; }
    bool isOnMission() { return onMission; }
    FuelSystem& getFuel() { return fuel; }
    Mission& getMission() { return mission; }

    void launch(Mission m) {
        if (onMission)
            throw StellarException(name + " is already on a mission.");
        if (fuel.neededFor(m.getDuration()) > fuel.getLevel())
            throw FuelException(name + " does not have enough fuel. Ask the Engineer to refuel it.");
        mission = m;
        onMission = true;
    }
    void finishMission() { onMission = false; }

    bool operator==(SpaceCraft& other) { return id == other.id; }

    friend ostream& operator<<(ostream& os, SpaceCraft& sc);
};

ostream& operator<<(ostream& os, SpaceCraft& sc) {
    os << "  [" << sc.id << "] " << sc.name << " (" << sc.getType() << ") - "
       << (sc.onMission ? "On Mission" : "Docked")
       << " - Fuel: " << sc.fuel.getLevel();
    return os;
}

class Satellite : public SpaceCraft {
private:
    double altitude;
public:
    Satellite(int i, string n, double alt) : SpaceCraft(i, n, 120, 4) {
        altitude = alt;
    }
    string getType() { return "Satellite"; }
    void showDetails() {
        cout << "      Orbit altitude: " << altitude << " km\n";
    }
    string emergencyAction() {
        return name + " switched to its backup antenna.";
    }
};

class Rover : public SpaceCraft {
private:
    double speed;
    string terrain;
public:
    Rover(int i, string n, double sp, string t) : SpaceCraft(i, n, 80, 6) {
        speed = sp;
        terrain = t;
    }
    string getType() { return "Rover"; }
    void showDetails() {
        cout << "      Speed: " << speed << " km/h on " << terrain << " terrain\n";
    }
    string emergencyAction() {
        return name + " parked safely and sealed the problem.";
    }
};

class Probe : public SpaceCraft {
private:
    string target;
public:
    Probe(int i, string n, string t) : SpaceCraft(i, n, 150, 3) {
        target = t;
    }
    string getType() { return "Probe"; }
    void showDetails() {
        cout << "      Target body: " << target << "\n";
    }
    string emergencyAction() {
        return name + " saved its data and waited for new orders.";
    }
};

class MissionControl {
private:
    SpaceCraft* fleet[10];
    int fleetCount;
    string logBook[50];
    int logCount;
    int nextId;
    Worker workers[4];

    SpaceCraft* findCraft(int id) {
        for (int i = 0; i < fleetCount; i++)
            if (fleet[i]->getId() == id) return fleet[i];
        throw StellarException("No spacecraft with ID " + to_string(id) + ".");
    }
    void addLog(string entry) {
        if (logCount < 50) logBook[logCount++] = entry;
    }
public:
    MissionControl() {
        fleetCount = 0;
        logCount = 0;
        nextId = 1;
        for (int i = 0; i < 4; i++) workers[i] = Worker(ROLES[i]);
    }
    ~MissionControl() {
        for (int i = 0; i < fleetCount; i++) delete fleet[i];
    }

    Worker& getWorker(int i) { return workers[i]; }

    void checkRole(Worker* w, string role) {
        string r = w->getRole();
        if (r != role)
            throw AccessDeniedException("only the " + role + " can do this.");
    }

    void viewFleet() {
        if (fleetCount == 0) {
            cout << "  Fleet is empty.\n";
            return;
        }
        cout << "\n  --- Fleet ---\n";
        for (int i = 0; i < fleetCount; i++) {
            cout << *fleet[i] << "\n";
            fleet[i]->showDetails();
        }
    }

    void buildCraft(Worker* w) {
        checkRole(w, "Engineer");
        if (fleetCount >= 10) {
            cout << "  Hangar is full.\n";
            return;
        }
        cout << "  1. Satellite  2. Rover  3. Probe\n";
        int type = readNumber<int>("  Type: ", 1, 3);
        string name = readLine("  Name: ");
        if (type == 1) {
            double alt = readNumber<double>("  Orbit altitude (km): ", 100, 40000);
            fleet[fleetCount++] = new Satellite(nextId, name, alt);
        } else if (type == 2) {
            double sp = readNumber<double>("  Speed (km/h): ", 1, 50);
            string terrain = readLine("  Terrain type: ");
            fleet[fleetCount++] = new Rover(nextId, name, sp, terrain);
        } else {
            string target = readLine("  Target body: ");
            fleet[fleetCount++] = new Probe(nextId, name, target);
        }
        addLog("BUILT " + name + " (by " + w->getRole() + ")");
        cout << "  " << name << " added to the fleet with ID " << nextId << ".\n";
        nextId++;
    }

    void scrapCraft(Worker* w) {
        checkRole(w, "Engineer");
        if (fleetCount == 0) {
            cout << "  Fleet is empty.\n";
            return;
        }
        viewFleet();
        int id = readNumber<int>("  ID to scrap: ", 1, 999);
        SpaceCraft* found = findCraft(id);
        if (found->isOnMission())
            throw StellarException("Cannot scrap a craft that is on a mission.");
        for (int i = 0; i < fleetCount; i++) {
            if (*fleet[i] == *found) {
                addLog("SCRAPPED " + fleet[i]->getName() + " (by " + w->getRole() + ")");
                cout << "  " << fleet[i]->getName() << " removed from the fleet.\n";
                delete fleet[i];
                for (int j = i; j < fleetCount - 1; j++)
                    fleet[j] = fleet[j + 1];
                fleetCount--;
                return;
            }
        }
    }

    void refuelCraft(Worker* w) {
        checkRole(w, "Engineer");
        if (fleetCount == 0) {
            cout << "  Fleet is empty.\n";
            return;
        }
        viewFleet();
        int id = readNumber<int>("  ID to refuel: ", 1, 999);
        SpaceCraft* sc = findCraft(id);
        if (sc->isOnMission())
            throw StellarException("Cannot refuel during a mission.");
        sc->getFuel().refuel();
        addLog("REFUELED " + sc->getName() + " (by " + w->getRole() + ")");
        cout << "  " << sc->getName() << " refueled to full.\n";
    }

    void launchMission(Worker* w) {
        checkRole(w, "Commander");
        if (fleetCount == 0) {
            cout << "  No spacecraft yet. Ask the Engineer to build one.\n";
            return;
        }
        viewFleet();
        int id = readNumber<int>("  Spacecraft ID: ", 1, 999);
        SpaceCraft* sc = findCraft(id);
        string mname = readLine("  Mission name: ");
        int days = readNumber<int>("  Duration (days): ", 1, 30);
        sc->launch(Mission(mname, days));
        addLog("LAUNCHED " + mname + " on " + sc->getName() + " (by " + w->getRole() + ")");
        cout << "  Mission " << mname << " launched on " << sc->getName() << "!\n";
    }

    void handleEmergency(Worker* w) {
        checkRole(w, "Commander");
        SpaceCraft* active[10];
        int n = 0;
        for (int i = 0; i < fleetCount; i++)
            if (fleet[i]->isOnMission()) active[n++] = fleet[i];
        if (n == 0) {
            cout << "  All quiet in space.\n";
            return;
        }
        SpaceCraft* sc = active[randomInt(n)];
        string events[3] = { "Fuel leak", "Solar storm", "Signal lost" };
        string ev = events[randomInt(3)];
        cout << "  EMERGENCY: " << ev << " on " << sc->getName() << "!\n";
        cout << "  Response: " << sc->emergencyAction() << "\n";
        addLog("EMERGENCY (" + ev + ") on " + sc->getName());
    }

    void monitorMissions(Worker* w) {
        checkRole(w, "Pilot");
        bool any = false;
        for (int i = 0; i < fleetCount; i++) {
            if (!fleet[i]->isOnMission()) continue;
            any = true;
            Mission& m = fleet[i]->getMission();
            cout << "  " << m.getName() << " on " << fleet[i]->getName()
                 << " - Day " << m.getDaysPassed() << " of " << m.getDuration()
                 << " - Fuel: " << fleet[i]->getFuel().getLevel() << "\n";
        }
        if (!any) cout << "  No active missions.\n";
    }

    void advanceDay(Worker* w) {
        checkRole(w, "Pilot");
        bool any = false;
        for (int i = 0; i < fleetCount; i++) {
            if (!fleet[i]->isOnMission()) continue;
            any = true;
            Mission& m = fleet[i]->getMission();
            try {
                fleet[i]->getFuel().burnOneDay();
                m.passDay();
                cout << "  Day " << m.getDaysPassed() << " of mission " << m.getName() << ".\n";
                if (m.isFinished()) {
                    fleet[i]->finishMission();
                    addLog("COMPLETED " + m.getName());
                    cout << "  Mission " << m.getName() << " completed successfully!\n";
                }
            } catch (FuelException& e) {
                fleet[i]->finishMission();
                addLog("FAILED " + m.getName() + " (out of fuel)");
                cout << "  " << e.what() << " Mission " << m.getName() << " failed.\n";
            }
        }
        if (!any) cout << "  No active missions.\n";
    }

    void viewLog(Worker* w) {
        checkRole(w, "Scientist");
        if (logCount == 0) {
            cout << "  Mission log is empty.\n";
            return;
        }
        cout << "\n  --- Mission Log ---\n";
        for (int i = 0; i < logCount; i++)
            cout << "  " << logBook[i] << "\n";
    }

    void addNote(Worker* w) {
        checkRole(w, "Scientist");
        string note = readLine("  Science note: ");
        addLog("NOTE: " + note + " (by " + w->getRole() + ")");
        cout << "  Note saved in the mission log.\n";
    }
};

void dutyMenu(MissionControl& mc, Worker* w) {
    while (true) {
        string r = w->getRole();
        cout << "\n  ===== " << r << "'s Duties =====\n";
        try {
            if (r == "Commander") {
                cout << "  1. Launch Mission\n  2. Handle Emergency\n"
                     << "  3. View Fleet\n  0. Log out\n";
                int c = readNumber<int>("  Choice: ", 0, 3);
                if (c == 1) mc.launchMission(w);
                else if (c == 2) mc.handleEmergency(w);
                else if (c == 3) mc.viewFleet();
                else return;
            } else if (r == "Pilot") {
                cout << "  1. Monitor Missions\n  2. Advance One Day\n  3. View Fleet\n  0. Log out\n";
                int c = readNumber<int>("  Choice: ", 0, 3);
                if (c == 1) mc.monitorMissions(w);
                else if (c == 2) mc.advanceDay(w);
                else if (c == 3) mc.viewFleet();
                else return;
            } else if (r == "Engineer") {
                cout << "  1. Build Spacecraft\n  2. Scrap Spacecraft\n  3. Refuel Spacecraft\n  4. View Fleet\n  0. Log out\n";
                int c = readNumber<int>("  Choice: ", 0, 4);
                if (c == 1) mc.buildCraft(w);
                else if (c == 2) mc.scrapCraft(w);
                else if (c == 3) mc.refuelCraft(w);
                else if (c == 4) mc.viewFleet();
                else return;
            } else {
                cout << "  1. View Mission Log\n  2. Add Science Note\n"
                     << "  3. View Fleet\n  0. Log out\n";
                int c = readNumber<int>("  Choice: ", 0, 3);
                if (c == 1) mc.viewLog(w);
                else if (c == 2) mc.addNote(w);
                else if (c == 3) mc.viewFleet();
                else return;
            }
        } catch (StellarException& e) {
            cout << "  " << e.what() << "\n";
        }
    }
}

int main() {
    MissionControl mc;
    cout << "\n  =====================================\n"
         << "     S T E L L A R   C O M M A N D\n"
         << "     Space Mission Control System\n"
         << "  =====================================\n";

    while (true) {
        cout << "\n  ----- Crew Login -----\n";
        for (int i = 0; i < 4; i++)
            cout << "  " << i + 1 << ". " << ROLES[i] << "\n";
        cout << "  0. Exit\n";
        int c = readNumber<int>("  Choice: ", 0, 4);
        if (c == 0) {
            cout << "  Mission Control signing off. Goodbye!\n";
            return 0;
        }

        Worker* w = &mc.getWorker(c - 1);

        if (!w->hasPin()) {
            cout << "  First login for the " << w->getRole() << ". Create your PIN.\n";
            string p1 = readPin("  New 4-digit PIN: ");
            string p2 = readPin("  Confirm PIN: ");
            if (p1 != p2) {
                cout << "  PINs did not match. Try again.\n";
                continue;
            }
            w->setPin(p1);
            cout << "  PIN saved.\n";
        }

        bool ok = false;
        for (int tries = 0; tries < 3 && !ok; tries++) {
            ok = w->checkPin(readPin("  Enter PIN: "));
            if (!ok) cout << "  Wrong PIN.\n";
        }
        if (!ok) {
            cout << "  Access denied after 3 wrong attempts.\n";
            continue;
        }

        cout << "  Welcome, " << w->getRole() << "!\n";
        cout << "  Your duty: " << DUTIES[c - 1] << "\n";
        dutyMenu(mc, w);
    }
}
