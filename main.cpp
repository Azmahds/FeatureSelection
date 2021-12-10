#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <cmath>
#include <algorithm>
#include <time.h>
#include "Node.h"

using namespace std;

vector<Node*> getFileInput(string fileName);
bool NearestNeighbor(vector<Node*>, Node*, int, vector<int>);
void ForwardSelection();
void BackwardElimination();
double LeaveOneOut(int featNum, vector<int> bestFeats);
long double distance(vector<long double>,vector<long double>, vector<int>);
double randVal();
void outputFeatureSet(vector<int>);
vector<vector<int>> FindSubsets(vector<int>);

vector<Node*> file_data;

int main(int argc, char *argv[]){
    string fileName;
    int algorithm = 1;

    cout << endl << "Welcome to Hamza Syed's Feature Selection Algorithm" << endl;
    cout << "Type in the name of the file to test: ";
    cin >> fileName;
    file_data = getFileInput(fileName);
    cout << endl << endl;

    cout << "Select the number of the algorithm you want to run." << endl << endl;
    cout << "\t (1) Forward Selection\n \t (2) Backward Elimination\n";
    cin >> algorithm;
    cout << endl << endl;

    if(algorithm != 1 && algorithm != 2){
        cout << "Invalid selection, now using algorithm **(1) Forward Selection**" << endl << endl;
        algorithm = 1;
    }

    cout << "This data set has " << file_data[0]->numFeatures << " features (not including class attribute), with " << file_data.size() << " instances." << endl << endl;

    time_t start = time(0);

    if(algorithm == 1){
        ForwardSelection();   
    }
    else{
        BackwardElimination();
    }
    double min_since_start = difftime( time(0), start)/60.0;

    cout << endl << "Total time until completion was " << min_since_start << " minutes." << endl << endl;

    return 0;
}

void ForwardSelection(){
    double totalAcc = 0.0, highestAcc = 0.0, localHigh = 0.0;
    int numFt = file_data[0]->numFeatures, cnt = 0, highestFeatIndex = -1, localHighFeatIndex = -1;
    char input = 'n';
    bool skip = false, decrease = false;
    vector<int> bestFeats, temp, continueSearch;

    cout << "Beginning Search: " << endl << endl;
    while(!decrease || temp.size() < numFt - 1){
        localHigh = 0.0; localHighFeatIndex = -1;
        for(int i = 0; i < numFt; ++i){
            for(int j = 0; j < temp.size(); ++j){
                if(temp.at(j) == i){skip = true; break;}
            }
            if(skip){skip = false; continue;}
            totalAcc = LeaveOneOut(i, temp);
            cout << "\tUsing feature(s) "; 
            if(temp.size() == 0){cout << '{';}
            else{outputFeatureSet(temp); cout << ", ";}
            cout <<  i+1 << "}, accuracy is " << totalAcc << '%' << endl;
            if(localHigh < totalAcc){localHigh = totalAcc; localHighFeatIndex = i;}
        }
        if(localHigh > highestAcc){highestAcc = localHigh; highestFeatIndex = localHighFeatIndex; bestFeats.push_back(highestFeatIndex); temp = bestFeats;}
        else{decrease = true; cout << endl << "WARNING: Accuracy Decreased!! "; if(input != 'y'){cout << "Continue full search in case of local maxima? (y/n) "; input = 'y'; if(input != 'y'){break;}}continueSearch = temp; continueSearch.push_back(localHighFeatIndex); temp = continueSearch;}
        cout << endl << "Feature(s) "; outputFeatureSet(temp); cout << "} was best, with accuracy " << localHigh << "%" << endl << endl;
    }
    cout << endl << "Finished search! The best feature subset is "; outputFeatureSet(bestFeats); cout << "} with an accuracy of " << highestAcc << '%' << endl << endl;
}

