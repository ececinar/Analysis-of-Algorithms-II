/***************************
 * 
 * Author: Ece Cinar, 150150138
 * 
 * ************************/

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <queue>
#include <stack>
#include <deque>
#include <map>
#include <limits>

using namespace std;

/*********CustomCompare***********
* Custom class that compares pairs in a priority queue
* Second element of each pair is shortest path length to that node.
* Function gives priority to the node with smallest distance.
****************************************/

class CustomCompare{
    public:
        bool operator()(const pair<int,int>& l, const pair<int,int>& r){    return l.second > r.second ;  }
};

class Graph{
    private:
        vector< vector<int> > adj;
        int node_number;
        int jo_src,jo_dest,lu_src,lu_dest;
        deque< pair<int,int> > jo_forward,lu_forward, jo_backward, lu_backward;
        int jo_time,lu_time;
        
    public:
        Graph(int);
        void print(deque< pair<int,int> >);
        void set_edges(pair<int,int>,int);
        void set_locations(int,int,int,int);
        deque<pair <int,int> > dijkstra(int,int,int);
        void find_shortest_paths();
        bool solve_conflicts(int, deque< pair<int,int> >&, deque< pair<int,int> >&);
};

/*********Constructor************
 * Takes size of adjaceny matrix as its parameter.
 * Resizes the vector to be an nxn square matrix.
********************************/

Graph::Graph(int n){
    node_number = n;
    adj.resize(node_number, vector<int>(node_number));
}

/************print***************
 * Prints a given path node by node.
********************************/

void Graph::print(deque< pair<int,int> > path){
    for(int i = 0; i < path.size(); i++)
        cout << "Node: " << path[i].first << "  Time: " << path[i].second << endl; 
}

/************set_edges***************
* Takes a pair representing an edge and its weight w as its parameters
* Adjusts the adjacency matrices related cell
********************************/

void Graph::set_edges(pair<int,int> e, int w){
    adj[e.first][e.second] = w;
}

/************set_locations***************
* Sets initial source and destination points for both Joseph and Lucy
********************************/

void Graph::set_locations(int j_h, int j_d, int l_h, int l_d){
    jo_src = j_h;
    jo_dest = j_d;
    lu_src = l_h;
    lu_dest = l_d;
}

/************dijkstra***************
* takes two node parameters: start point and end point
* time shows the initial time before starting the algorithm
* visited has node ids as key and length of the shortest path from start to that node as value
* path holds the shortest path from start to end discovered during the algorithm
* distances is prioritized with respect to path lengths from start to each node
* distances consists of pairs of node id and path length to that node
* the largest possible integer value is used as infinity
* previous holds the previous node id for each node
********************************/


deque< pair<int,int> > Graph::dijkstra(int start, int end, int time){
    int infinity = numeric_limits<int>::max();
    pair<int,int> node, neighbor;
    map<int,int> visited;
    int edge_weight;
    deque< pair<int,int> > path;
    vector<int> previous(node_number);
    stack< pair<int,int> > update;
    priority_queue< pair<int,int>, vector< pair<int,int> >, CustomCompare > distances;
    
    // each node is pushed into the priority queue.
    // starting node is pushed with initial time value in a pair
    // other nodes are pushed with infinity instead 

    for(int i = 0; i < node_number; i++){
        if(i == start)
            distances.push(make_pair(i,time));  
        else
            distances.push(make_pair(i,infinity));
        previous[i] = i;
    }

    // if top element of the priority queue has infinite path length, it means that the remaining nodes cannot be reached.
    // while ends if all nodes are visited or the remaining nodes cannot be reached.
    

    while(visited.size() < node_number && distances.top().second != infinity){
        // the top node of priority queue is assigned to node and popped . this node is then inserted into visited.
        node = distances.top();
        distances.pop();
        visited.insert(node);
        // remaining nodes of priority queue are moved to a stack to be updated.
        while(!distances.empty()){
            update.push(distances.top());
            distances.pop();
        }
        while(!update.empty()){
            edge_weight = adj[node.first][update.top().first];
            // if edge weight is greater than zero, it means that the current node is connected to the node at the top of the stack
            if(edge_weight){
                // so this node is a neighbor of the current node
                neighbor = update.top();
                // we update the shortest path length to neighbor if newly calculated distance is smaller than previous
                // we also initialize current node to previous node of the neighbor
                if(node.second +  edge_weight < neighbor.second ){
                    neighbor.second = node.second + edge_weight;
                    previous[neighbor.first] = node.first;
                }
                update.pop();
                // node is moved to priority queue again
                distances.push(neighbor);
            }
            else{
                // node is moved to priority queue again
                distances.push(update.top());
                update.pop();
            }
        }
    }

    int idx = end;

    // using previous vector, path is found starting from the end

    path.push_front( make_pair( idx, visited.find(idx)->second ) );
    while(idx != previous[idx]){
        path.push_front(make_pair( previous[idx], visited.find( previous[idx] )->second ));
        idx = previous[idx];
    }

    // if start node cannot be reached, it means that there is no path from start to end
    // path is cleared
    if(idx!=start)
        path.clear();
    
    return path;
}

