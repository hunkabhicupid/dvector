#pragma once
#include "thread_win.h"
#include <map>
#include <memory>

class Node : public PlatformThread::Delegate
{
public:    
    static shared_ptr<Node> CreateNode(char node_char);
    static void Send(char dest_node_char);  
    static int16_t Index(char c);

    Node(char c);
    ~Node() override;

    void ThreadMain() override;
    void UpdateOrigVector(vector<int16_t>);
    PlatformThreadId GetThreadId() const;
    
private:
    PlatformThreadHandle _handle;
    int16_t _nodeindex;
    char _nodechar;
    vector<int16_t> _origvector;

    static map<char, shared_ptr<Node>> _map;
};
