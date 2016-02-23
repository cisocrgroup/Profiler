#include <Global.h>
#include <FBDicString/FBDicString.h>
#include <Getopt/Getopt.h>

using namespace csl;
int main( int argc, const char** argv ) {

    std::locale::global( std::locale( "" ) );


    try {
	Getopt options( argc, argv );

	if( options.getArgumentCount() != 2 ) {
	    std::cerr<<"Use like: "<<argv[0]<<" <txtDic> <binDic>"<<std::endl;
	    return 1;
	}

	FBDicString fbds;

	if( options.hasOption( "cislex" ) ) {
	    fbds.setKeyValueDelimiter( '.' );
	}

	fbds.compileDic( options.getArgument( 0 ).c_str() );
	fbds.writeToFile( options.getArgument( 1 ).c_str() );


    }
    catch( csl::exceptions::cslException exc ) {
            std::wstring wide_what(OCRCorrection::Utils::utf8(exc.what()));

	std::wcerr << "csl::compileFBDString: caught cslException and aborted: " << wide_what << std::endl;
	return 1;
    }
    catch( std::exception exc ) {
	std::wcerr << "csl::compileFBDString: caught std::exception. Aborted." << std::endl;
	return 1;
    }

    return 0;
}
