#include "stdafx.h"
#include "VQ.h"
using namespace std;

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

VQ::VQ(void)
{
}


VQ::~VQ(void)
{
}
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
        while( moveCentroids ){
            find_nearest_centroid( X, C, idx, n + 1 );
            find_mean( X, C, idx, pow( 2, n + 1 ) );
            cost = compute_cost( X, C, idx );
            moveCentroids = ( moveCentroids < 0 ) || ( ( costPrev - cost ) / costPrev > distortionToll );
            costPrev = cost;
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
    while( itX != X.end() ){
        i = argMinDist = 0;
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

