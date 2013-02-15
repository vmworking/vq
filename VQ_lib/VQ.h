#define VQ_API _declspec(dllexport)
#pragma once
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <list>
#include <numeric>
#include <cmath>

using namespace std;

//////
//VQm
//////
//some handmade math
class VQ_API VQm 
{
public:
    static float coord_squared( const float& v1, const float& v2 );
    static float L2distance( const vector<float>& v1, const vector<float>& v2 );
    static void vector_by_scalar_inplace( vector<float>& v, float s );
};


//////
//bit_stream
//////
//bitstreaming, saving and loading bitstreams into/from a file
class VQ_API bit_stream
{
    int _blockSize;
    int _codeSize;
    bool _deleteBufferWhenFinish;
    bool _streamIsGood;
    string _inputFile;

    unsigned long _bufferPos;
    int _bufferPosIntra;
    unsigned long _bufferSize;
    unsigned char* _buffer;

    void _init_defaults(void);
    //checks  if there's enough bits left in _buffer to store/read a full code
    bool _check_buffer_capacity(void);
    //moves internal pointers
    bool _move_pointers( int bitsRead );

public:
    bit_stream(void);
    bit_stream( const int codeSize, const unsigned long bufferSize );
    //reads the whole file and puts its content into _buffer
    bit_stream( const string inputFile, const int codeSize );
    //creates a proper formatted _buffer from input
    bit_stream( 
        const vector<unsigned long> &input, 
        const int codeSize 
        );
    ~bit_stream(void);
    //reads certain bits in value
    static unsigned long pick_bits( 
        const unsigned long value, 
        const int start, 
        const int offset 
        );
    //returns the amount of full codes in _buffer
    unsigned long count_codes(void);
    //flushes the whole buffer into file
    bool flush( const string file );
    //show current buffer size
    unsigned long get_buffer_size(void);
    //returns status of the stream
    bool good(void);
    //parces the whole buffer and puts it into out
    void read_all( vector<unsigned long> &out );
    //reads single code
    unsigned long read_single(void);
    //puts carrets on bofferPos and bufferPosIntra if it's possible
    void seek( unsigned long bofferPos, int bufferPosIntra );
    //recreates buffer from  input
    void write_all( const vector<unsigned long> &input );
    //writes a single code in  buffer
    bool write_single( const unsigned long val );
};


//////
//VQconf
//////
struct VQ_API VQconf{
    unsigned int 
        dimensionality
        ,codeSizePower;
    float
        step, distortion;
};


//////
//VQIO
//////
class VQ_API VQIO
{
public:
    //reads binary file into vector of vectors of size dimensionality
    bool read_binary( 
        const string file, 
        vector<vector<float>>& X, 
        const int dimensionality
        );
    //writes every X element into file
    bool write_binary( 
        const string file, 
        const vector<vector<float>>& X 
        );
};


//////
//coach
//////
class VQ_API coach
{
    //input vectors
    vector<vector<float>> _X;
    //centroids (code book)
    vector<vector<float>> _C;
    //encoded data (indexes of _C elements
    vector<unsigned long> _idx;
    //features space dimensionality
    int _featuresNumber;
    //code book size is 2^_CBSizePower
    int _CBSizePower;
    //epsilons for the trainnig process
    float _stepSize, _distortionToll;

public:
    //computes cost of a given code for a given data
    float compute_cost( 
        const vector<vector<float>>& X,
        const vector<vector<float>>& C,
        const vector<unsigned long>& idx
        ); 
    //decodes idx by C and puts the result into X
    bool decode(
        vector<vector<float>>& X,
        const vector<vector<float>>& C,
        const vector<unsigned long>& idx
        );
    //finds mean vectors over  X elements belonging to one code
    //and stores them in C
    void find_mean( 
        const vector<vector<float>>& X, 
        vector<vector<float>>& C, 
        const vector<unsigned long>& idx,
        const unsigned long CBSize
        );
    //finds nearest centroids indices, puts them into idx and computes cost
    float find_nearest_centroid( 
        const vector<vector<float>>& X, 
        const vector<vector<float>>& C, 
        vector<unsigned long>& idx,
        const int CBSizePow
        );
    //splits centroids and offsets them 
    void split_and_move_centroids(
        vector<vector<float>>& C,
        const int CBCurrentSizePow,
        const float eps
        );
    //generates code book of size 2^n into C
    bool train( 
        const vector<vector<float>>& X,
        vector<vector<float>>& C,
        vector<unsigned long> &idx,
        const int N,
        const float stepSize,
        const float distortionToll
        );
    bool train(void){
        return train( _X, _C, _idx, _CBSizePower, _stepSize, _distortionToll );
    }
};


//////
//VQ
//////
//base class for vector quantization
class VQ_API VQ
{
    VQconf _conf;
    bool _isGood;
    VQIO _io;
    vector<vector<float>> _source;
    vector<vector<float>> _codeBook;
    vector<unsigned long> _encoded;
    coach _c;
public:
    VQ(void);
    ~VQ(void);
    bool configure( VQconf configuration );
    bool good(void);
    bool train(void);
    bool encode(void);
    bool decode(void);
    bool load_source( const string file );
    bool load_code_book( const string file );
    bool load_encoded( const string file );
    bool save_decoded( const string file );
    bool save_code_book( const string file );
    bool save_encoded( const string file );
};

