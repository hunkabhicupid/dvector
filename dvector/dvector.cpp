// dvector.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

vector<vector<int16_t>> orig_vec;

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
            cout << "  " << std::setw(5) << std::right << vec[i][j];
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

    do {
        // get the initial link costs
        string input;
        cout << endl
             << "Link cost between each pair of routers (Set to \"inf\" if no "
                "connection)"
             << endl
             << "<example A,B,7;B,C,1;C,E,inf; and so on> ? ";
        cin.ignore();
        getline(cin, input);
        ParseInputAndUpdateOrigVector(input);

        // print the input link costs vectors
        PrintVector(orig_vec);

        // Update orig vector for all nodes
        Node::NotifyUpdatedOrigVector(orig_vec);
		Node::InformNeighborsDelayed();

        char c;
        cout << " Press 'q' to quit else any other key to continue...." << endl;
        cin >> c;
        quit = (c == 'q');
    } while (!quit);

    return 0;
}
