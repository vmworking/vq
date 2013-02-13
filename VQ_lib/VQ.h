#define VQ_API _declspec(dllexport)
#pragma once
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <list>
#include <numeric>
#include <cmath>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/dynamic_bitset.hpp>

using namespace std;

//some handmade math
class VQ_API VQm 
{
public:
    static float coord_squared( const float& v1, const float& v2 );
    static float L2distance( const vector<float>& v1, const vector<float>& v2 );
    static void vector_by_scalar_inplace( vector<float>& v, float s );
};

//bitstreaming, saving and loading bitstreams into/from a file
class VQ_API bit_stream
{
    int _blockSize;
    int _codeSize;
    bool _deleteBufferWhenFinish;
    bool _streamIsGood;
    string _inputFile;

    long long _bufferPos;
    int _bufferPosIntra;
    long long _bufferSize;
    unsigned char* _buffer;

    void _init_defaults(void);
    //moves internal pointers
    bool bit_stream::_move_pointers( int bitsRead );

public:
    bit_stream(void);
    bit_stream( const string inputFile, const int codeSize );
    bit_stream( 
        unsigned char* buffer, 
        const long long bufferSize, 
        const int codeSize 
        );
    ~bit_stream(void);
    //reads certain bits in value
    static unsigned long pick_bits( 
        const unsigned long value, 
        const int start, 
        const int offset 
        );
    //show current buffer size
    long long get_buffer_size(void);
    //returns status of the stream
    bool good(void);
    //parces whole buffer and puts it into out
    void read_all( vector<unsigned long> &out );
    //reads single code
    unsigned long read_single(void);
    //writes whole buffer into outputFile
    bool write_all( const string outputFile );

};

class VQ_API VQ
{
public:
    VQ(void);
    ~VQ(void);
};

class VQ_API VQIO
{
public:
    //reads binary file into vector of vectors of size dimensionality
    bool read_binary( 
        const string file, 
        vector<vector<float>>& X, 
        const int dimensionality
        );
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

