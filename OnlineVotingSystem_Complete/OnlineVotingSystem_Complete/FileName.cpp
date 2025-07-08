#include<iostream>
#include<fstream>
#include<string>
#include<iomanip>
#include <limits> 
#include <windows.h> 
#undef max
using namespace std;

void clearScreen()
{
    system("cls");
}

const int MAX_USERS = 100;
const int MAX_PARTICIPANTS = 50;
const int MAX_AREAS = 10;
const int MAX_ELECTION_TYPES = 2;
const string ADMIN_FILE = "admin.txt";
const string VOTERS_FILE = "voters.txt";
const string PARTICIPANTS_FILE = "participants.txt";

void setTextColor(int color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}
void resetTextColor()
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}
//center console
void centerConsoleWindow(int width = 900, int height = 600)
{
    HWND console = GetConsoleWindow();
    RECT r;
    GetWindowRect(console, &r);

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    int xPos = (screenWidth / 2) - (width / 2);
    int yPos = (screenHeight / 2) - (height / 2);

    MoveWindow(console, xPos, yPos, width, height, TRUE);
}
//font size

void setConsoleFontAndSize()
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;
    cfi.dwFontSize.X = 0;
    cfi.dwFontSize.Y = 20; // Increase height
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    wcscpy_s(cfi.FaceName, L"Consolas"); // Or "Lucida Console"
    SetCurrentConsoleFontEx(hOut, FALSE, &cfi);
}


void displayWelcomeMessage()
{
    setTextColor(9);

    cout << R"(
  ___    _   _ __    _____  _   _  _____     __     ______   _____  _____  _   _   _____
 / _ \  | \ | | |    _____ | \ | || ____|    \ \   / / __  \|_____| _____ | \ | | / ____| 
| | | | | |  \| |      |   | |  \||  _|       \ \_/ / |  | |   |      |   | |  \|| | |_ |
| |_| | | |\  | |___ __|__ | |\  || |___       \   /| |  | |   |    __|__ | |\  || |__| |
 \___/  |_| \_|_____|_____ |_| \_||_____|       \ /  \____/    |   |_____ |_| \_| \_____|
    )" << "\n";
    cout << "\n\n";
    resetTextColor();
}

class VotingSystem;
class Voter;

class User
{
protected:
    string username;
    string password;
    string role;
    VotingSystem* votingSystem;
public:
    User(string u = "", string p = "", string r = "", VotingSystem* vs = nullptr) :username(u), password(p), role(r), votingSystem(vs)
    {}
    string getUsername()
    {
        return username;
    }
    string getPassword()
    {
        return password;
    }
    string getRole()
    {
        return role;
    }
    bool checkLogin(string u, string p)
    {
        return(username == u && password == p);
    }
    VotingSystem* getVotingSystem()
    {
        return votingSystem;
    }
    virtual void showMenu() = 0;
    virtual ~User()
    {

    }
};
class Admin :public User {
public:
    Admin(string u = "", string p = "", VotingSystem* vs = nullptr) :User(u, p, "admin", vs)
    {

    }
    void showMenu()override
    {
        cout << "\n\t*ADMIN MENU:" << endl;
        cout << "1. Add Candidate" << endl;
        cout << "2. Remove Candidate" << endl;
        cout << "3. View All Voters" << endl;
        cout << "4. View Election Result" << endl;
        cout << "5. Reset Voting System" << endl;
        cout << "6. Logout" << endl;

    }
    void addCandidate();
    void removeCandidate();
    void viewAllVoters();
    void viewResults();
    void resetSystem();

};
class Voter : public User
{
private:
    string cnic;
    string area;
    bool hasVoted;
    int age;

public:
    Voter(string u = "", string p = "", string c = "", string a = "", bool voted = false, int ag = 0, VotingSystem* vs = nullptr)
        : User(u, p, "voter", vs), cnic(c), area(a), hasVoted(voted), age(ag) {}

    bool getVotedStatus()
    {
        return hasVoted;
    }

    void setVotedStatus(bool status)
    {
        hasVoted = status;
    }
    string getCNIC()
    {
        return cnic;

    }
    string getArea()
    {
        return area;
    }
    int getAge()
    {
        return age;
    }

