#include<iostream>

#include <cstdlib>
#include <stdexcept>
#include <sys/types.h>
#include <dirent.h>

#include "./Dirk.h"
#include<csl/Getopt/Getopt.h>

int main( int argc, char const** argv ) {

    try {

    std::locale::global( std::locale( "" ) );

    Getopt options( argc, argv );
    
    if( options.getArgumentCount() != 0 ) {
	std::wcerr << "Use like: ocr_analysis>"<< std::endl
		   << "[--modernDict=<dictFile>]" << std::endl
		   << "[--patternFile=<patternFile>]" << std::endl
		   << "[--histDict=<dictFile>]" << std::endl
		   << "[--compoundDict=<compoundDictFile>]" << std::endl
		   << "[--latinDict=<dictFile>]" << std::endl
		   << "[--geoDict=<dictFile>]" << std::endl
		   << std::endl
		   << "[--sourceFile=<txtFile>] OR" << std::endl
		   << "[--sourceDir=<dir1> --targetDir=<dir2>]" << std::endl
		   << std::endl
		   << "For all dictionaries and the pattern file, the string \"DEFAULT\" will invoke the use of standard dicts."
	    ;
	exit( 1 );
    }
    
    std::string modernDictFile, patternFile, histDictFile, compoundDictFile, latinDictFile, geoDictFile;

    std::string modernDictFile_def = "/mounts/data/proj/impact/software/Vaam/cislexAll_plus_dCorrect_kompAbschl.fbdic";
    std::string histDictFile_def = "/mounts/Users/student/uli/implement/cxx/dirk/historic.fbdic";
    std::string compoundDictFile_def = "/mounts/data/proj/impact/software/Vaam/compounds.tt";
    std::string geoDictFile_def = "/mounts/data/proj/impact/lexika/geo/ortsnamen.fbdic";
    std::string latinDictFile_def = "/mounts/data/proj/impact/latein/morph/LatinForms.fbdic";
    std::string patternFile_def = "/mounts/data/proj/impact/software/Vaam/patterns.lexgui.txt";


    Dirk dirk;

    CompoundDictModule* compoundModule = 0;

    if( options.hasOption( "patternFile" ) ) {
	std::string const& patternFile = ( options.getOption( "patternFile" ) == "DEFAULT" )? 
	    patternFile_def : options.getOption( "histDict" );
	dirk.getDictSearch().initHypothetic( patternFile.c_str() );
    }
    if( options.hasOption( "modernDict" ) ) {
	std::string const& dictFile = ( options.getOption( "modernDict" ) == "DEFAULT" )? 
	    modernDictFile_def : options.getOption( "modernDict" );
	csl::DictSearch::DictModule& modernMod = dirk.getDictSearch().addDictModule( L"modern", dictFile );
	modernMod.setDLevWordlengths();
	modernMod.setPriority( 100 );
	if( options.hasOption( "patternFile" ) ) {
	    modernMod.setMaxNrOfPatterns( 1000 );
	    modernMod.setDLevHypothetic( 1 );
	}
    }
    if( options.hasOption( "geoDict" ) ) {
	std::string const& dictFile = ( options.getOption( "geoDict" ) == "DEFAULT" )? 
	    geoDictFile_def : options.getOption( "geoDict" );
	csl::DictSearch::DictModule& geoMod = dirk.getDictSearch().addDictModule( L"geo", dictFile );
	geoMod.setPriority( 90 );
	geoMod.setDLevWordlengths();
	if( options.hasOption( "patternFile" ) ) {
	    geoMod.setMaxNrOfPatterns( 1000 );
	}
    }
    if( options.hasOption( "histDict" ) ) {
	std::string const& dictFile = ( options.getOption( "histDict" ) == "DEFAULT" )? 
	    histDictFile_def : options.getOption( "histDict" );
	csl::DictSearch::DictModule& histMod = dirk.getDictSearch().addDictModule( L"hist", dictFile );
	histMod.setPriority( 80 );
	histMod.setDLevWordlengths();
    }
    if( options.hasOption( "latinDict" ) ) {
	std::string const& dictFile = ( options.getOption( "latinDict" ) == "DEFAULT" )? 
	    latinDictFile_def : options.getOption( "latinDict" );
	csl::DictSearch::DictModule& latinMod = dirk.getDictSearch().addDictModule( L"latin", dictFile );
	latinMod.setPriority( 70 );
    }
    if( options.hasOption( "compoundDict" ) ) {
	std::string const& dictFile = ( options.getOption( "compoundDict" ) == "DEFAULT" )? 
	    compoundDictFile_def : options.getOption( "compoundDict" );
	compoundModule = new CompoundDictModule( dictFile );
	compoundModule->setPriority( 10 );
	dirk.getDictSearch().addExternalDictModule( *compoundModule );
    }

   
    if( options.hasOption( "sourceFile" ) && options.hasOption( "sourceDir" ) ) {
	throw std::runtime_error( "Pass either 'sourceFile' or 'sourceDir' as option." );
    }
    else if( options.hasOption( "sourceFile" ) ) {
	dirk.processText( options.getOption( "sourceFile" ), std::wcout );
    }

    else if( options.hasOption( "sourceDir" ) ) {
	if( ! options.hasOption( "targetDir" ) ) {
	    throw std::runtime_error( "With option 'sourceDir' always add a 'targetDir' as well." );
	}
	struct dirent *pDirEnt;

	/* Open the current directory */
	std::string sourceDir = options.getOption( "sourceDir" );
	if( sourceDir.at( 0 ) == '~' ) {
	    sourceDir.replace( 0, 1, getenv( "HOME" ) );
	}

	std::string targetDir = options.getOption( "targetDir" );
	if( targetDir.at( 0 ) == '~' ) {
	    targetDir.replace( 0, 1, getenv( "HOME" ) );
	}

	DIR *pDIR = opendir( sourceDir.c_str() );

	if ( pDIR == NULL ) {
	    throw std::runtime_error( "opendir failed" );
	}

	/* Get each directory entry from pDIR and print its name */
	
	pDirEnt = readdir( pDIR );
	while ( pDirEnt != NULL ) {
	    std::string entry( pDirEnt->d_name );
	    size_t pos = entry.rfind( ".txt" );
	    if( ( pos != std::string::npos ) && ( pos == (entry.size() - 4 ) )  ) {
		std::string inFile = sourceDir + std::string( "/" ) +  entry;
		wprintf( L"%s\n", inFile.c_str() );
		
		std::string outFile = targetDir + std::string( "/" ) +  entry + ".html" ;

		std::wofstream outStream( outFile.c_str() );
		if( ! outStream.good() ) throw std::runtime_error( "ocr_analysis: Could not open outFile" );
		
		dirk.processText( inFile, outStream );
		

	    }

	    pDirEnt = readdir( pDIR );
	}
	
	/* Release the open directory */
	
	closedir( pDIR );
    }

    if( compoundModule ) delete( compoundModule );

    } catch( std::exception& exc ) {
	wprintf( L"ocr_analysis: caught exception: %s\n", exc.what() );
	throw exc;
    }
}

