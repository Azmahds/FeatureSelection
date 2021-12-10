#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <stdio.h>
#include <vector>

using namespace std;

struct Node {
    vector<long double> features;
    double object;
    float accuracy;
    int numFeatures;

    Node(){
        accuracy = numFeatures = 0;
        object = -1;
    }
    Node(long double creature){
        accuracy = numFeatures = 0;
        object = creature;
    }
    void addFeature(long double feature){
        features.push_back(feature);
        numFeatures = features.size();
    }
    void displayFeatures(){
        cout << "Features for object(" << object << "): {";
        int i;
        for(i = 0; i < features.size()-1; ++i){
            cout << features[i] << ", ";
        }
        cout << features[i] << '}' << endl;
    }
    double setObject(double num){
        object = num;
    }
};
#endif