    void viewCandidatesByArea();
    void viewAllCandidates();
    void castVote();
    void viewResults();

    void showMenu() override
    {
        cout << "\n\t*VOTER MENU:" << endl;
        cout << "1. View Candidates in My Area (" << area << ")" << endl;
        cout << "2. View All Candidates" << endl;
        cout << "3. Vote" << endl;
        cout << "4. View Election Results" << endl;
        cout << "5. Logout" << endl;
    }
};
class Participant
{
private:
    string id;
    string name;
    string party;
    int votes;
    string electionType;
    string area;
public:
    Participant(string n = "", string p = "", string i = "", int v = 0, string et = "General", string a = "")
        : id(i), name(n), party(p), votes(v), electionType(et), area(a) {}

    string getId()
    {
        return id;
    }
    string getName()
    {
        return name;
    }
    string getParty()
    {
        return party;
    }
    int getVotes()
    {
        return votes;
    }
    string getElectionType()
    {
        return electionType;
    }
    string getArea()
    {
        return area;
    }


    void addVote()
    {
        votes++;
    }

    void display()
    {
        cout << left << setw(10) << id << setw(20) << name
            << setw(15) << party << setw(10) << electionType
            << setw(10) << area << setw(5) << votes << endl;
    }

    void saveToFile(ofstream& file)
    {
        file << id << " " << name << " " << party << " "
            << votes << " " << electionType << " " << area << endl;
    }

    static Participant loadFromFile(ifstream& file)
    {
        string id, name, party, electionType, area;
        int votes;
        file >> id >> name >> party >> votes >> electionType >> area;
        return Participant(name, party, id, votes, electionType, area);
    }
};

class VotingSystem
{
private:
    User* users[MAX_USERS];
    int userCount;
    Participant participants[MAX_PARTICIPANTS];
    int participantCount;
    string areas[MAX_AREAS];
    int areaCount;
    string electionTypes[MAX_ELECTION_TYPES];
    int electionTypeCount;
public:
    VotingSystem() : userCount(0), participantCount(0), areaCount(0), electionTypeCount(0)
    {
        addArea("North");
        addArea("South");
        addArea("East");
        addArea("West");
        addArea("Central");
        electionTypes[electionTypeCount++] = "National";
        electionTypes[electionTypeCount++] = "General";

        loadData();

    }

    void addArea(const string& areaName)
    {
        if (areaCount < MAX_AREAS)
        {
            areas[areaCount++] = areaName;
        }
    }
    void displayAreas()
    {
        cout << "*Available areas:\n";
        for (int i = 0; i < areaCount; i++)
        {
            cout << (i + 1) << ". " << areas[i] << endl;
        }
    }

    void displayElectionTypes()
    {
        cout << "*Available election types:\n";
        for (int i = 0; i < electionTypeCount; i++)
        {
            cout << (i + 1) << ". " << electionTypes[i] << endl;
        }
    }


    void adminMenu(Admin* admin)
    {
        while (true)
        {
            admin->showMenu();
            cout << "Enter choice: ";
            int choice;
            cin >> choice;

            if (choice == 1)
            {
                admin->addCandidate();
            }
            else if (choice == 2)
            {
                admin->removeCandidate();
            }
            else if (choice == 3)
            {
                admin->viewAllVoters();
            }
            else if (choice == 4)
            {
                admin->viewResults();
            }
            else if (choice == 5)
            {
                admin->resetSystem();
            }
            else if (choice == 6)
            {
                break;
            }
            else
            {
                cout << "Invalid choice!" << endl;
            }
        }
    }
    void adminAddCandidate()
    {
        if (participantCount >= MAX_PARTICIPANTS)
        {
            cout << "Maximum candidates reached!" << endl;
            return;
        }

        string id, name, party, area, electionType;
        cout << "Enter candidate id: ";
        cin >> id;

        cout << "Enter candidate name: ";
        cin.ignore();
        getline(cin, name);

        cout << "Enter party: ";
        getline(cin, party);

        displayAreas();
        cout << "Select candidate's area (1-" << areaCount << "): ";
        int areaChoice;
        cin >> areaChoice;

        if (areaChoice < 1 || areaChoice > areaCount)
        {
            cout << "Invalid area selected!" << endl;
            return;
        }
        area = getAreaByIndex(areaChoice - 1);

        displayElectionTypes();
        cout << "Select election type (1-" << electionTypeCount << "): ";
        int typeChoice;
        cin >> typeChoice;

        if (typeChoice < 1 || typeChoice > electionTypeCount)
        {
            cout << "Invalid election type selection!" << endl;
            return;
        }
        electionType = getElectionTypeByIndex(typeChoice - 1);

        for (int i = 0; i < participantCount; i++)
        {
            if (participants[i].getId() == id)
            {
                cout << "Candidate ID already exists!" << endl;
                return;
            }
        }

        participants[participantCount++] = Participant(name, party, id, 0, electionType, area);
        cout << "Candidate added for " << area << " area (" << electionType << " election)!" << endl;
    }