void BackwardElimination(){
    vector<int> features, bestSet;
    vector<vector<int>> subsets;
    double accuracy = 0.0, highestAcc = -1.0, localHigh = -1.0;
    int subsetSize = file_data[0]->numFeatures, bestSubsetIndex = -1, localIndex;

    cout << "Beginning Search: " << endl << endl;
    for(int i = 0; i < subsetSize; ++i){
        features.push_back(i);
    }
    --subsetSize;
    highestAcc = LeaveOneOut(-1, features);
    cout << "\tUsing feature(s) "; outputFeatureSet(features); cout << "}, accuracy is " << highestAcc << '%' << endl;

    for(int i = 0; i < subsetSize; ++i){
        subsets = FindSubsets(features);
        localHigh = 0.0;
        for(int j = 0; j < subsets.size(); ++j){
            accuracy = LeaveOneOut(-1, subsets.at(j));
            cout << "\tUsing feature(s) "; outputFeatureSet(subsets.at(j)); cout << "}, accuracy is " << accuracy << '%' << endl;
            if(accuracy > localHigh){localHigh = accuracy; localIndex = j;}
            accuracy = 0.0;
        }
        if(localHigh < highestAcc){cout << "WARNING: Accuracy Decreased!! " << endl;}
        cout << endl << "Feature(s) ";
        outputFeatureSet(subsets[localIndex]); 
        cout << "} was best, with accuracy " << localHigh << "%" << endl << endl;
        features = subsets.at(localIndex);
        if(localHigh > highestAcc){highestAcc = localHigh; bestSubsetIndex = localIndex; bestSet = features;}
    }
    cout << endl << "Finished search! The best feature subset is "; outputFeatureSet(bestSet); cout << "} with an accuracy of " << highestAcc << '%' << endl << endl;
}

vector<vector<int>> FindSubsets(vector<int> set){
    vector<vector<int>> subsets;
    vector<int>::iterator it;
    int numMissing = file_data[0]->numFeatures - set.size();

    
    bool valid = false;
        for(int i = 0; i < set.size()*10; ++i){
            if(subsets.size() == set.size()){break;}
            vector<int> temp = {};
            if(find(set.begin(), set.end(), i) == set.end()){continue;}

            for(int j = 0; j < file_data[0]->numFeatures; ++j){
                if(j == i){continue;}
                valid = false;
                for(int k = 0; k < set.size(); ++k){
                    if(j == set[k]){
                        valid = true;
                        break;
                    }
                }
                if(valid && !(temp.size()+1 >= set.size())){temp.push_back(j);}
            }
            if(find(subsets.begin(), subsets.end(), temp) == subsets.end())subsets.push_back(temp);
        }
    return subsets;
}

double LeaveOneOut(int featNum, vector<int> bestFeats){
    vector<Node*> temp;
    Node* testRow;
    double matched = 0.0;
    double accur = 0.0;

    for(int j = 0; j < file_data.size(); ++j){
        vector<Node*> temp = file_data;
        testRow = temp.at(j);
        swap(temp.at(j), temp.at(temp.size()-1));
        temp.pop_back();
        if(NearestNeighbor(temp, testRow, featNum, bestFeats)){++matched;}
    }
    
    accur = (matched/file_data.size()) * 100;
    return accur;
}

bool NearestNeighbor(vector<Node*> train, Node* test, int ftNum, vector<int> bestFeats){
    long double minDist = 1000000, dist = 0.0;
    int numFt = test->numFeatures;
    Node* temp;
    
    if(ftNum != -1){bestFeats.push_back(ftNum);}

    for(int i = 0; i < train.size(); ++i){
        if(train[i] == test){continue;}
        
        dist = distance(train[i]->features, test->features, bestFeats);

        if(minDist > dist){
            minDist = dist;
            temp = train.at(i);
        }
        dist = 0.0;
    }
    bestFeats.pop_back();
    return (temp->object == test->object);
}

long double distance(vector<long double> x, vector<long double> y, vector<int> feats){
    long double val = 0.0;
    vector<long double> temp;

    for(int i = 0; i < feats.size(); ++i){
        temp.push_back(pow(x.at(feats[i]) - y.at(feats[i]), 2));
    }
    for(int i = 0; i < temp.size(); ++i){
        val += temp[i];
    }
    return val;
}

double randVal(vector<Node*> data){
    return (rand()%100+1);
}

void outputFeatureSet(vector<int> set){
    int i;
    if(set.size() > 0){cout << "{" << set[0]+1;}
    for(i = 1; i < set.size(); ++i){
        cout << ", " << set[i]+1;
    }
}

vector<Node*> getFileInput(string fileName){
    vector<Node*> data;
    Node* temp;
    ifstream fin;

    fin.open(fileName);

    if(!fin.is_open()){
        cout << "Error opening file " << fileName << ". Please enter a valid data file." << endl;
        exit(0);
    }

    string buffer;

    while(getline(fin, buffer)){
        stringstream dataLine(buffer);
        long double feature;
        dataLine >> feature;
        temp = new Node(feature);
        while(dataLine >> feature){
            temp->addFeature(feature);
        }
        data.push_back(temp);
    }

    return data;
}