#include "stdafx.h"
#include <cassert>

map<char, shared_ptr<Node>> Node::_map;

void Node::ThreadMain()
{
    // Message Loop
    MSG msg;
	PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE);
	_isReady = true;
    while (GetMessage(&msg, 0, 0, 0))
    {
        if (msg.message == WM_APP + 1)
            cout << "Node " << _nodechar << " received a message." << endl << std::flush;;

        DispatchMessage(&msg);
    }
	_isReady = false;
    cout << "Node " << _nodechar << " is going down." << endl << std::flush;;

    // Process Message
}

shared_ptr<Node> Node::CreateNode(char c)
{
    shared_ptr<Node> node = make_shared<Node>(c);
    _map.insert(pair<char, shared_ptr<Node>>(c, node));
    return node;
}

void Node::Send(char dest_node_char, UINT msg, void*)
{
    std::map<char, shared_ptr<Node>>::iterator it;
    it = _map.find(dest_node_char);
    if (it == _map.end())
        return;
    shared_ptr<Node> node = it->second;

	while (!node->IsReady()) { ::Sleep(0); };

    PostThreadMessage(node->GetThreadId(), msg, 0, 0);
}

int16_t Node::Index(char c) {
    if (c >= 'A' && c <= 'Z') {
        return (c - 65);
    }

    if (c >= 'a' && c <= 'z') {
        return (c - 97);
    }

    throw std::exception("Invalid Input");
}

Node::Node(char c)
{
	_isReady = false;
    _nodechar = c;
    _nodeindex = Index(c);
    assert(PlatformThread::Create(0, this, &_handle) == true);
}

void Node::UpdateOrigVector(vector<int16_t>)
{

}

PlatformThreadId Node::GetThreadId() const
{
    return ::GetThreadId(_handle);
}

bool Node::IsReady() const
{
	return _isReady;
}

Node::~Node()
{
    // Send Quit
	while (!IsReady()) { ::Sleep(0); };	
	PostThreadMessage(GetThreadId(), WM_QUIT, 0, 0);

    //
    PlatformThread::Join(_handle);
}