    void adminRemoveCandidate()
    {
        string id;
        adminViewAllCandidates();
        cout << "Enter candidate id to remove: " << endl;
        cin >> id;
        for (int i = 0; i < participantCount; i++)
        {
            if (participants[i].getId() == id)
            {
                for (int j = i; j < participantCount - 1; j++)
                {
                    participants[j] = participants[j + 1];
                }
                participantCount--;
                cout << "Participant removed succesfully!" << endl;
                return;
            }
        }
        cout << "Candidate not found!" << endl;
    }

    void adminViewAllCandidates()
    {
        cout << "\n*ALL CANDIDATES:\n";
        cout << left << setw(10) << "ID" << setw(20) << "Name"
            << setw(25) << "Party" << setw(10) << "Election" << setw(10) << "Votes" << endl;
        cout << string(75, '-') << endl;

        for (int i = 0; i < participantCount; i++)
        {
            participants[i].display();
        }
    }
    void adminViewAllVoters()
    {
        cout << "\n*REGISTERED VOTERS:\n";
        cout << left << setw(15) << "Username" << setw(15) << "CNIC"
            << setw(10) << "Area" << setw(10) << "Voted" << endl;
        cout << string(50, '-') << endl;

        for (int i = 0; i < userCount; i++)
        {
            if (users[i]->getRole() == "voter")
            {
                Voter* v = dynamic_cast<Voter*>(users[i]);
                if (v)
                {
                    cout << left << setw(15) << v->getUsername()
                        << setw(15) << v->getCNIC()
                        << setw(10) << v->getArea()
                        << setw(10) << (v->getVotedStatus() ? "Yes" : "No") << endl;
                }
            }
        }
    }

    void adminResetSystem()
    {
        char confirm;
        cout << "WARNING: This will reset all votes and voter statuses!\n";
        cout << "Are you sure? (y/n): ";
        cin >> confirm;

        if (confirm == 'y' || confirm == 'Y')
        {
            for (int i = 0; i < participantCount; i++)
            {
                Participant p(participants[i].getId(), participants[i].getName(),
                    participants[i].getParty(), 0, participants[i].getElectionType());
                participants[i] = p;
            }

            for (int i = 0; i < userCount; i++)
            {
                if (users[i]->getRole() == "voter")
                {
                    Voter* v = dynamic_cast<Voter*>(users[i]);
                    if (v)
                    {
                        v->setVotedStatus(false);
                    }
                }
            }

            cout << "System has been reset. All votes cleared.\n";
        }
        else
        {
            cout << "Reset cancelled.\n";
        }
    }

    void viewAllResults()
    {
        cout << "\n*ALL ELECTION RESULTS:\n";
        cout << left << setw(10) << "ID" << setw(20) << "Name"
            << setw(25) << "Party" << setw(10) << "Election" << setw(10) << "Votes" << endl;
        cout << string(75, '-') << endl;

        for (int i = 0; i < participantCount; i++)
        {
            participants[i].display();
        }

        int totalVotes = 0;
        for (int i = 0; i < participantCount; i++)
        {
            totalVotes += participants[i].getVotes();
        }
        cout << "\n*TOTAL VOTES CAST: " << totalVotes << endl;

        findAndDisplayWinners();

    }


