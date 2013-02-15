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


int main(int argc, char* argv[])
{

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
    if( ! model.good() ) 
        upset( "Wrong configuration parameters" );
    
    //load raw data
    if( 
        action == "c"
        || action == "e"
        || action == "ce"
        ){
        if( ! model.load_source( fileSource ) )
            upset( "Cannot load source data" );
    }
    //load codebook
    if( 
        action == "e"
        || action == "o"
        ){
        if( ! model.load_code_book( fileCodebook ) )
            upset( "Cannot load codebook" );
    }
    //create codebook
    if( 
        action == "c" 
        || action == "ce" 
        ){
        if( !( model.train() && model.save_code_book( fileCodebook ) ) )
            upset( "Cannot save codebook" );
    }
    //encode raw data 
    if( 
        action == "e" 
        || action == "ce" 
        ){
        if( !( model.encode() && model.save_encoded( fileEncoded ) ) )
            upset( "Cannot encode or save the result" );
    }
    //decode data by a given codebook
    if( action == "o" ){
        if( ! ( 
            model.load_encoded( fileEncoded ) 
            && model.decode() 
            && model.save_decoded( fileDecoded ) 
            ))
            upset( "Cannot decode or save the result" );
    }
    system( "pause " );
    return 0; 
} 