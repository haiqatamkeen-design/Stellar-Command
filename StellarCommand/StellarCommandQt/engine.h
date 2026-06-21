#ifndef ENGINE_H
#define ENGINE_H

#include <string>
using std::string;
using std::to_string;

inline unsigned long seed = 12345;
inline int randomInt(int n) {
    seed = seed * 1103515245 + 12345;
    return (int)((seed / 65536) % n);
}

class StellarException {
protected:
    string message;
public:
    StellarException(string msg) { message = msg; }
    virtual ~StellarException() {}
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

inline const string ROLES[4] = { "Commander", "Pilot", "Engineer", "Scientist" };
inline const string DUTIES[4] = {
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
    double getCapacity() { return capacity; }
    double getPercent() { return (level / capacity) * 100.0; }
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
    virtual string detailLine() = 0;
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
};

class Satellite : public SpaceCraft {
private:
    double altitude;
public:
    Satellite(int i, string n, double alt) : SpaceCraft(i, n, 120, 4) {
        altitude = alt;
    }
    string getType() { return "Satellite"; }
    string detailLine() { return "Orbit altitude: " + to_string((int)altitude) + " km"; }
    string emergencyAction() { return name + " switched to its backup antenna."; }
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
    string detailLine() { return "Speed: " + to_string((int)speed) + " km/h on " + terrain + " terrain"; }
    string emergencyAction() { return name + " parked safely and sealed the problem."; }
};

class Probe : public SpaceCraft {
private:
    string target;
public:
    Probe(int i, string n, string t) : SpaceCraft(i, n, 150, 3) {
        target = t;
    }
    string getType() { return "Probe"; }
    string detailLine() { return "Target body: " + target; }
    string emergencyAction() { return name + " saved its data and waited for new orders."; }
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
    int count() { return fleetCount; }
    SpaceCraft* at(int i) { return fleet[i]; }

    void checkRole(Worker* w, string role) {
        string r = w->getRole();
        if (r != role)
            throw AccessDeniedException("only the " + role + " can do this.");
    }

    string buildSatellite(Worker* w, string name, double alt) {
        checkRole(w, "Engineer");
        if (fleetCount >= 10) throw StellarException("Hangar is full.");
        fleet[fleetCount++] = new Satellite(nextId, name, alt);
        addLog("BUILT " + name + " (by " + w->getRole() + ")");
        return name + " added to the fleet with ID " + to_string(nextId++) + ".";
    }
    string buildRover(Worker* w, string name, double sp, string terrain) {
        checkRole(w, "Engineer");
        if (fleetCount >= 10) throw StellarException("Hangar is full.");
        fleet[fleetCount++] = new Rover(nextId, name, sp, terrain);
        addLog("BUILT " + name + " (by " + w->getRole() + ")");
        return name + " added to the fleet with ID " + to_string(nextId++) + ".";
    }
    string buildProbe(Worker* w, string name, string target) {
        checkRole(w, "Engineer");
        if (fleetCount >= 10) throw StellarException("Hangar is full.");
        fleet[fleetCount++] = new Probe(nextId, name, target);
        addLog("BUILT " + name + " (by " + w->getRole() + ")");
        return name + " added to the fleet with ID " + to_string(nextId++) + ".";
    }

    string scrap(Worker* w, int id) {
        checkRole(w, "Engineer");
        SpaceCraft* found = findCraft(id);
        if (found->isOnMission())
            throw StellarException("Cannot scrap a craft that is on a mission.");
        for (int i = 0; i < fleetCount; i++) {
            if (*fleet[i] == *found) {
                string nm = fleet[i]->getName();
                addLog("SCRAPPED " + nm + " (by " + w->getRole() + ")");
                delete fleet[i];
                for (int j = i; j < fleetCount - 1; j++)
                    fleet[j] = fleet[j + 1];
                fleetCount--;
                return nm + " removed from the fleet.";
            }
        }
        return "";
    }

    string refuel(Worker* w, int id) {
        checkRole(w, "Engineer");
        SpaceCraft* sc = findCraft(id);
        if (sc->isOnMission())
            throw StellarException("Cannot refuel during a mission.");
        sc->getFuel().refuel();
        addLog("REFUELED " + sc->getName() + " (by " + w->getRole() + ")");
        return sc->getName() + " refueled to full.";
    }

    string launch(Worker* w, int id, string mname, int days) {
        checkRole(w, "Commander");
        SpaceCraft* sc = findCraft(id);
        sc->launch(Mission(mname, days));
        addLog("LAUNCHED " + mname + " on " + sc->getName() + " (by " + w->getRole() + ")");
        return "Mission " + mname + " launched on " + sc->getName() + "!";
    }

    string emergency(Worker* w) {
        checkRole(w, "Commander");
        SpaceCraft* active[10];
        int n = 0;
        for (int i = 0; i < fleetCount; i++)
            if (fleet[i]->isOnMission()) active[n++] = fleet[i];
        if (n == 0) return "All quiet in space.";
        SpaceCraft* sc = active[randomInt(n)];
        string events[3] = { "Fuel leak", "Solar storm", "Signal lost" };
        string ev = events[randomInt(3)];
        addLog("EMERGENCY (" + ev + ") on " + sc->getName());
        return "EMERGENCY: " + ev + " on " + sc->getName() + "!\n"
             + "Response: " + sc->emergencyAction();
    }

    string advanceDay(Worker* w) {
        checkRole(w, "Pilot");
        string out;
        bool any = false;
        for (int i = 0; i < fleetCount; i++) {
            if (!fleet[i]->isOnMission()) continue;
            any = true;
            Mission& m = fleet[i]->getMission();
            try {
                fleet[i]->getFuel().burnOneDay();
                m.passDay();
                out += "Day " + to_string(m.getDaysPassed()) + " of mission " + m.getName() + ".\n";
                if (m.isFinished()) {
                    fleet[i]->finishMission();
                    addLog("COMPLETED " + m.getName());
                    out += "Mission " + m.getName() + " completed successfully!\n";
                }
            } catch (FuelException& e) {
                fleet[i]->finishMission();
                addLog("FAILED " + m.getName() + " (out of fuel)");
                out += e.what() + string(" Mission ") + m.getName() + " failed.\n";
            }
        }
        if (!any) return "No active missions.";
        return out;
    }

    string logText(Worker* w) {
        checkRole(w, "Scientist");
        if (logCount == 0) return "Mission log is empty.";
        string out = "Mission Log (" + to_string(logCount) + " entries):\n";
        for (int i = 0; i < logCount; i++) out += "  " + logBook[i] + "\n";
        return out;
    }

    string addNote(Worker* w, string note) {
        checkRole(w, "Scientist");
        addLog("NOTE: " + note + " (by " + w->getRole() + ")");
        return "Note saved in the mission log.";
    }
};

#endif