    ~VotingSystem()
    {
        saveData();
        for (int i = 0; i < userCount; i++)
        {
            delete users[i];
        }
    }

    string getAreaByIndex(int index)
    {
        if (index >= 0 && index < areaCount)
        {
            return areas[index];
        }
        return "";
    }

    string getElectionTypeByIndex(int index)
    {
        if (index >= 0 && index < electionTypeCount)
        {
            return electionTypes[index];
        }
        return "";
    }
    void loadData()
    {
        ifstream adminFile(ADMIN_FILE);
        if (adminFile.is_open())
        {
            string username, password;
            adminFile >> username >> password;
            users[userCount++] = new Admin(username, password, this);
            adminFile.close();
        }
        else
        {
            users[userCount++] = new Admin("admin", "admin123", this);
        }
        ifstream voterFile(VOTERS_FILE);
        if (voterFile.is_open())
        {
            string username, password, cnic, area, votedStr, ageStr;
            while (getline(voterFile, username, ',') &&
                getline(voterFile, password, ',') &&
                getline(voterFile, cnic, ',') &&
                getline(voterFile, area, ',') &&
                getline(voterFile, votedStr, ',') &&
                getline(voterFile, ageStr) &&
                userCount < MAX_USERS)
            {
                bool voted = (votedStr == "1");
                int age = stoi(ageStr);
                users[userCount++] = new Voter(username, password, cnic, area, voted, age, this);
            }
            voterFile.close();
        }
        ifstream partFile(PARTICIPANTS_FILE);
        if (partFile.is_open())
        {
            while (partFile.peek() != EOF && participantCount < MAX_PARTICIPANTS)
            {
                participants[participantCount++] = Participant::loadFromFile(partFile);
            }
            partFile.close();
        }
    }

    void saveData()
    {
        ofstream adminFile(ADMIN_FILE);
        if (adminFile.is_open())
        {
            for (int i = 0; i < userCount; i++)
            {
                if (users[i]->getRole() == "admin")
                {
                    adminFile << users[i]->getUsername() << " " << users[i]->getPassword() << endl;
                    break;
                }
            }
            adminFile.close();
        }
        ofstream voterFile(VOTERS_FILE);
        if (voterFile.is_open())
        {
            for (int i = 0; i < userCount; i++)
            {
                if (users[i]->getRole() == "voter")
                {
                    Voter* v = dynamic_cast<Voter*>(users[i]);
                    if (v)
                    {
                        voterFile << v->getUsername() << "," << v->getPassword() << ","
                            << v->getCNIC() << "," << v->getArea() << ","
                            << (v->getVotedStatus() ? "1" : "0") << ","
                            << v->getAge() << endl;
                    }
                }
            }
            voterFile.close();
        }
        ofstream partFile(PARTICIPANTS_FILE);
        if (partFile.is_open())
        {
            for (int i = 0; i < participantCount; i++)
            {
                participants[i].saveToFile(partFile);
            }
            partFile.close();
        }
    }
    User* login()
    {
        clearScreen();
        string username, password, roleChoice;
        int attempts = 0;
        const int maxAttempts = 3;

        cout << "*Login as:\n1. Admin\n2. Voter\nEnter choice: ";
        cin >> roleChoice;

        while (attempts < maxAttempts)
        {

            cout << "Username and password are case-sensitive\n";
            cout << "Username: ";
            cin >> username;
            cout << "Password: ";
            cin >> password;

            bool validRole = (roleChoice == "1" || roleChoice == "2");
            if (!validRole)
            {
                cout << "Invalid role selection! Please choose 1 (Admin) or 2 (Voter)\n";
                break;
            }

            bool credentialsValid = false;
            User* matchedUser = nullptr;

            for (int i = 0; i < userCount; i++)
            {
                if (users[i]->checkLogin(username, password))
                {
                    matchedUser = users[i];
                    credentialsValid = true;
                    break;
                }
            }

            if (credentialsValid)
            {
                string requiredRole = (roleChoice == "1") ? "admin" : "voter";
                if (matchedUser->getRole() == requiredRole)
                {
                    cout << "\nLogin successful as " << matchedUser->getRole() << endl;
                    return matchedUser;
                }
                else
                {
                    cout << "\nError: You tried to login as " << requiredRole
                        << " but this account is " << matchedUser->getRole() << endl;
                }
            }
            else
            {
                cout << "\nInvalid username or password!\n";
                if (attempts < maxAttempts - 1)
                {
                    cout << "Please try again\n";
                }
            }

            attempts++;
        }

        if (attempts >= maxAttempts)
        {
            cout << "\nMaximum login attempts reached! Access temporarily locked.\n";
            cout << "Please try again later or contact system administrator.\n";
        }
        return nullptr;
    }

