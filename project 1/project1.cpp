/*************************************************

   AUTHOR - ECE CINAR, 150150138

**************************************************/

#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <cstring>
#include <string>
#include <sstream>
#include <fstream>
#include <set>
#include <time.h>
using namespace std;


/*************************************************

                  STATE CLASS

**************************************************/

class State{
    public:
        int pikaHp, pikaPp;
        int blastHp,blastPp;
        string prevAttack;
        bool valid;
        char turn;
        float prob;
        int level;
        vector<State*> children;
        State* parent;
};

/*************************************************

                  ATTACK CLASS

**************************************************/

class Attack{
    public:
        string attack;
        int attackPp;
        int accuracy;
        int damage;
        int usage;
};

/*************************************************

             BATTLE CLASS & METHODS

**************************************************/

class Battle{
    private:
        vector<Attack> pikaAttack;
        vector<Attack> blastAttack;
        State* root;
        int part;
        int maxLevel;
        char mode;
    public:
        Battle(int,int,int,int);
        void setPikaAttack(Attack&);
        void setBlastAttack(Attack&);
        void initGraph(int,int);
        void createGraph(State*);
        void attackBlast(State*);
        void attackPika(State*);
        void print(State*);
        void traverseBFS();
        void traverseDFS();
        State* findFinishNode();
        void findShortestPath(char,int);
        //~Battle();
};

Battle::Battle(int pikaHP, int pikaPP, int blastHP, int blastPP){
    root = new State;
    root->pikaHp = pikaHP;
    root->pikaPp = pikaPP;
    root->blastHp = blastHP;
    root->blastPp = blastPP;
    root->turn = 'P';
    root->prob = 1;
    root->level = 0;
    root->parent = NULL;
    root->valid = 0;
    root->prevAttack = "";
}


void Battle::print(State* s){
    cout << "P_HP:" << s->pikaHp;
    cout << " P_PP:" << s->pikaPp;
    cout << " B_HP:" << s->blastHp;
    cout << " B_PP:" << s->blastPp;
    cout << " PROB:" << s->prob << endl;
}


void Battle::setPikaAttack(Attack& a){
    pikaAttack.push_back(a);
}


void Battle::setBlastAttack(Attack& a){
    blastAttack.push_back(a);
}

void Battle::initGraph(int level,int p){
    maxLevel = level;
    part = p;
    createGraph(root);
}


void Battle::createGraph(State* curr){
    if(curr->level < maxLevel && curr->blastHp && curr->pikaHp){
        if(curr->turn == 'P')
            attackBlast(curr);
        else
            attackPika(curr);
        for(int i = 0; i < curr->children.size(); i++)
            createGraph(curr->children[i]);
    }
}


void Battle::attackBlast(State* curr){
    int count = 0;
    for(int i = 0; i < pikaAttack.size(); i ++){
        if(pikaAttack[i].usage <= curr->level)
            count += 1;
    }
    
    for(int i = 0; i < pikaAttack.size();i ++){
        if(pikaAttack[i].usage <= curr->level && curr->pikaPp + pikaAttack[i].attackPp >= 0){
            State* success = new State;
            success->pikaHp = curr->pikaHp;
            success->pikaPp = curr->pikaPp + pikaAttack[i].attackPp;

            if(curr->blastHp - pikaAttack[i].damage>0)
                success->blastHp = curr->blastHp - pikaAttack[i].damage;
            else
                success->blastHp = 0;

            success->blastPp = curr->blastPp;
            success->prob = curr->prob * pikaAttack[i].accuracy  / count / 100;
            success->turn = 'B';
            success->level = curr->level + 1;
            success->parent = curr;
            success->prevAttack = pikaAttack[i].attack;
            success->valid = 1;

            curr->children.push_back(success);

            if(success->level == maxLevel && part == 1)
                print(success);
            
            if(pikaAttack[i].accuracy != 100){
                State* failure = new State;
                *failure = *success;
                failure->blastHp = curr->blastHp;
                failure->prob = curr->prob * (100 - pikaAttack[i].accuracy) / count / 100;
                failure->valid = 0;
                curr->children.push_back(failure);
                if(failure->level == maxLevel && part == 1)
                    print(failure);

            }
        }
    }
}


void Battle::attackPika(State* curr){
    int count = 0;
    for(int i = 0; i < blastAttack.size(); i ++){
        if(blastAttack[i].usage <= curr->level)
            count += 1;
    }
    for(int i = 0; i < blastAttack.size();i ++){
        if(blastAttack[i].usage <= curr->level && curr->blastPp + blastAttack[i].attackPp >= 0){
            State* success = new State;
            success->blastHp = curr->blastHp;
            success->blastPp = curr->blastPp + blastAttack[i].attackPp;

            if(curr->pikaHp - blastAttack[i].damage>0)
                success->pikaHp = curr->pikaHp - blastAttack[i].damage;
            else
                success->pikaHp = 0;

            success->pikaPp = curr->pikaPp;
            success->prob = curr->prob* blastAttack[i].accuracy / count / 100;
            success->turn = 'P';
            success->level = curr->level + 1;
            success->parent = curr;
            success->prevAttack = blastAttack[i].attack;
            success->valid = 1;

            curr->children.push_back(success);

            if(success->level == maxLevel && part == 1)
                print(success);

            if(blastAttack[i].accuracy != 100){
                State* failure = new State;
                *failure = *success;
                failure->pikaHp = curr->pikaHp;
                failure->prob = curr->prob * (100 - blastAttack[i].accuracy) / count / 100;
                failure->valid = 0;
                curr->children.push_back(failure);
                if(failure->level == maxLevel && part == 1)
                    print(failure);
            }                    
        }
    }
}


