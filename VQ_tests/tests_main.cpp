#include "stdafx.h"
#include "CppUnitTest.h"
#include "VQ.h"

using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

const float Toll = 0.00001f;

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

namespace VQ_tests
{		
	TEST_CLASS(UnitTest1)
	{
        vector<vector<float>> _X, _C, _COutput;
        vector<long long> _idx, _idxOutput;
        coach _c;
        int _cbpower;

	public:
		
		TEST_METHOD(CodeBookContent_train_coach)
		{
			if ( _X.size() == 0 )
                _cbpower = load_test_data( "../tests/test2dm.dat", _X, _C, _idx );
            _c.train( _X, _COutput, _idx, _cbpower, 0.01f, 0.01f );

            Assert::AreEqual<int>( 
                _C.size(), 
                _COutput.size() 
                ); 	

            for( int i = 0; i < _C.size(); i++ )
                for( int j = 0; j < _C[0].size(); j++ )
                    Assert::AreEqual( 
                        _C[i][j], 
                        _COutput[i][j],
                        Toll
                   );                        
		}

		TEST_METHOD(CodeBookSize_train_coach)
		{
			if ( _X.size() == 0 ) 
                _cbpower = load_test_data( "../tests/test2dm.dat", _X, _C, _idx );
            _c.train( _X, _COutput, _idx, _cbpower, 0.01f, 0.01f );
            Assert::AreEqual<int>( 
                _C.size(), 
                _COutput.size() 
                ); 	
		}

		TEST_METHOD(Find_mean_coach)
		{
            int dimension = 3, setSize = 9, centroidsNumber = 3;
            int chunkSize = setSize / centroidsNumber;
            vector<long long> idx( setSize, 0 );
            vector<float> v0( dimension, 0 );
            vector<vector<float>>   X( setSize ), 
                                    C( centroidsNumber ), 
                                    COut ( centroidsNumber, v0 );
            for( int i = 0; i < centroidsNumber; i++ ){
                fill_n( idx.begin() + i * chunkSize , chunkSize , i );
                vector<float> v( dimension, i );
                fill_n( X.begin() + i * chunkSize , chunkSize , v );
                C[i] = v;
            }
            _c.find_mean( X, COut, idx, centroidsNumber );
            Assert::AreEqual<int>( 
                C.size(), 
                COut.size() 
                ); 
            for( int i = 0; i < centroidsNumber; i++ ){
                Assert::AreEqual<bool>( 
                    true, 
                    equal( 
                        C[i].begin(), 
                        C[i].end(), 
                        COut[i].begin() 
                        )
                    );  
            }
		}

        TEST_METHOD(Find_nearest_centroid_coach)
		{

            //lets create test data
            int dimension = 3, setSize = 12, centroidsNumber = 4;
            int chunkSize = setSize / centroidsNumber;
            vector<long long> idx( setSize, 0 ), idxOut( setSize, 0 );
            vector<float> v0( dimension, 0 );
            vector<vector<float>>   X( setSize ), 
                                    C( centroidsNumber );
            for( int i = 0; i < centroidsNumber; i++ ){
                fill_n( idx.begin() + i * chunkSize , chunkSize , i );
                vector<float> v( dimension, i );
                fill_n( X.begin() + i * chunkSize , chunkSize , v );
                C[i] = v;
            }
            _c.find_nearest_centroid( X, C, idxOut, 2 );
            Assert::AreEqual<int>( 
                idx.size(), 
                idxOut.size() 
                ); 
            Assert::AreEqual<bool>( 
                true, 
                equal( 
                        idx.begin(), 
                        idx.end(), 
                        idxOut.begin() 
                     )
            );  
		}

        TEST_METHOD(Cost_find_nearest_centroid_coach)
		{

            //lets create test data
            int dimension = 3, setSize = 12, centroidsNumber = 4;
            int chunkSize = setSize / centroidsNumber;
            vector<long long> idx( setSize, 0 ), idxOut( setSize, 0 );
            vector<float> v0( dimension, 0 );
            vector<vector<float>>   X( setSize ), 
                                    C( centroidsNumber );
            for( int i = 0; i < centroidsNumber; i++ ){
                fill_n( idx.begin() + i * chunkSize , chunkSize , i );
                vector<float> v( dimension, i );
                fill_n( X.begin() + i * chunkSize , chunkSize , v );
                C[i] = v;
            }
            X[0][0] = 0.1;
            X[3][0] = 1.2;
            X[6][0] = 2.3;
            
            float expectedV =  0.1*0.1 + 0.2*0.2 + 0.3*0.3;

            Assert::AreEqual( 
               expectedV, 
                _c.find_nearest_centroid( X, C, idxOut, 2 ),
                Toll
            ); 
 
		}

        TEST_METHOD(Split_and_move_centroids_coach)
		{
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
            _c.split_and_move_centroids( COut, initialCentroidsPower, eps );
            for( int i = 0; i < C.size(); i++ )
                for( int j = 0; j < dimension; j++ )
                    Assert::AreEqual( 
                        C[i][j], 
                        COut[i][j],
                        Toll
                   );
		}

        TEST_METHOD(Compute_cost_coach)
		{

            //lets create test data
            int dimension = 3, setSize = 12, centroidsNumber = 4;
            int chunkSize = setSize / centroidsNumber;
            vector<long long> idx( setSize, 0 );
            vector<float> v0( dimension, 0 );
            vector<vector<float>>   X( setSize ), 
                                    C( centroidsNumber );
            for( int i = 0; i < centroidsNumber; i++ ){
                fill_n( idx.begin() + i * chunkSize , chunkSize , i );
                vector<float> v( dimension, i );
                fill_n( X.begin() + i * chunkSize , chunkSize , v );
                C[i] = v;
            }
            X[0][0] = 0.1;
            X[3][0] = 1.2;
            X[6][0] = 2.3;
            
            float expectedV =  0.1*0.1 + 0.2*0.2 + 0.3*0.3;

            Assert::AreEqual( 
               expectedV, 
                _c.compute_cost( X, C, idx ),
                Toll
            ); 
 
		}

        TEST_METHOD(Coord_squared_VQm)
		{
            float v1=0.1, v2=0.2;
            Assert::AreEqual( 
                0.01f, 
                VQm::coord_squared( v1, v2 ),
                Toll
            ); 
		}
        
        TEST_METHOD(L2distance_VQm)
		{
            vector<float> v1( 3, 0.1 ), v2( 3, 0.2 );
            Assert::AreEqual( 
                0.03f,
                VQm::L2distance( v1, v2 ),
                Toll
            ); 

		}
        
        TEST_METHOD(Vector_by_scalar_inplace_VQm)
		{
            vector<float> v1( 3, 3.0f ), v2( 3, 1.5f );
            VQm::vector_by_scalar_inplace( v1, 0.5f );
            for( int i = 0; i < 3; i++ )
                Assert::AreEqual( 
                        v1[i], 
                        v2[i],
                        Toll
                );            

		}        

	};
}