/************find_shortest_paths***************
* finds shortest paths for both Lucy and Joseph and prints them
**********************************************/

void Graph::find_shortest_paths(){

    int overlap = 0;
    int overlap_node;
    int temp;
    bool solved;

    jo_time = 0;
    lu_time = 0;

    // forward shortest paths for Joseph and Lucy are calculated

    jo_forward = dijkstra(jo_src,jo_dest,jo_time);
    lu_forward = dijkstra(lu_src,lu_dest,lu_time);

    // if there is an overlap, overlap flag is set and overlap node is determined

    for(int i = 0; i < jo_forward.size(); i++){
        for(int j = 0; j < lu_forward.size(); j++){
            if(jo_forward[i] == lu_forward[j] || (lu_forward[j].first == jo_forward.back().first && lu_forward[j].second >= jo_forward.back().second &&  lu_forward[j].second <= jo_forward.back().second + 30)){
                overlap = 1;
                overlap_node = lu_forward[j].first;
                break;
            }
        }
        if(overlap)
            break;     
        if(jo_forward[i].first == lu_forward.back().first && jo_forward[i].second >= lu_forward.back().second &&  jo_forward[i].second <= lu_forward.back().second + 30){
            overlap = 1;
            overlap_node = jo_forward[i].first;
            break;
        }
    }

    // overlapping node and two paths to be recalculated are given as parameters to solve_conflicts
    // if the overlap is not solved, program terminates after printing no solution

    if(overlap){
        solved = solve_conflicts(overlap_node,jo_forward,lu_forward);
        if(!solved){
            cout << "No solution!" << endl;
            return;
        }
    }
        
    // starting times for backwards trip are calculated for both Joseph and Lucy

    jo_time = jo_forward.back().second + 30;
    lu_time = lu_forward.back().second + 30;

    // source and destination nodes are swapped for both Joseph and Lucy    

    temp = jo_src;
    jo_src = jo_dest;
    jo_dest = temp;

    temp = lu_src;
    lu_src = lu_dest;
    lu_dest = temp;

    // same things done for forward path search are repeated for swapped source and dest nodes

    jo_backward = dijkstra(jo_src,jo_dest,jo_time);
    lu_backward = dijkstra(lu_src,lu_dest,lu_time);

    overlap = 0;

    for(int i = 0; i < jo_backward.size(); i++){
        for(int j = 0; j < lu_backward.size(); j++){
            if(jo_backward[i] == lu_backward[j] || (lu_backward[j].first == jo_backward[0].first && lu_backward[j].second <= jo_backward[0].second &&  lu_backward[j].second >= jo_backward[0].second - 30)){
                overlap = 1;
                overlap_node = lu_backward[j].first;
                break;
            }
        }
        if(overlap) 
            break;
        if(jo_backward[i].first == lu_backward[0].first && jo_backward[i].second <= lu_backward[0].second &&  jo_backward[i].second >= lu_backward[0].second - 30){
            overlap = 1;
            overlap_node = jo_backward[i].first;
            break;
        }
    }


    if(overlap){
        solved = solve_conflicts(overlap_node,jo_backward,lu_backward);
        if(!solved){
            cout << "No solution!" << endl;
            return;
        }
    }

    // all of the shortest paths are printed out

    cout << "Joseph's Path, duration: " << jo_backward.back().second << endl;
    print(jo_forward);
    cout << "-- return --" << endl;
    print(jo_backward);
    cout << endl;
    cout << "Lucy's Path, duration: " << lu_backward.back().second << endl;
    print(lu_forward);
    cout << "-- return --" << endl;
    print(lu_backward);
    
}