void Battle::traverseBFS(){
    int start, end;
    start = clock();
    queue<State*> waitQueue;
    int stateCount = 0;
    State* curr;
    if(root){
        waitQueue.push(root);
        while(!waitQueue.empty()){
            curr = waitQueue.front();
            waitQueue.pop();
            stateCount++;
            if(!curr->children.empty()){
                for(int i = 0; i < curr->children.size(); i++)
                    waitQueue.push(curr->children[i]);
            }
        }
    }
    end = clock();
    cout << "Node count: " << stateCount << " Running time: " << end - start << " clock ticks." << endl;
}


void Battle::traverseDFS(){
    int start,end;
    start = clock();
    stack<State*> waitStack;
    int stateCount = 0;
    State* curr;
    if(root){
        waitStack.push(root);
        while(!waitStack.empty()){
            curr = waitStack.top();
            waitStack.pop();
            stateCount++;
            if(!curr->children.empty()){
                for(int i = 0; i < curr->children.size(); i++)
                    waitStack.push(curr->children[i]);
            }
        }
    }
    end = clock();
    cout << "Node count: " << stateCount << " Running time: " << end - start << " clock ticks." << endl;
}
 
State* Battle::findFinishNode(){
    queue<State*> waitQueue;
    set<string> subStateSet;
    string subState;
    State* curr;
    if(root){
        waitQueue.push(root);
        while(!waitQueue.empty()){
            curr = waitQueue.front();
            maxLevel = curr->level + 2 ;
            waitQueue.pop();
            if((mode == 'P' && !curr->blastHp) || (mode == 'B' && !curr->pikaHp))
                return curr;
            if(curr->turn == 'P')
                attackBlast(curr);
            else
                attackPika(curr);

            if(!curr->children.empty()){
                for(int i = 0; i < curr->children.size(); i++){
                    subState = to_string(curr->children[i]->pikaHp) + '#' + to_string(curr->children[i]->pikaPp) + '#' + to_string(curr->children[i]->blastHp) + '#' + to_string(curr->children[i]->blastPp) + '#' + curr->children[i]->turn;
                    if(!subStateSet.count(subState)){
                        subStateSet.insert(subState);
                        waitQueue.push(curr->children[i]);
                    }
                }   
            }
        }
    }
    return curr;
}


void Battle::findShortestPath(char m, int p){
    part = p;
    mode = m;
    int levelCount;
    float prob;
    string player;
    stack<State*> path;
    State* traverse = new State;

    traverse = findFinishNode();
    levelCount = traverse->level;
    prob = traverse->prob;
    
    while(traverse!=root){
        path.push(traverse);
        traverse = traverse->parent;
    }
    while(!path.empty()){
        traverse = path.top();
        path.pop();
        if(traverse->turn=='B')
            player = "Pikachu";
        else
            player = "Blastoise";
        if(traverse->valid)
            cout << player << " used " << traverse->prevAttack << ". It's effective." << endl;
        else
            cout << player << " used " << traverse->prevAttack << ". It was uneffective." << endl;
    }

    cout << "Level count: " << levelCount << endl;
    cout << "Probability: " << prob << endl;
}

/*************************************************

             FILEOPS CLASS & METHODS

**************************************************/

class FileOps{
    private:
        ifstream myfile;
        string token, line;
        string fileName;
        Attack a;
    public:
        FileOps(string);        
        bool isOpen();
        void readFile(Battle&,char);
        ~FileOps();
};


FileOps::FileOps(string file){
    myfile.open(file);
    fileName = file;
}


FileOps::~FileOps(){
    myfile.close();
}


bool FileOps::isOpen(){
    if(myfile.is_open())
        return true;
    return false;
}


void FileOps::readFile(Battle& battle,char mode){

    int counter;
    myfile.ignore(1000,'\n');
    while(getline(myfile,line)){
        stringstream ss(line);
        counter=0;
        while(getline(ss,token,',')){
            switch(counter){
                case 0:
                    a.attack = token;
                    break;
                case 1:
                    a.attackPp = stoi(token);
                    break;
                case 2:
                    a.accuracy = stoi(token);
                    break;
                case 3:
                    a.damage = stoi(token);
                    break;
                case 4:
                    a.usage = stoi(token);
            }
            if(counter == 4 && mode == 'P')
                battle.setPikaAttack(a);
            else if(counter == 4 && mode == 'B')
                battle.setBlastAttack(a);
            counter = (counter + 1) % 5 ;
        }
    }
}

int main(int argc, char** argv){
   
    int level;
    ifstream myfile;
    Battle battle(200,100,200,100);
    FileOps pFile("pikachu.txt");
    
    if(pFile.isOpen())
        pFile.readFile(battle,'P');
    FileOps bFile("blastoise.txt");
    if(bFile.isOpen())
        bFile.readFile(battle,'B');

  
    if(!strcmp(argv[1],"part1")){
        level = atoi(argv[2]);
        battle.initGraph(level,1);
    }

    else if(!strcmp(argv[1],"part2")){
        
        level = atoi(argv[2]);
        battle.initGraph(level,2);

        if(!strcmp(argv[3],"dfs"))
            battle.traverseDFS();

        else if(!strcmp(argv[3],"bfs"))
            battle.traverseBFS();
    }
    else if(!strcmp(argv[1],"part3")){
        
        if(!strcmp(argv[2],"pikachu"))
            battle.findShortestPath('P',3);
        
        else
            battle.findShortestPath('B',3);
    }
    
    return 0;
}