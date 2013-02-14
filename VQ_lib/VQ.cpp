#include "stdafx.h"
#include "VQ.h"
using namespace std;
//////
//VQm
//////
float VQm::coord_squared( const float& v1, const float& v2 )
{
    return (v1 - v2) * (v1 - v2);
}
float VQm::L2distance( const vector<float>& v1, const vector<float>& v2 )
{
    return inner_product( 
                v1.begin(), 
                v1.end(), 
                v2.begin(), 
                0.0, 
                plus<float>(), 
                coord_squared
                );
}
void VQm::vector_by_scalar_inplace( vector<float>& v, const float s )
{
    vector<float>::iterator it( v.begin() );
    while( it != v.end() )
        *it++ = *it * s;
}


//////
//bit_stream
//////
bit_stream::bit_stream(void)
{
    _init_defaults();
}
bit_stream::bit_stream( const int codeSize, const long long bufferSize)
{
    _init_defaults();
    if( bufferSize > 0 ){
        _codeSize = codeSize;
        _bufferSize = bufferSize;
        _deleteBufferWhenFinish = true;
        _buffer = new unsigned char[ _bufferSize ];
        for( long long i = 0; i < _bufferSize; i++ )
            _buffer[i] = 0;
        _streamIsGood = true;
    }

}

//reads the whole file and puts its content into _buffer
bit_stream::bit_stream( const string inputFile, const int codeSize )
{
    _init_defaults();
    if( codeSize > 0 ){
        //input stream
        ifstream s( inputFile, ios_base::binary );
        if( s.good() ){
            s.seekg( 0, ios::end );
            long long fileSize = s.tellg();
            s.seekg( 0, ios::beg );
            _buffer = new unsigned char[ fileSize  ];
            s.read( reinterpret_cast<char*>(_buffer), fileSize );
            s.close();
            _deleteBufferWhenFinish = true;
            _bufferSize = fileSize;
            _codeSize = codeSize;
            _streamIsGood = true;
        }
    }
}
//creates a proper formatted _buffer from inputV
bit_stream::bit_stream( 
        const vector<unsigned long> &input, 
        const int codeSize 
    )
{
    _init_defaults();
    _codeSize = codeSize;
    write_all( input );
}

bit_stream::~bit_stream(void)
{
    if( _deleteBufferWhenFinish ) delete[] _buffer;
}
//check  if there's enough bits left in buffer to store/read a full code
bool bit_stream::_check_buffer_capacity(void)
{
    if ( 
        ( _blockSize - _bufferPosIntra ) //how many bits is left in the current block
        + ( _bufferSize - (_bufferPos + 1) ) * _blockSize < _codeSize 
        )
        return _streamIsGood = false;
    return true;
}
void bit_stream::_init_defaults(void)
{
    _deleteBufferWhenFinish = false;
    _streamIsGood = false;
    _inputFile = "";
    _codeSize = 8;
    _bufferSize = 0;
    _bufferPos = 0;
    _bufferPosIntra = 0;
    _buffer = NULL;
    _blockSize = 8;
}
//moves internal pointers
bool bit_stream::_move_pointers( int bitsRead )
{
    _bufferPosIntra += bitsRead;
    if ( ( _bufferPosIntra >= _blockSize ) ) {
        _bufferPos++;
        _bufferPosIntra = 0;
    }
    return true;
}
//reads certain bits in value
unsigned long bit_stream::pick_bits( 
        const unsigned long value, 
        const int start, 
        const int offset 
        )
{
    unsigned long bf = 0;
    int containerLength = 8 * sizeof( bf );
    bf = ( ( value >> start ) << ( containerLength - offset ) ) >> ( containerLength - offset );
    return bf;
}
//returns the amount of full codes in _buffer
long long bit_stream::count_codes(void)
{
    return (long long) _bufferSize * _blockSize / _codeSize;
}
//flushes the whole buffer into file
bool bit_stream::flush( const string file )
{
    if( _bufferSize > 0 && _buffer != NULL ){
        ofstream ofs( file, ios_base::binary );
        if( ofs.good() ){
            ofs.write( reinterpret_cast<char *>( _buffer ), _bufferSize );
            ofs.close();
            return true;
        }
        return false;
    }
    return false;
}
//shows current buffer size
long long bit_stream::get_buffer_size(void){
    return _bufferSize;
}
//returns status of the stream
bool bit_stream::good(void){
    return _streamIsGood;
}
//parces the whole buffer and puts it into out
void bit_stream::read_all( vector<unsigned long> &out )
{
    if( _streamIsGood ){
        out.clear();
        out.resize( count_codes() );
        seek( 0, 0 );
        vector<unsigned long>::iterator it( out.begin() );
        while( _streamIsGood )
            *it++ = read_single();
    }
}
//reads single code
unsigned long bit_stream::read_single(void)
{
    if( _streamIsGood && _check_buffer_capacity() ) {
        int codePosIntra = 0;
        unsigned long bfLF = 0, code = 0;
        while( _streamIsGood && codePosIntra < _codeSize ){
            bfLF = _buffer[ _bufferPos ];
            int bitsToReadFromCurrentByte = min( 
                                      _codeSize - codePosIntra, 
                                      _blockSize - _bufferPosIntra
                                      );
            bfLF = pick_bits( 
                    bfLF, 
                    _bufferPosIntra, 
                    bitsToReadFromCurrentByte 
                    );
            code += ( bfLF ) << codePosIntra;
            codePosIntra += bitsToReadFromCurrentByte;
            _move_pointers( bitsToReadFromCurrentByte );
        }
        _check_buffer_capacity();
        return code;
    }
    return -1;
}
//puts carrets on bofferPos and bufferPosIntra if it's possible
void bit_stream::seek( long long bofferPos, int bufferPosIntra )
{
    if (
        bofferPos < _bufferSize 
        && bofferPos >= 0
        && bufferPosIntra < _blockSize
        && bufferPosIntra >= 0
        ){
        _bufferPos = bofferPos;
        _bufferPosIntra = bufferPosIntra;
        _streamIsGood = true;
        _check_buffer_capacity();
    }
}
//recreates buffer from  input
void bit_stream::write_all( const vector<unsigned long> &input )
{
    if( input.size() > 0 && _codeSize > 0 ){
        if( _buffer != NULL && _deleteBufferWhenFinish ) delete[] _buffer;
        _bufferSize = ceil( (double)( input.size() * _codeSize ) / _blockSize );
        _buffer = new unsigned char[ _bufferSize ];
        for( long long i = 0; i < _bufferSize; i++ )
            _buffer[i] = 0;
        _deleteBufferWhenFinish = true;
        seek( 0, 0 );
        vector<unsigned long>::const_iterator it( input.begin() );
        while( it != input.end() )
            write_single( *it++ );
    }
}
//writes a single code in  buffer
bool bit_stream::write_single( const unsigned long val )
{
    if( _streamIsGood && _check_buffer_capacity() ) {
        int codePosIntra = 0;
        unsigned char bfLF = 0;
        while( codePosIntra < _codeSize ){
            int bitsToWriteToCurrentByte = min( 
                                      _codeSize - codePosIntra, 
                                      _blockSize - _bufferPosIntra
                                      );
            bfLF = pick_bits( 
                    val, 
                    codePosIntra, 
                    bitsToWriteToCurrentByte 
                    );
            _buffer[ _bufferPos ] += ( bfLF ) << _bufferPosIntra;
            codePosIntra += bitsToWriteToCurrentByte;
            _move_pointers( bitsToWriteToCurrentByte );
        }
        _check_buffer_capacity();
        return true;
    }
    return false;       
}

