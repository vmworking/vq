#include "VQ.h"
using namespace std;

const bool Debug = true;
template <class T>
void debugP( T s ) 
{
    if (Debug) cout<<"    "<< s <<'\n';
}

template <class T1, class T2>
void debugP( T1 s1, T2 s2 ) 
{
    if (Debug) cout<<"    "<< s1 <<" "<< s2 <<'\n';
}

int load_test_data( 
    string file,
    vector<vector<float>>& X, 
    vector<vector<float>>& C,
    vector<long long>& idx
    )
{
    vector<float> vs;
    vector<float>::iterator vit;
    ifstream input( file );    
    if ( input.good() ){
        istream_iterator <float> iter( input ), eos; 
        copy( iter, eos, back_inserter( vs ) );
        input.close();
        vit = vs.begin(); 
        vit += 2;
        int i = 0;
        int samplesNumber = vs[ 0 ], featuresNumber = vs[ 1 ];
        X.resize( samplesNumber );
        for ( i=0; i < samplesNumber; i++ ){
            copy_n( vit, featuresNumber, back_inserter( X[ i ] ) );
            vit += featuresNumber;
        }
        int codeBookPower = *vit++;
        int codeBookSize = pow( 2.0, codeBookPower );
        C.resize( codeBookSize );
        for ( i=0; i < codeBookSize; i++ ){
            copy_n( vit, featuresNumber, back_inserter( C[ i ] ) );
            vit += featuresNumber;
        }
        copy_n( vit, samplesNumber, back_inserter( idx ) );
        
        return codeBookPower;
    }
    
    return 0;
}

void load_test_io(
    string file,
    list<float>& l
    )
{
    ifstream input( file );
    if ( input.good() ){
        copy( 
            istream_iterator <float>( input ), 
            istream_iterator <float>(), 
            back_inserter( l ) 
            );
    }
}

unsigned long read_bits( const unsigned long value, const int start, const int offset )
{
    unsigned long bf = 0;
    int containerLength = 8 * sizeof( bf );
    bf = ( ( value >> start ) << ( containerLength - offset ) ) >> ( containerLength - offset );
    return bf;
}

