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

void upset( string message )
{
    cout << message << endl;
    system( "pause " );
    exit(1);
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
        vector<vector<float>> _X, _C, _COutput;
        vector<long long> _idx, _idxOutput;
        coach _c;
        int _cbpower;

			if ( _X.size() == 0 )
                _cbpower = load_test_data( "../tests/test_matlab_5_f.dat", _X, _C, _idx );
            _c.train( _X, _COutput, _idx, _cbpower, 0.01, 0.01 );

            cout << "size C---  " << _C.size() << _COutput.size() << endl; 	
            system( "pause " );
            for( int i = 0; i < _C.size(); i++ )
                for( int j = 0; j < _C[0].size(); j++ )
                    if( abs( _C[i][j] - _COutput[i][j] ) > 0.001 )
                        cout << "size C---  " << i << " " << j << " " << _C[i][j] << " vs " << _COutput[i][j] << endl; 	

            system( "pause " );
    
    return 1;
    
    
    int c;
    VQ model;
    string 
        action = "",
        fileSource = "",
        fileCodebook = "",
        fileEncoded = "",
        fileDecoded = "";
    int 
        dimensionality = 0, 
        codeSizePower = 0;

    //parsing command line
    while (--argc > 0 ){
        if ( (*++argv)[0] == '-' ){
            c = *++argv[0];
            switch (c) {
            case 'a':
                if (argc > 1) {
                    action =  string (*(argv+1) );
                    *(argv+1) = "";
                }
                break;
            case 'd':
                if (argc > 1) {
                    dimensionality = atoi( *(argv+1) );
                    *(argv+1) = "";
                }
                break;
            case 'n':
                if (argc > 1) {
                    codeSizePower = atoi( *(argv+1) );
                    *(argv+1) = "";
                }
                break;
            case 's':
                if (argc > 1) {
                    fileSource =  string (*(argv+1) );
                    *(argv+1) = "";
                }
                break;
            case 'c':
                if (argc > 1) {
                    fileCodebook =  string (*(argv+1) );
                    *(argv+1) = "";
                }
                break;
            case 'e':
                if (argc > 1) {
                    fileEncoded =  string (*(argv+1) );
                    *(argv+1) = "";
                }
                break;
            case 'o':
                if (argc > 1) {
                    fileDecoded =  string (*(argv+1) );
                    *(argv+1) = "";
                }
                break;

            }
            *(argv) = "";
        }
    }

    if( action == "" ) {

    }

    VQconf conf;
    conf.dimensionality = dimensionality;
    conf.codeSizePower = codeSizePower;
    conf.step = conf.distortion = 0.01;
    model.configure( conf );
    if( !model.good() ) 
        upset( "Wrong configuration parameters" );
    
    //create codebook
    if( action == "c" ){
        if( !model.load_source( fileSource ) )
            upset( "Cannot load source data" );
        model.train();
        if( !model.save_code_book( fileCodebook ) )
            upset( "Cannot save codebook" );
    }

    system( "pause " );
    return 0; 
} 