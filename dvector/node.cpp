#include "stdafx.h"
#include <algorithm>
#include <cassert>

map<char, shared_ptr<Node>> Node::_map;

void Node::ThreadMain() {
    // Message Loop
    MSG msg;
    PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE);
    _isReady = true;
    while (GetMessage(&msg, 0, 0, 0)) {
        if (msg.message == WM_UPDATE_ROUTING_TABLE) {

            cout << "Node " << _nodechar << " received a message." << endl
                 << std::flush;
            ;
        }

        DispatchMessage(&msg);
    }
    _isReady = false;
    cout << "Node " << _nodechar << " is going down." << endl << std::flush;
    ;

    // Process Message
}

void Node::InitializeVector(vector<vector<int16_t>> &vec, int16_t num_routers) {
    for (int i = 0; i < num_routers; i++) {
        vector<int16_t> row; // Create an empty row
        for (int16_t j = 0; j < num_routers; j++) {
            row.push_back(INT16_MAX); // invalid value - INT16_MAX
        }
        vec.push_back(row); // Add the row to the main vector
    }
}

shared_ptr<Node> Node::CreateNode(char c) {
    shared_ptr<Node> node = make_shared<Node>(c);
    _map.insert(pair<char, shared_ptr<Node>>(c, node));
    return node;
}

void Node::NotifyUpdatedOrigVector(const vector<vector<int16_t>> &orig_vector) {
    for (auto &i : _map) {
        shared_ptr<Node> node = i.second;
        int16_t nodeindex = ToIndex(i.first);
        node->UpdateOrigVector(orig_vector[nodeindex]);
    }
}

void Node::DumpAll() {
    for (auto &i : _map) {
        shared_ptr<Node> node = i.second;
        node->Dump();
    }
}

void Node::Send(char dest_node_char, UINT msg, void *) {
    std::map<char, shared_ptr<Node>>::iterator it;
    it = _map.find(dest_node_char);
    if (it == _map.end())
        return;
    shared_ptr<Node> node = it->second;

    while (!node->IsReady()) {
        ::Sleep(0);
    };

    PostThreadMessage(node->GetThreadId(), msg, 0, 0);
}

int16_t Node::ToIndex(char c) {
    if (c >= 'A' && c <= 'Z') {
        return (c - 65);
    }

    if (c >= 'a' && c <= 'z') {
        return (c - 97);
    }

    throw std::exception("Invalid Input");
}

char Node::ToChar(int16_t c) {
    if (c < 0 || c >= 26)
        throw std::exception("Invalid Input");

    return (static_cast<char>(65 + c));
}

Node::Node(char c) {
    _isReady = false;
    _nodechar = c;
    _nodeindex = ToIndex(c);
    assert(PlatformThread::Create(0, this, &_handle) == true);
}

// Structure of _shortest_path_vector for 'A' for example
// from A	via A	via B	via C	via D
// to A
// to B
// to C
// to D

// This is direct call as user changes the input cost
// no path or has path now / cost has increased or decreased
void Node::UpdateOrigVector(const vector<int16_t> &orig_vector) {
    int num_routers = static_cast<int>(orig_vector.size());
    bool first_time = (_shortest_path_vector.size() == 0) ? true : false;
    // one time and first time
    if (first_time) {
        InitializeVector(_shortest_path_vector, num_routers);
    }

    // check if there is any change from earlier _orig_vector
    if (!first_time &&
        equal(orig_vector.begin(), orig_vector.end(), _orig_vector.begin()))
        return;

    // Update orignal vector
    // this also provides information about neighbors
    _orig_vector = orig_vector;

    vector<vector<int16_t>> latest_from_orig = _shortest_path_vector;
    for (int i = 0; i < num_routers; i++) {
        latest_from_orig[i][i] = orig_vector[i];
    }

    if (_shortest_path_vector != latest_from_orig) {

        // check if that changes affects our _shortest_path_vector
        // if yes, then send the same to neighbors

        _shortest_path_vector = latest_from_orig;
    }

    assert(ValidateShortestPath());
}

PlatformThreadId Node::GetThreadId() const { return ::GetThreadId(_handle); }

bool Node::IsReady() const { return _isReady; }

void Node::GetLatestShortestPath(vector<vector<int16_t>> &ret,
                                 vector<vector<int16_t>> &latest,
                                 int16_t latest_from) const {
    int16_t num_routers, i, k;
    num_routers = static_cast<int16_t>(latest.size());
    // Initialize with current
    ret = _shortest_path_vector;

    k = latest_from;
    const int16_t min_cost_to_k =
        *(std::min_element(ret[k].begin(), ret[k].end()));

    // Apply the update
    for (i = 0; i < num_routers; i++) {

        if (i == _nodeindex)
            continue;

        const int16_t min_cost_to_i_from_k =
            *(std::min_element(latest[i].begin(), latest[i].end()));

        if (ret[i][k] > min_cost_to_k + min_cost_to_i_from_k) {
            ret[i][k] = min_cost_to_k + min_cost_to_i_from_k;
        }
    }
}

bool Node::ValidateShortestPath() const {
    bool ret = true;
    int16_t num_routers, i, j;
    num_routers = static_cast<int16_t>(_shortest_path_vector.size());
    for (i = 0; i < num_routers; i++) {
        for (j = 0; j < num_routers; j++) {
            if (((i == _nodeindex) || (j == _nodeindex)) &&
                (_shortest_path_vector[i][j] != INT16_MAX))
                ret = false;
            else if (_shortest_path_vector[i][j] <= 0)
                ret = false;
        }
    }

    return ret;
}

vector<char> Node::GetNeighbors() {
    vector<char> vec;
    for (size_t i = 0; i < _orig_vector.size(); i++) {
        if ((_orig_vector[i] != INT16_MAX) && (_orig_vector[i] > 0))
            vec.push_back(ToChar(static_cast<int16_t>(i)));
    }
    return vec;
}

Node::~Node() {
    // Send Quit
    while (!IsReady()) {
        ::Sleep(0);
    };
    PostThreadMessage(GetThreadId(), WM_QUIT, 0, 0);

    //
    PlatformThread::Join(_handle);
}

void Node::Dump() {
    int16_t num_routers = static_cast<int16_t>(_shortest_path_vector.size());
    cout << endl;
    cout << "For " << _nodechar << " (row TO column VIA)" << endl;

    cout << " ";
    for (int16_t i = 0; i < num_routers; i++) {
        cout << "  " << std::setw(5) << std::right << static_cast<char>(65 + i);
    }
    for (int i = 0; i < num_routers; i++) {
        cout << endl << static_cast<char>(65 + i);
        for (int16_t j = 0; j < num_routers; j++) {
            cout << "  " << std::setw(5) << std::right
                 << _shortest_path_vector[i][j];
        }
    }
    cout << endl;
}