int main(int argc, char* argv[])
{
    coach c;
   /*
    vector<vector<float>> m1;
    vector<vector<float>> X, C, COutput;
    vector<long long> idx, idxOutput;
    int cbpower = load_test_data( "../tests/test5dw.dat", X, C, idx );
    debugP( "X size", X.size() );
    debugP( "Xb ", X[0][0] );
    debugP( "Xb ", X[0][1] );
    debugP( "Xb ", X[1][0] );
    debugP( "Xb ", X[1][1] );
    debugP( "C size", C.size() );
    debugP( "C", C[0][0] );
    debugP( "C", C[0][1] );
    debugP( "Idx size", idx.size() );
    debugP( "Idx", idx[0] );
    debugP( "Idx", idx[0] );
    debugP( "power", cbpower );
    c.train( X, COutput, idxOutput, cbpower, 0.01, 0.01);

    for( int i = 0; i < C.size(); i++ ){
        for( int j = 0; j < C[i].size(); j++ )
            if ( abs( C[i][j] - COutput[i][j] ) > 0.0001 )  cout << C[i][j] << " vs " << COutput[i][j] << "  " << i << "  " << j << endl;
        cout << endl;
    }
    for( int i = 0; i < X.size(); i++ )
        if( ( idx[i] - 1 ) != idxOutput[i] ) cout << i << endl;
    */
       /*for( int i = 0; i< 80; i++ ){
        idx[i] = 0;
        idx[ 40 + i ] = 1;
    }
     c.find_mean( X, COutput, idx, );*/
    
    /*
    //Find_mean_coach
    int dimension = 3, setSize = 9, centroidsNumber = 3;
            int chunkSize = setSize / centroidsNumber;
            vector<long long> idx( setSize, 0 );
            vector<float> v0( dimension, 0 );
            vector<vector<float>>   X( setSize ), 
                                    C( centroidsNumber ), 
                                    COut ( centroidsNumber, v0 );
            for ( int i = 0; i < centroidsNumber; i++ ){
                fill_n( idx.begin() + i * chunkSize , chunkSize , i );
                vector<float> v( dimension, i );
                fill_n( X.begin() + i * chunkSize , chunkSize , v );
                C[i] = v;
            }

            for(int i = 0; i < setSize; i++ ){
                cout<< idx[i] << endl;
            }
            c.find_mean( X, COut, idx, centroidsNumber );
    vector<vector<float>>::iterator itC( COut.begin() );
    while ( itC != COut.end() ){
        for ( int i=0; i < (*itC).size(); i++ ){
            cout << (*itC).at(i) << ' ';
        }
        cout << endl;
        itC++; 
    }*/
    /*//Find_nearest_centroid_coach
            int dimension = 3, setSize = 12, centroidsNumber = 4;
            int chunkSize = setSize / centroidsNumber;
            vector<long long> idx( setSize, 0 ), idxOut( setSize, 0 );
            vector<float> v0( dimension, 0 );
            vector<vector<float>>   X( setSize ), 
                                    C( centroidsNumber );
            for ( int i = 0; i < centroidsNumber; i++ ){
                fill_n( idx.begin() + i * chunkSize , chunkSize , i );
                vector<float> v( dimension, i );
                fill_n( X.begin() + i * chunkSize , chunkSize , v );
                C[i] = v;
            }
            X[0][0] = 0.1;
            X[3][0] = 1.2;
            X[6][0] = 2.3;
            cout << "cost: " << c.find_nearest_centroid( X, C, idxOut, 2 ) << endl;
        for ( int i = 0; i < idxOut.size() ; i++ ) {
            cout << idxOut[i] << endl;
        }
        
        */
    /*
    //move_centroids
            int dimension = 3, 
                setSize = 12, 
                initialCentroidsPower = 1, 
                initialCentroidsNumber = pow( 2, initialCentroidsPower),
                fullCentroidsNumber = initialCentroidsNumber * 2;
            vector<float> v0( dimension, 0 );
            vector<vector<float>>   C( fullCentroidsNumber, v0 ), 
                                    COut( fullCentroidsNumber, v0 );
            float eps = 0.5f;
            for( int i = 0; i < initialCentroidsNumber; i++ ){
                vector<float> v( dimension, i );
                COut[i] = v;
             }
            for( int i = 0; i < initialCentroidsNumber; i++ ){
                vector<float> v( dimension, i );
                for( int j = 0; j < dimension; j++ ){
                    C[i][j] = COut[i][j] * (1 + eps ); 
                    C[ initialCentroidsNumber + i ][j] = COut[i][j] * ( 1 - eps ); 
                }
            }
            c.split_and_move_centroids( COut, initialCentroidsPower, eps );

            for( int i = 0; i < C.size(); i++ )
                for( int j = 0; j < dimension; j++ )
                    cout << C[i][j] << "  " << COut[i][j] << endl;                  */
        
//    debugP( c.train() );
//    m1 = c.load();
    /*list<float> l;
    load_test_io( "../tests/test-ascii-s.dat", l );
    
    cout << l.size() << "  " << *l.begin() << endl;

    ifstream ifsT( "../tests/test.dat", ios_base::binary );
    if ( ifsT.good() ){
        float* f;
        f = new float[5];
        //vector<vector<float>> f( 2, vector<float>( 2, 0.0 ) );
//        unsigned char* bf = (unsigned char*)&f;
//        unsigned char* bf = reinterpret_cast<unsigned char*>(f);
//        char* bf;
 //       bf = new char[20];
//        bf = new unsigned char[16];
//        ifsT.read( (char*)bf, sizeof( f ) );
        ifsT.read( reinterpret_cast<char*>(f), 20 );
//        vector<float*> fv( f, f+2 );
//        ifsT.read( bf, 20 );
//        memcpy( f, bf, 20 );
        f++;
        f++;
        f++;
        f++;
//        vector<float> v(5);
//        memcpy( &v, f, 20 );

        cout << f[0] << "  " << f[1] << "  " << f[4] << endl;
        debugP( sizeof( f ), sizeof( f ) );
//        memcpy( &f, bf, sizeof( f ) );
//        f = static_cast<float>(*bf);
        cout  << endl;
    }

            VQIO io;
            bool result = false;
            int n = 13;
            vector<vector<float>> V;
            io.read_binary( "../tests/test.dat", V, n );
            cout << V.size() << endl;
                 for( int i = 0; i < 2 * n; i++ ){
                     cout << i << " " << V[ (int) i / n ][ i % n ] << endl;
                     cout << V.size() - i/n << " " << V[ (int)( V.size() - 2 + i/n ) ][ i % n ] << endl;
                }*/

   /*boost::dynamic_bitset<unsigned long> bitSet;
    std::cout << bitSet << endl;
    for( unsigned long i = 1; i < 15; i++ ){
        bitSet.resize( bitSet.size() + 4, 0 ); 
        std::cout << bitSet << " size " << bitSet.size() << endl;
        
    }
    //std::cout << bitSet << endl;
    std::cout<< log(  ULONG_MAX ) << endl;
    std::cout<<  ULLONG_MAX << endl;
    std::cout<<  ULONG_MAX << endl;
    std::cout<<  ( pow( 2, 32 ) - 1 ) << endl;*/
   
    //bitstream
    unsigned long bfSize = 3;
    int codeWordLength = 6, codeWordsNumber = 13;
    unsigned long *bfL, *codes, bf, bf1, *codes2;
    codes = new unsigned long[ codeWordsNumber ];
    bfL = new unsigned long[ bfSize ];
    unsigned char *bfC = reinterpret_cast<unsigned char*>( bfL );
    /*
    *bfL = ~0ul;
    *bfL = 0ul;
    for( int i = 0; i < 4; i++ ){
        cout<< i << " " << bfC[i] << endl;
    }
   */
    int bfLPos = 0, bfLPosIntra = 0, bfLLengthIntra = 32;
    for( int i = 0; i < codeWordsNumber; i++ )
        codes[i] = i;
    int newPos = 0, ci = 0;
    bfL[ci] = 0;
    for( int i = 0; i < codeWordsNumber; i++ ){
        newPos = bfLPosIntra + codeWordLength;
        bf = codes[i];
        cout << bf << " moved is " <<  (bf << bfLPosIntra ) << endl;
        bfL[ci] += ( bf << bfLPosIntra );
        if( newPos < bfLLengthIntra ){
            // *bfL += (bf << bfLPosIntra );
            bfLPosIntra += codeWordLength;
//            boost::dynamic_bitset<unsigned long> bitSet( 32 ,*bfL );
//            cout << bitSet << endl;
        }else{
//            boost::dynamic_bitset<unsigned long> bitSet( 32 ,*bfL );
//            cout << bitSet << endl;            
            bf = ( codes[i] >> ( bfLLengthIntra - bfLPosIntra )  );
            ci++;
            bfL[ci] = 0;
            bfL[ci] += bf;
            bfLPosIntra = codeWordLength - ( bfLLengthIntra - bfLPosIntra );
//            boost::dynamic_bitset<unsigned long> bitSet1( 32 ,*bfL );
//            cout << bitSet1 << endl;        
        }

    }

    for( int i = 0; i < bfSize; i++ ){
       boost::dynamic_bitset<unsigned long> bitSet1( 32 , bfL[i] );
       cout << bitSet1 << endl; 
    }

    long long fileSize = 4 * bfSize, codesInFile = fileSize * 8 / codeWordLength, filePos = 0;

    /*ofstream ofs( "../tests/bits.dat", ios_base::binary );
    ofs.write( reinterpret_cast<char *>( bfC ), fileSize );
    ofs.close();*/

    unsigned char* bfCr;
    bfCr = new unsigned char[ fileSize ];
    ifstream ifs( "../tests/bits.dat", ios_base::binary );
    ifs.read( reinterpret_cast<char *>( bfCr ), fileSize );
    ifs.close();

    unsigned char bfC1 = 0;
    unsigned long bfLF = 0;
    int filePosIntra = 0, codesPosIntra = 0, newFilePos, newCodePos, blockSize = 8; 
    codes2 = new unsigned long[ codesInFile ];
/* read_bits
    bfLF = pow( 2, 5 ) - 1;
    bfLF = bfLF << 8;
    boost::dynamic_bitset<unsigned long> bitSet( 32 , bfLF );
    cout << bitSet << endl; 

    bfLF = read_bits( bfLF, 10, 4 );
    boost::dynamic_bitset<unsigned long> bitSet1( 32 , bfLF );
    cout << bfLF << " bin " << bitSet1 << endl; 
*/
    for( long long i = 0; i < codesInFile; i++ ){
        codes2[i] = 0;
        codesPosIntra = 0;
        while( codesPosIntra < codeWordLength ){
            bfLF = bfCr[ filePos ];
            int bitsToReadFromCurrentByte = min( 
                                        codeWordLength - codesPosIntra, 
                                        blockSize - filePosIntra
                                        );
            bfLF = read_bits( bfLF, filePosIntra, bitsToReadFromCurrentByte );
            codes2[i] += ( bfLF ) << codesPosIntra;
            codesPosIntra += bitsToReadFromCurrentByte;
            filePosIntra += bitsToReadFromCurrentByte;
            if ( filePosIntra >= blockSize ) {
                filePos++;
                filePosIntra = 0;
            }
        }
    }
    
    for( int i = 0; i < codesInFile; i++ ){
       boost::dynamic_bitset<unsigned long> bitSet1( 32 , codes2[i] );
       cout << codes2[i] << " bin " << bitSet1 << endl; 
    }

    delete[] codes;
    delete[] codes2;
    delete[] bfL;
    delete[] bfCr;

//    std::cout<< bfL << endl;
    int codeSize = 6;//,  codeWordsNumber = 13;            
    //in this file we've encoded numbers from 0 to 12 with 6 bit code
    bit_stream bs( "../tests/bits.dat", codeSize );
    cout << "bf size " << bs.get_buffer_size() << endl;
    while( bs.good() ){
        cout << bs.read_single() << endl;
    }
    int i = 0, i1 = i;
    /*for( int i = 0; i < codeWordsNumber; i++ )
        Assert::AreEqual<int>( i, bs.read_single() );
    for( int i = codeWordsNumber; i < 16; i++ )
        Assert::AreEqual<int>( 0, bs.read_single() );
    Assert::AreEqual<int>( -1, bs.read_single() );*/

    if ( argc > 1 ) {

    }
    system( "pause " );
    
    return 0; 
}