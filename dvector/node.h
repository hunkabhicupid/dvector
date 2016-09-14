#pragma once
#include "thread_win.h"
#include <map>
#include <memory>

#define WM_UPDATE_ROUTING_TABLE (WM_APP + 1)

class Node : public PlatformThread::Delegate
{
public:    
	static void InitializeVector(vector<vector<int16_t>> &vec, int16_t num_routers);
    static shared_ptr<Node> CreateNode(char node_char);	
	static void NotifyUpdatedOrigVector(const vector<vector<int16_t>>& orig_vector);
	static void DumpAll();
    static void Send(char dest_node_char, UINT msg, void* msg_data);  
    static int16_t ToIndex(char c);
	static char ToChar(int16_t c);

    Node(char c);
    ~Node() override;

	void Dump();

    void ThreadMain() override;
    void UpdateOrigVector(const vector<int16_t>& orig_vector);
    PlatformThreadId GetThreadId() const;
	bool IsReady() const;
	
private:	
	void GetLatestShortestPath(vector<vector<int16_t>>& ret, vector<vector<int16_t>>& latest, int16_t latest_from) const;
	bool ValidateShortestPath() const;
	vector<char> GetNeighbors();

    PlatformThreadHandle _handle;
    int16_t _nodeindex;
    char _nodechar;
    vector<int16_t> _origvector;
	mutable bool _isReady;
	vector<int16_t> _orig_vector;			// to a node via that node
	vector<vector<int16_t>> _shortest_path_vector;	// to a node may via the same node or any other node
    static map<char, shared_ptr<Node>> _map;
};