//////
//VQ
//////
//base class for vector quantization
VQ::VQ(void)
{
    _isGood = false;
}
VQ::~VQ(void)
{
}
bool VQ::configure( VQconf c )
{
    if( 
        ( c.codeSizePower > 0 ) 
        && ( c.codeSizePower < 32 )
        && ( c.dimensionality > 0 )
        && ( c.dimensionality < 100 )
        ){
        _conf = c;
        return _isGood = true;
    }
    return false;
}
bool VQ::good(void)
{
    return _isGood;
}
bool VQ::train(void)
{
    if( good() ){
        return _c.train( 
                    _source, 
                    _codeBook, 
                    _encoded,
                    _conf.codeSizePower,
                    _conf.step, 
                    _conf.distortion 
                    );
    }
    return false;
}
bool VQ::encode(void)
{
    return false;
}
bool VQ::decode(void)
{
    return false;
}
bool VQ::load_source( const string file )
{
    return _io.read_binary( file, _source, _conf.dimensionality );
}
bool VQ::load_code_book( const string file )
{
    return false;
}
bool VQ::load_encoded( const string file )
{
    return false;
}
bool VQ::save_decoded( const string file )
{
    return false;
}
bool VQ::save_code_book( const string file )
{
    return _io.write_binary( file, _codeBook );
}
bool VQ::save_encoded( const string file )
{
    return false;
}


//////
//VQIO
//////
//reads binary file into a vector of vectors of size dimensionality
bool VQIO::read_binary( 
    const string file, 
    vector<vector<float>>& X, 
    const int dimensionality
    )
{
    X.clear();
    ifstream ifsT( file, ios_base::binary );
    if( ifsT.good() ){
        ifsT.seekg( 0, ios::end );
        long long fileSize = ifsT.tellg(), setSize = fileSize / dimensionality / 4;
        if( fileSize > 0 && ( fileSize % dimensionality == 0 ) ){
            ifsT.seekg( 0, ios::beg );
            float* bf = new float[ dimensionality * setSize  ];
            ifsT.read( reinterpret_cast<char*>(bf), fileSize );
            ifsT.close();
            X.resize( setSize );
            vector<vector<float>>::iterator itX( X.begin() );
            for( int i = 0; i < setSize; i++ ){
                *itX++ = vector<float>( 
                                bf + i * dimensionality, 
                                bf + (i + 1) * dimensionality 
                                );
            }

            return true;
        }

    }
    return false;
}
//writes every X element into file
bool VQIO::write_binary( const string file, const vector<vector<float>>& X )
{
    if( X.size() > 0 ){
        ofstream ofs( file, ios_base::binary );
        if( ofs.good() ){
            vector<vector<float>>::const_iterator itX( X.begin() );
            int dimensionality = (*itX).size();
            while( itX != X.end() ){
                vector<float> v( *itX++ );
                ofs.write( reinterpret_cast<char *>(&v[0]), dimensionality * 4 );
            }
            ofs.close();
            return true;
        }
    }
    return false;
}


