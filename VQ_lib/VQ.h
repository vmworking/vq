#define VQ_API _declspec(dllexport)
#pragma once
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <numeric>
#include <cmath>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>

using namespace std;

//some handmade math
class VQ_API VQm 
{
public:
    static float coord_squared( const float& v1, const float& v2 );
    static float L2distance( const vector<float>& v1, const vector<float>& v2 );
    static void vector_by_scalar_inplace( vector<float>& v, float s );
};

class VQ_API VQ
{
public:
    VQ(void);
    ~VQ(void);
};

class VQ_API coach
{
    //input vectors
    vector<vector<float>> _X;
    //centroids (code book)
    vector<vector<float>> _C;
    //encoded data (indexes of _C elements
    vector<long long> _idx;
    //features space dimensionality
    int _featuresNumber;
    //code book size is 2^_CBSizePower
    int _CBSizePower;
    //epsilons for the trainnig process
    float _stepSize, _distortionToll;

public:
    //generates code book of size 2^n into C
    bool train( 
        const vector<vector<float>>& X,
        vector<vector<float>>& C,
        vector<long long> &idx,
        const int N,
        const float stepSize,
        const float distortionToll
        );
    bool train(void){
        return train( _X, _C, _idx, _CBSizePower, _stepSize, _distortionToll );
    }
    //finds mean vectors over  X elements belonging to one code
    //and stores them in C
    void find_mean( 
        const vector<vector<float>>& X, 
        vector<vector<float>>& C, 
        const vector<long long>& idx,
        const long long CBSize
        );
    //finds nearest centroids indices, puts them into idx and computes cost
    float find_nearest_centroid( 
        const vector<vector<float>>& X, 
        const vector<vector<float>>& C, 
        vector<long long>& idx,
        const int CBSizePow
        );
    //splits centroids and offsets them 
    void split_and_move_centroids(
        vector<vector<float>>& C,
        const int CBCurrentSizePow,
        const float eps
        );
    //computes cost of a given code for a given data
    float compute_cost( 
        const vector<vector<float>>& X,
        const vector<vector<float>>& C,
        const vector<long long>& idx
        );

};