    void run()
    {
        clearScreen();
        displayWelcomeMessage();

        while (true)
        {
            cout << "\n\t*MAIN MENU:" << endl;
            cout << "1. Login" << endl;
            cout << "2. Register as Voter" << endl;
            cout << "3. Exit" << endl;
            cout << "Enter choice: ";

            int choice;
            cin >> choice;

            if (choice == 1)
            {
                User* user = login();
                if (user)
                {
                    if (user->getRole() == "admin")
                    {
                        Admin* admin = dynamic_cast<Admin*>(user);
                        adminMenu(admin);
                    }
                    else
                    {
                        Voter* voter = dynamic_cast<Voter*>(user);
                        voterMenu(voter);
                    }
                }
            }
            else if (choice == 2)
            {
                registerVoter();
            }
            else if (choice == 3)
            {
                cout << "Thank you for using the Online Voting System!" << endl;
                break;
            }
            else
            {
                cout << "Invalid choice!" << endl;
            }
        }
    }
    void registerVoter()

    {
        if (userCount >= MAX_USERS)
        {
            cout << "Maximum users reached!" << endl;
            return;
        }

        string username, password, cnic, area;
        int age;

        while (true)
        {
            cout << "Enter username (4-20 characters, no spaces): ";
            cin >> username;

            if (username.length() < 4 || username.length() > 20)
            {
                cout << "Username must be between 4-20 characters!\n";
                continue;
            }

            bool hasSpace = false;

            for (char c : username)
            {
                if (isspace(c))
                {
                    hasSpace = true;
                    break;
                }
            }

            if (hasSpace) {
                cout << "Username cannot contain spaces!\n";
                continue;
            }

            bool usernameExists = false;

            for (int i = 0; i < userCount; i++)
            {
                if (users[i]->getUsername() == username)
                {
                    usernameExists = true;
                    break;
                }
            }

            if (usernameExists)
            {
                cout << "Username already taken! Please choose another.\n";
                continue;
            }

            break;
        }

        while (true)
        {
            cout << "Enter password (at least 6 characters): ";
            cin >> password;

            if (password.length() < 6)
            {
                cout << "Password must be at least 6 characters!\n";
                continue;
            }

            break;
        }

        cout << "Enter CNIC (without dashes, 13 digits): ";
        cin >> cnic;

        while (cnic.length() != 13 || cnic.find_first_not_of("0123456789") != string::npos)
        {
            cout << "Invalid CNIC! Must be 13 digits without dashes.\n";
            cout << "Enter CNIC: ";
            cin >> cnic;
        }
        cout << "Enter CNIC (format: 12345-1234567-1): ";
        cin >> cnic;

        /*while (cnic.length() != 15 || cnic[5] != '-' || cnic[13] != '-') {
            cout << "Invalid CNIC! Format must be 12345-1234567-1\n";
            cout << "Enter CNIC again: ";
            cin >> cnic;
        }*/

        cout << "Enter your age: ";

        while (!(cin >> age) || age < 18)
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (age < 18)
            {
                cout << "You must be at least 18 years old to register!\n";
            }
            else
            {
                cout << "Invalid age! Please enter a number: ";
            }
        }

