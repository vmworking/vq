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

int main(int argc, char* argv[])
{
    coach c;
   
    vector<vector<float>> m1;
    vector<vector<float>> X, C, COutput;
    vector<long long> idx;
    int cbpower = load_test_data( "../tests/test2dm.dat", X, C, idx );
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
    c.train( X, COutput, idx, cbpower, 0.01f, 0.01f);

    for( int i = 0; i < C.size(); i++ ){
        for( int j = 0; j < C[i].size(); j++ )
            cout << C[i][j] << " vs " << COutput[i][j] << endl;
        cout << endl;
    }
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
    if ( argc > 1 ) {

    }
    system( "pause " );
    
    return 0; 
}