/************solve_conflicts***************
*if there is an overlap, solve_conflicts tries to solve the overlap
*if solved returns true, otherwise returns false
*takes three parameters: overlapping node and paths of Lucy and Joseph
******************************************/

bool Graph::solve_conflicts(int overlap_node, deque< pair<int,int> >& jo, deque< pair<int,int> >& lu){
    
    deque< pair<int,int> > jo_altered, lu_altered;
    int lu_invalid = 0;
    int jo_invalid = 0;
    int temp;

    // path causing overlap is removed for Lucy
    for(int i = 1; i < lu.size();i++){
        if(lu[i].first == overlap_node){
            temp = adj[lu[i-1].first][lu[i].first];
            adj[lu[i-1].first][lu[i].first] = 0;
            lu_altered = dijkstra(lu_src,lu_dest,lu_time);
            adj[lu[i-1].first][lu[i].first] = temp;
            break;
        }
    }
    
    // path causing overlap is removed for Joseph
    for(int i = 1; i < jo.size();i++){
        if(jo[i].first == overlap_node){
            temp = adj[jo[i-1].first][jo[i].first];
            adj[jo[i-1].first][jo[i].first] = 0;
            jo_altered = dijkstra(jo_src,jo_dest,jo_time);
            adj[jo[i-1].first][jo[i].first] = temp;
            break;
        }
    }

    // if there are no alternative paths, function returns false
    if(jo_altered.empty() && lu_altered.empty())
        return false;

    // if Lucy doesn't have an alternative path, lu_invalid is set
    if(lu_altered.empty())
        lu_invalid = 1;
    
    // if Joseph doesn't have an alternative path, jo_invalid is set
    if(jo_altered.empty())
        jo_invalid = 1;

    // it is checked if Joseph's alternative path and Lucy's original path intersect
    // if they have an intersection, Joseph's alternative path will be invalid
    if(!jo_invalid)
        for(int i = 0; i < jo_altered.size(); i++){
            for(int j = 0; j < lu.size(); j++){
                if(jo_altered[i] == lu[j]){
                    jo_invalid = 1;
                    break;
                }
            }
            if(jo_invalid)
                break;
    }

    // it is checked if Lucy's alternative path and Joseph's original path intersect
    // if they have an intersection, Lucy's alternative path will be invalid
    if(!lu_invalid)
        for(int i = 0; i < lu_altered.size(); i++){
            for(int j = 0; j < jo.size(); j++){
                if(lu_altered[i] == jo[j]){
                    lu_invalid = 1;
                    break;
                }
            }
            if(lu_invalid)
                break;
    }

    // if both alternative paths are invalid, there is no solution
    if(lu_invalid && jo_invalid)
        return false;

    // if Joseph's alternative path is invalid and Lucy's alternative path is valid, Lucy's path will be used
    // also if both alternative paths are valid and Lucy's path is optimal in terms of overall total
    if(jo_invalid || (!lu_invalid && (lu_altered.back().second + jo.back().second < jo_altered.back().second + lu.back().second))){
        lu.clear();
        for(int i = 0; i < lu_altered.size(); i++)
            lu.push_back(lu_altered[i]);
    }
        
    else{
        jo.clear();
        for(int i = 0; i < jo_altered.size(); i++)
            jo.push_back(jo_altered[i]);
    }

    return true;
}


class File{
    private:
        ifstream myfile;
    public:
        File(const char* file_name) { myfile.open(file_name); }
        Graph read();
        ~File() {   myfile.close(); }
};

/************read***************
* properties of graph are read from the file
* set_edges function is called to set edges of the graph
*******************************/

Graph File::read(){
    int node_number, edge_weight;
    pair<int,int> edge;
    int jo_src,jo_dest,lu_src,lu_dest;
    int count = 0;
    myfile >> node_number;
    Graph graph(node_number);
    myfile >> jo_src >> jo_dest >> lu_src >> lu_dest;
    graph.set_locations(jo_src,jo_dest,lu_src,lu_dest);
    while(myfile >> edge.first >> edge.second >> edge_weight)
        graph.set_edges(edge,edge_weight);
    return graph;
}


int main(int argc, char** argv){
    File f(argv[1]);
    Graph g = f.read();
    g.find_shortest_paths();

    return 0;
}