        displayAreas();
        cout << "Select your area (1-" << areaCount << "): ";
        int areaChoice;
        while (!(cin >> areaChoice) || areaChoice < 1 || areaChoice > areaCount)
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid selection! Choose between 1-" << areaCount << ": ";
        }
        area = getAreaByIndex(areaChoice - 1);

        for (int i = 0; i < userCount; i++)
        {
            if (users[i]->getRole() == "voter")
            {
                Voter* v = dynamic_cast<Voter*>(users[i]);
                if (v && v->getCNIC() == cnic)
                {
                    cout << "This CNIC is already registered!\n";
                    return;
                }
            }
        }

        users[userCount++] = new Voter(username, password, cnic, area, false, age, this);
        cout << "\nRegistration successful!\n";
        cout << "You can now vote in " << area << " area.\n";

    }

    void viewResultsMenu()
    {
        cout << "\n*ELECTION RESULTS:\n";
        cout << "1. View All Results\n";
        cout << "2. View National Election Results\n";
        cout << "3. View General Election Results\n";
        cout << "Enter choice: ";

        int choice;
        cin >> choice;

        if (choice == 1)
        {
            viewAllResults();
        }
        else if (choice == 2)
        {
            viewElectionTypeResults("National");
        }
        else if (choice == 3)
        {
            viewElectionTypeResults("General");
        }

        else
        {
            cout << "Invalid choice!\n";
        }
    }

    void voterMenu(Voter* voter)
    {
        clearScreen();
        while (true)
        {
            voter->showMenu();
            cout << "Enter choice: ";
            int choice;
            cin >> choice;

            if (choice == 1)
            {
                voter->viewCandidatesByArea();
            }
            else if (choice == 2)
            {
                voter->viewAllCandidates();
            }
            else if (choice == 3)
            {
                voter->castVote();
            }
            else if (choice == 4)
            {
                voter->viewResults();
            }
            else if (choice == 5)
            {
                break;
            }
            else
            {
                cout << "Invalid choice!" << endl;
            }
        }
    }
    void voterViewCandidatesByArea(const string& area)
    {
        cout << "\n*CANDIDATES IN " << area << " AREA:\n";
        cout << left << setw(10) << "ID" << setw(20) << "Name"
            << setw(15) << "Party" << setw(10) << "Election"
            << setw(10) << "Area" << setw(5) << "Votes" << endl;
        cout << string(70, '-') << endl;

        bool found = false;
        for (int i = 0; i < participantCount; i++)
        {
            if (participants[i].getArea() == area)
            {
                participants[i].display();
                found = true;
            }
        }

        if (!found)
        {
            cout << "No candidates found in your area.\n";
        }
    }

    void voterViewAllCandidates()
    {
        cout << "\n*ALL CANDIDATES:\n";
        cout << left << setw(10) << "ID" << setw(20) << "Name"
            << setw(25) << "Party" << setw(10) << "Election" << setw(10) << "Votes" << endl;
        cout << string(75, '-') << endl;

        for (int i = 0; i < participantCount; i++)
        {
            participants[i].display();
        }

        int totalVotes = 0;
        for (int i = 0; i < participantCount; i++)
        {
            totalVotes += participants[i].getVotes();
        }
        cout << "\n*TOTAL VOTES CAST: " << totalVotes << endl;
    }

    bool voterCastVote(Voter* voter)
    {
        if (voter->getVotedStatus())
        {
            cout << "You have already voted!" << endl;
            return false;
        }

        cout << "\nYou are voting in " << voter->getArea() << " area\n";
        voterViewCandidatesByArea(voter->getArea());

        string id;
        cout << "Enter candidate ID to vote (or '0' to cancel): ";
        cin >> id;

        if (id == "0") return false;

        for (int i = 0; i < participantCount; i++)
        {
            if (participants[i].getId() == id && participants[i].getArea() == voter->getArea())
            {
                participants[i].addVote();
                voter->setVotedStatus(true);
                cout << "Vote recorded for " << participants[i].getName() << " ("
                    << participants[i].getElectionType() << " election)!" << endl;
                return true;
            }
        }
        cout << "Invalid candidate ID or candidate not in your area!" << endl;
        return false;
    }
    void viewElectionTypeResults(const string& electionType)
    {
        cout << "\n" << electionType << " ELECTION RESULTS:\n";
        cout << left << setw(10) << "ID" << setw(20) << "Name"
            << setw(25) << "Party" << setw(10) << "Votes" << endl;
        cout << string(65, '-') << endl;

        bool found = false;
        int totalTypeVotes = 0;

        for (int i = 0; i < participantCount; i++)
        {
            if (participants[i].getElectionType() == electionType)
            {
                cout << left << setw(10) << participants[i].getId()
                    << setw(20) << participants[i].getName()
                    << setw(25) << participants[i].getParty()
                    << setw(10) << participants[i].getVotes() << endl;
                totalTypeVotes += participants[i].getVotes();
                found = true;
            }
        }

        if (!found)
        {
            cout << "No candidates found for " << electionType << " election.\n";
            return;
        }

        cout << "\n*TOTAL VOTES CAST IN " << electionType << " ELECTION: " << totalTypeVotes << endl;

        int maxVotes = 0;
        int winnerIndex = -1;
        bool tie = false;

        for (int i = 0; i < participantCount; i++)
        {
            if (participants[i].getElectionType() == electionType)
            {
                if (participants[i].getVotes() > maxVotes)
                {
                    maxVotes = participants[i].getVotes();
                    winnerIndex = i;
                    tie = false;
                }
                else if (participants[i].getVotes() == maxVotes)
                {
                    tie = true;
                }
            }
        }

        if (maxVotes > 0)
        {
            if (tie)
            {
                cout << "\n" << electionType << " ELECTION RESULT: There is a tie between multiple candidates!\n";
            }
            else
            {
                cout << "\n" << electionType << " ELECTION WINNER: " << participants[winnerIndex].getName()
                    << " (" << participants[winnerIndex].getParty() << ") with "
                    << maxVotes << " votes!\n";
            }
        }
        else
        {
            cout << "\nNo votes have been cast yet for " << electionType << " election.\n";
        }
    }
    void findAndDisplayWinners()
    {
        for (int et = 0; et < electionTypeCount; et++)
        {
            string currentType = electionTypes[et];
            int maxVotes = 0;
            int winnerIndex = -1;
            bool tie = false;

            for (int i = 0; i < participantCount; i++)
            {
                if (participants[i].getElectionType() == currentType)
                {

                    if (participants[i].getVotes() > maxVotes)
                    {
                        maxVotes = participants[i].getVotes();
                        winnerIndex = i;
                        tie = false;
                    }
                    else if (participants[i].getVotes() == maxVotes)
                    {
                        tie = true;
                    }
                }
            }

            if (maxVotes > 0)
            {
                if (tie)
                {
                    cout << "\n" << currentType << " ELECTION RESULT: There is a tie between multiple candidates!\n";
                }
                else
                {
                    cout << "\n" << currentType << " ELECTION WINNER: " << participants[winnerIndex].getName()
                        << " (" << participants[winnerIndex].getParty() << ") with "
                        << maxVotes << " votes!\n";
                }
            }
            else
            {
                cout << "\nNo votes have been cast yet for " << currentType << " election.\n";
            }
        }
    }


};



