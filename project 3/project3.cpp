/*************************************************

   AUTHOR - ECE CINAR, 150150138
   Compilation: g++ -std=c++11 150150138.cpp
**************************************************/

#include <iostream>
#include <vector>
#include <queue>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <algorithm>
using namespace std;

/*********************************************************
 * TestSuite Class: Represents test suites 
 * ******************************************************/

class TestSuite{
    private:
        string testSuiteId;
        int bugsDetected;
        int runningTime;
        vector< vector<int> > testFreqs;
    public:
        string getTestSuiteId(){   return testSuiteId;}
        void setTestSuiteId(string testSuiteId){ this->testSuiteId = testSuiteId; }
        int getBugsDetected(){  return bugsDetected;    }
        void setBugsDetected(int bugsDetected){ this->bugsDetected = bugsDetected; }
        int getRunningTime(){   return runningTime; }
        void setRunningTime(int runningTime){ this->runningTime = runningTime; }
        vector< vector<int> > getTestFreq(){   return testFreqs;    } 
        void setTestFreqs(vector<int> testFreq){ testFreqs.push_back(testFreq);    }
        void printTest();
};

/*********************************************************
 * printTest Method: prints ID, bug number and running time of test suite 
 * ******************************************************/

void TestSuite::printTest(){
    cout << "Test Id: " << testSuiteId;
    cout << " Bugs Detected: " << bugsDetected;
    cout << " Running Time: " << runningTime << endl;  
}

/*********************************************************
 * Optimization Class:
 * Consists of optimization methods and their helpers
 * ******************************************************/
class Optimization{
    private:
        vector<TestSuite> testList;
        vector<TestSuite> selectedList;
        vector< vector<int> > finalOrders;
        int timeLimit, totalRunTime;
    public:
        void setTests(TestSuite t){  testList.push_back(t); }
        void setTimeLimit(int timeLimit) {  this->timeLimit = timeLimit;    }
        void selectSubset();
        int calculateDistance(vector<int>, vector<int>);
        vector< vector<int> > orderFrequencies(vector< vector<int> >);
        void prioritize();
        void printOutputs();
};

/*********************************************************
 * selectSubset Method:
 * selects a subset of tests
 * maximizes # of bugs subject to the time limit.
 * inserts selected suites into selectedList vector
 * ******************************************************/

void Optimization::selectSubset(){
    int row = testList.size(), col = timeLimit, t, b;
    totalRunTime = 0;
    //initializing table
    vector<vector<int> > table(row + 1, vector<int>(col + 1));
    for(int i = 1; i < row + 1; i++){
        t = testList[i-1].getRunningTime();
        b = testList[i-1].getBugsDetected();
        for(int j = 1; j < col + 1; j++){
            // if running time is greater than the column's time, nothing changes
            if(t > j)
                table[i][j] = table[i-1][j];
            else
                table[i][j] = max(table[i-1][j], table[i-1][j - t] + b);            
        }
    }
    //tracing back the selected test suites
    while(row > 0){
        if(table[row][col] > table[row - 1][col]){
            // selected test suites are pushed back into selectedList vector
            selectedList.push_back(testList[row-1]);
            totalRunTime += testList[row-1].getRunningTime();
            col -= testList[row-1].getRunningTime();
        }
        row--;
    }
}

/*********************************************************
 * calculateDistance Method:
 * calculates how dissimilar two vectors are
 * Levenshtein's Algorithm is used
 * returns dissimilarity rate
 * ******************************************************/

int Optimization::calculateDistance(vector<int> vecOne, vector<int> vecTwo){
    int row = vecTwo.size(), col = vecOne.size();
    // initializing table
    vector<vector<int> > table(row + 1, vector<int>(col + 1));
    int cost;
    // 0th row and column values are assigned
    for(int i = 0; i < row + 1; i++)
        table[i][0] = i;

    for(int i = 0; i < col + 1; i++)
        table[0][i] = i;

    for(int i = 1; i < row + 1; i++){
        for(int j = 1; j < col + 1; j++){
            // if the numbers are the same, cost is zero. Otherwise 1
            cost = 1 - (vecTwo[i-1] == vecOne[j-1]); 
            //choosing the operation that minimizes comparison cost
            table[i][j] = min(table[i-1][j] + 1, min(table[i][j-1] + 1, table[i-1][j-1]) + cost);
        }
    }
    return table[row][col];
}
/*********************************************************
 * orderFrequencies Method:
 * Orders frequency indexes from least executed to most executed
 * returns ordered vector of integer vectors
 * ******************************************************/

vector< vector<int> > Optimization::orderFrequencies(vector< vector<int> > testFreqs){
    vector< vector<int> > orderedFreqs;
    int size = testFreqs.size();

    for(int i = 0; i < size; i++){
        multimap<int,int> orderedFreq;
        // inserting each element of a frequency into multimap along with its index
        // multimap sorts keys from least frequent to most so that the indexes are ordered
        for(int j = 0; j < testFreqs[i].size(); j++)
            orderedFreq.insert(make_pair(testFreqs[i][j], j));

        multimap<int,int>::iterator it = orderedFreq.begin();
        vector<int> frequency;
        // inserting values of each key value pair into frequency vector
        while(it != orderedFreq.end())
            frequency.push_back(it++->second + 1);
        orderedFreqs.push_back(frequency); 
    }
    return orderedFreqs;
}