//////
//coach
//////
//generates code book of size 2^n into C
bool coach::train( 
        const vector<vector<float>>& X,
        vector<vector<float>>& C,
        vector<long long>& idx,
        const int N,
        const float stepSize,
        const float distortionToll
        )
{
    int featuresNum = X[0].size(), CBCurrentPower = 0;
    vector<float> v0( featuresNum, 0 );
    C.clear();
    C.resize( pow( 2, N ), v0 );
    idx.clear();
    idx.resize( X.size(), 0 );
    find_mean( X, C, idx, 1 );
    for( int n = 0; n < N; n++ ){
        split_and_move_centroids( C, n, stepSize );
        float costPrev = -1.0, cost = costPrev;
        bool moveCentroids = true;
        
        //dbuging
        cout << " n is --------" << n << endl;
        unsigned long moves = 0;
        //-------

        while( moveCentroids ){
            find_nearest_centroid( X, C, idx, n + 1 );
            find_mean( X, C, idx, pow( 2, n + 1 ) );
            cost = compute_cost( X, C, idx ) / pow( 2, n + 1 ) / X.size()  ;
            moveCentroids = ( moveCentroids < 0 ) || ( ( costPrev - cost ) / costPrev > distortionToll );
            costPrev = cost;
            
            //dbuging
            cout << " move is --------" << ++moves << endl;
            //-------

        }
    }
    return true;
}
//finds mean vectors over  X elements belonging to one code
//and stores them in C
void coach::find_mean( 
    const vector<vector<float>>& X, 
    vector<vector<float>>& C, 
    const vector<long long>& idx,
    const long long CBSize
    )
{
    vector<vector<float>>::const_iterator itX( X.begin() );
    vector<long long>::const_iterator itIdx( idx.begin() );
    int dimensionality = (*itX).size();
    vector<float> bfV( dimensionality, 0.0 );
    vector<long long> quantity( CBSize, 0 );
    fill_n( C.begin(), CBSize, bfV );
    while( itX != X.end() ){
        for( int i=0; i < dimensionality; i++ ){
            C[ *itIdx ][i] += (*itX).at( i );
        }        
        quantity[ *itIdx ]++;
        itX++; itIdx++;
    }
    vector<vector<float>>::iterator itC( C.begin() );
    itIdx = quantity.begin();
    while( itIdx != quantity.end() ){
        if( *itIdx != 0 ) VQm::vector_by_scalar_inplace( *itC,(float) 1/ *itIdx );
        itC++; itIdx++;
    }
   
}
//finds nearest centroids indices and puts them into idx
float coach::find_nearest_centroid( 
    const vector<vector<float>>& X, 
    const vector<vector<float>>& C, 
    vector<long long>& idx,
    const int CBSizePow
    )
{
    vector<vector<float>>::const_iterator itX( X.begin() ), itC( C.begin() );
    long bookSize = pow( 2, CBSizePow ), i = 0;
    //current position in X
    long long iX = 0;
    float minDist, argMinDist, cost = 0 , bf;
    
    //debuging
    int xco = 0;
    //-----
    
    while( itX != X.end() ){
        
        //debuging
        if ( xco == 50 )
            xco = 100;
        xco++;
        //-----

        i = argMinDist = 0;
        idx[ iX ] = i;
        minDist = VQm::L2distance( *itX, C[i] );
        while( i < bookSize ){
            if ( ( bf = VQm::L2distance( C[i], *itX ) ) < minDist ){
                idx[ iX ] = i;
                minDist = bf;
            }
            i++;
        }
        cost = cost + minDist;
        iX++;
        itX++;
    }

    return cost;
    
}
//splits centroids and offsets them 
void coach::split_and_move_centroids(
    vector<vector<float>>& C,
    const int CBCurrentSizePow,
    const float eps
    )
{
    long long CBCurrentSize = pow( 2, CBCurrentSizePow );
    copy_n( C.begin(), CBCurrentSize, C.begin() + CBCurrentSize );
    for( long long i = 0; i < CBCurrentSize; i++ ){
        VQm::vector_by_scalar_inplace( C[i], ( 1 + eps ) );
        VQm::vector_by_scalar_inplace( C[ CBCurrentSize + i ], ( 1 - eps ) );
    }
}
//computes cost of a given code for a given data
float coach::compute_cost( 
    const vector<vector<float>>& X,
    const vector<vector<float>>& C,
    const vector<long long>& idx
    )
{
    vector<vector<float>>::const_iterator itX( X.begin() );
    vector<long long>::const_iterator itIdx( idx.begin() );
    float cost = 0.0f;
    while( itX != X.end() )
        cost += VQm::L2distance( *itX++, C[ *itIdx++ ] );
    
    return cost;
}