void Admin::addCandidate()
{
    if (votingSystem)
    {
        votingSystem->adminAddCandidate();
    }
}
void Admin::removeCandidate()
{
    if (votingSystem)
    {
        votingSystem->adminRemoveCandidate();
    }
}
void Admin::viewAllVoters()
{
    if (votingSystem)
    {
        votingSystem->adminViewAllVoters();
    }
}

void Admin::viewResults()
{
    if (votingSystem)
    {
        votingSystem->viewResultsMenu();
    }
}

void Admin::resetSystem()
{
    if (votingSystem)
    {
        votingSystem->adminResetSystem();
    }
}
void Voter::viewCandidatesByArea()
{
    if (votingSystem)
    {
        votingSystem->voterViewCandidatesByArea(area);
    }
}

void Voter::viewAllCandidates()
{
    if (votingSystem)
    {
        votingSystem->voterViewAllCandidates();
    }
}


void Voter::castVote()
{
    if (votingSystem)
    {
        votingSystem->voterCastVote(this);
    }
}

void Voter::viewResults()
{
    if (votingSystem)
    {
        votingSystem->viewResultsMenu();
    }
}

int main()
{
    setConsoleFontAndSize();
    centerConsoleWindow();
    setTextColor(15);
    VotingSystem system;
    system.run();

    cout << "\nOnline Voting System shutdown complete.\n";
    return 0;
}