/*********************************************************
 * prioritize Method:
 * Sorts test cases of each selected test suite and stores them in finalOrders vector
 * Helpers: calculateDistance and orderFrequencies
 * ******************************************************/

void Optimization::prioritize(){
    vector< vector<int> > testFreqs;
    int zeroCount, min;

    for(int i = 0; i < selectedList.size(); i++){
        // frequencies of the selected test suite is obtained 
        testFreqs = selectedList[i].getTestFreq();
        min = testFreqs[0].size() + 1;
        multimap<int,int> zeroCounts;
        // for each frequency profile, number of zeros are calculated
        // (# of zeros, idx) key value pairs are inserted into zeroCounts
        for(int j = 0; j < testFreqs.size(); j++){
            zeroCount = count(testFreqs[j].begin(), testFreqs[j].end(), 0);
            zeroCounts.insert(make_pair(zeroCount,j));
            // minimum # of zeros is calculated 
            if(zeroCount < min)
                min = zeroCount;                
        }
        vector<int> finalOrder;
        multimap<int,int>::iterator it = zeroCounts.begin();
        // test cases with min zero count are inserted into the beginning of finalOrder
        while(it != zeroCounts.end() && it->first == min)
            finalOrder.push_back(it++->second + 1);
        // all frequency profiles are ordered by calling orderFrequencies
        vector< vector<int> > orderedFreqs = orderFrequencies(testFreqs);
        // baseVec is the frequency profile with min zeros and smaller index
        vector<int> baseVec = orderedFreqs[finalOrder[0]-1];
        multimap<int,int, greater<int> > costOrder;
        // dissimilarity between baseVec and other vecs are calculated calling calculateDistance
        // other vectors with minimum zero number are not included
        for(int j = 0; j < orderedFreqs.size(); j++)
            if(!count(finalOrder.begin(), finalOrder.end(), j + 1))
                costOrder.insert(make_pair(calculateDistance(baseVec, orderedFreqs[j]), j + 1));
        // (dissimilarity, index) pairs are inserted into costOrder multimap
        // multimap keeps the keys sorted in descending order, the order we want
        multimap<int, int, greater<int> >::iterator iter = costOrder.begin();
        // finalOrders vector is finalized by the remaining test cases in order
        while(iter != costOrder.end())
            finalOrder.push_back(iter++->second);
        finalOrders.push_back(finalOrder);
    }
}

/*********************************************************
 * printOutputs Method:
 * prints findings in required format
 * ******************************************************/

void Optimization::printOutputs(){
    cout << "Selected test suites: " << endl;
    for(int i = selectedList.size() - 1; i >= 0; i--)
        selectedList[i].printTest();
    cout << "Total running time: " << totalRunTime << endl;
    cout << "Test case orderings: " << endl;
    for(int i = finalOrders.size() - 1; i >= 0; i--){
        cout << selectedList[i].getTestSuiteId() << " ";
        for(int j = 0; j < finalOrders[i].size(); j++){
            cout << finalOrders[i][j] << " ";
        }
        cout << endl;
    }
}

/*********************************************************
 * File Class:
 * performs file operations
 * ******************************************************/

class File{
    private:
        ifstream myfile;
    public:
        File(const char* file_name) { myfile.open(file_name); }
        Optimization read();
        ~File() {   myfile.close(); }
};

/*********************************************************
 * read Method: reads required info from the input file
 * Creates Optimization instance and returns it
 * Uses stringstream and tokenizes inputs into strings
 * ******************************************************/

Optimization File::read(){
    Optimization opt;
    string line, token;
    getline(myfile,line);
    stringstream ss(line);

    // line is seperated by : and second token is taken
    for(int i = 0; i < 2; i++)
        getline(ss,token,':');

    // time limit is set
    opt.setTimeLimit(stoi(token));

    // second two lines are ignored
    for(int i = 0; i < 2; i++)
        myfile.ignore(1000,'\n');

    // lines of text file are read
    while(getline(myfile,line)){
        stringstream ss(line);
        TestSuite test;
        vector<string> temp;
        // each line is divided into tokens by space char.
        // tokens are inserted into temp vector
        while(getline(ss,token,' '))
            temp.push_back(token);
        //first three tokens of temp are used to set ID, bug # and runtime
        test.setTestSuiteId(temp[0]);
        test.setBugsDetected(stoi(temp[1]));
        test.setRunningTime(stoi(temp[2]));

        // remaining tokens are frequency profiles and they are converted into integer vectors
        for(int i = 3; i < temp.size(); i++ ){
            stringstream iss(temp[i]);
            vector<int> testFreq;
            while(getline(iss,token,'-'))
                testFreq.push_back(stoi(token));
            // converted integer frequency vector is used to set frequencies 
            test.setTestFreqs(testFreq);
        }
        // each test suite is inserted into testList of Optimization instance
        opt.setTests(test);
    }
    return opt;
};


int main(int argc, char** argv){
    File f(argv[1]);
    Optimization opt = f.read();
    opt.selectSubset();
    opt.prioritize();
    opt.printOutputs();

    return 0;

}