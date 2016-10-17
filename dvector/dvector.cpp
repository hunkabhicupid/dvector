// dvector.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <thread>

vector<vector<int16_t>> orig_vec;

int DisplayMenu(string &input) {
    int choice;
    input = string("");
    std::stringstream str;
    str << "Press number as below: " << endl;
    str << "1. Enter New Configuration: <enter A,B,7; B,C,1; C,E,inf and so on>"
        << endl;
    str << "2. Get the least cost path between any 2 nodes: <node1>, <node2>"
        << endl;
    str << "3. Quit" << endl;
    cout << str.str();

    cin >> choice;
    if (choice == 3)
        return choice;
    cin.ignore();
    getline(cin, input);
    return choice;
}

void PrintVector(vector<vector<int16_t>> &vec) {
    int16_t num_routers = static_cast<int16_t>(vec.size());
    cout << endl;
    cout << " ";
    for (int16_t i = 0; i < num_routers; i++) {
        cout << "  " << std::setw(5) << std::right << static_cast<char>(65 + i);
    }
    for (int i = 0; i < num_routers; i++) {
        cout << endl << static_cast<char>(65 + i);
        for (int16_t j = 0; j < num_routers; j++) {
            cout << "  " << std::setw(5) << std::right
                 << ((vec[i][j] != INT16_MAX) ? vec[i][j] : -1);
        }
    }
    cout << endl;
}

void ParseLinkCostAndUpdateOrigVector(const string str) {
    char *cstr = new char[str.length() + 1];
    size_t len = str.copy(cstr, str.length());
    cstr[len] = '\0';

    char *next_token;
    char *pch = strtok_s(cstr, ",", &next_token);
    if (pch == nullptr)
        throw std::exception("Invalid Input");

    int16_t src = Node::ToIndex(pch[0]);

    pch = strtok_s(nullptr, ",", &next_token);
    if (pch == nullptr)
        throw std::exception("Invalid Input");

    int16_t dest = Node::ToIndex(pch[0]);

    pch = strtok_s(nullptr, ",", &next_token);
    if (pch == nullptr)
        throw std::exception("Invalid Input");

    int16_t cost = INT16_MAX;
    if ((strcmp(pch, "inf") != 0) && (strcmp(pch, "INF") != 0))
        cost = static_cast<int16_t>(strtoul(pch, nullptr, 10));

    if (cost <= 0)
        cost = INT16_MAX;

    orig_vec[src][dest] = cost;
    orig_vec[dest][src] = cost;

    delete[] cstr;
}

void ParseInputAndUpdateOrigVector(const string &str) {
    char *cstr = new char[str.length() + 1];
    size_t len = str.copy(cstr, str.length());
    cstr[len] = '\0';

    char *next_token;
    char *pch = strtok_s(cstr, ";", &next_token);
    while (pch != nullptr) {
        ParseLinkCostAndUpdateOrigVector(pch);
        pch = strtok_s(nullptr, ";", &next_token);
    }
    delete[] cstr;
}

void ParseNodeNode(const string &str, int16_t &src, int16_t &dest) {
    char *cstr = new char[str.length() + 1];
    size_t len = str.copy(cstr, str.length());
    cstr[len] = '\0';

    char *next_token;
    char *pch = strtok_s(cstr, ",", &next_token);
    if (pch == nullptr)
        throw std::exception("Invalid Input");

    src = Node::ToIndex(pch[0]);

    pch = strtok_s(nullptr, ",", &next_token);
    if (pch == nullptr)
        throw std::exception("Invalid Input");

    dest = Node::ToIndex(pch[0]);

    delete[] cstr;
}

void HandleConfigChange(string &input) {
    ParseInputAndUpdateOrigVector(input);

    // print the input link costs vectors
    PrintVector(orig_vec);

    // Update orig vector for all nodes
    Node::NotifyUpdatedOrigVector(orig_vec);
    Node::InformNeighborsDelayed();
}

void HandleLowCostPath(string &input) {
    int16_t src, dest;
    ParseNodeNode(input, src, dest);
    Node::PrintLowCostPath(src, dest);
}

int main() {
    int16_t num_routers;
    cout << "No: of routers(max 26): ";
    cin >> num_routers;

    if ((num_routers < 0) || (num_routers > 26))
        throw std::exception("Invalid Input");

    cout << "Routers:" << endl;
    for (int16_t i = 0; i < num_routers; i++)
        cout << static_cast<char>(65 + i) << " ";

    // initialize the initial link costs vectors
    Node::InitializeVector(orig_vec, num_routers);
    for (int16_t i = 0; i < num_routers; i++)
        Node::CreateNode(static_cast<char>(65 + i));

    bool quit = false;

    // get the initial link costs
    string input;
    cout << endl
         << "Link cost between each pair of routers (Set to \"inf\" if no "
            "connection)"
         << endl
         << "<example A,B,7;B,C,1;C,E,inf; and so on> ? ";
    cin.ignore();
    getline(cin, input);

    HandleConfigChange(input);
    std::this_thread::sleep_for(1s);

    while (true) {
        int choice = DisplayMenu(input);
        switch (choice) {
        case 1:
            HandleConfigChange(input);
            std::this_thread::sleep_for(1s);
            break;
        case 2:
            HandleLowCostPath(input);
            break;
		case 3:
        default:
			quit = true;
			break;
        }

		if (quit)
			break;

    }

    return 0;
}
