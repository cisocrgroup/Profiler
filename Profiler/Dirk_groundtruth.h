#ifndef OCRCORRECTION_DIRK_H__
#define OCRCORRECTION_DIRK_H__

#include<string>
#include<sstream>
#include<TransTable/TransTable.h>
#include<DictSearch/DictSearch.h>
#include<Pattern/ComputeInstruction.h>
#include<Document/Document.h>
#include<Exceptions.h>
#include <time.h>

#include "CompoundDictModule.h"


/**
 *
 */
class Dirk {
public:
    enum Interactive {CLICK, GUESS_ALL, GUESS_UNAMBIGUOUS};

	Dirk():
	compoundModule_( 0 ),
	interactive_( CLICK )
	{

	}

    ~Dirk() {
	if( compoundModule_ ) delete( compoundModule_ );
    }

    void setInteractive( Interactive i ) {
	interactive_ = i;
    }

    csl::DictSearch& getDictSearch() {
	return dictSearch_;
    }

    void processText( std::string const& source, std::wostream& outStream ) {
	OCRCorrection::Document document;

	document.parseAlignedTXT( source.c_str() );


	csl::DictSearch::CandidateSet cands;

	csl::PatternWeights patternProbabilities;
	patternProbabilities.setDefault( csl::PatternWeights::PatternType( 1, 1 ), 1e-5 );
	patternProbabilities.setDefault( csl::PatternWeights::PatternType( 1, 0 ), 1e-5 );
	patternProbabilities.setDefault( csl::PatternWeights::PatternType( 0, 1 ), 1e-5 );
	patternProbabilities.setDefault( csl::PatternWeights::PatternType( 1, 2 ), 1e-5 );
	patternProbabilities.setDefault( csl::PatternWeights::PatternType( 2, 1 ), 1e-5 );
	patternProbabilities.setSmartMerge( true );



	csl::ComputeInstruction instructionComputer;
	instructionComputer.connectPatternProbabilities( patternProbabilities );
	instructionComputer.setMaxNumberOfInstructions( 10 );
	std::vector< csl::Instruction > ocrInstructions; // is filled later in the document loop



	std::wostringstream xml_out;
	xml_out.imbue( csl::CSLLocale::Instance() );


	// set up counter
	std::map< std::wstring, int > counter;
	std::vector< std::wstring > counter_keys;
	counter_keys.push_back( L"tokens" );
	counter_keys.push_back( L"kurz" );
	counter_keys.push_back( L"unbekannt" );
	counter_keys.push_back( L"modern" );
	counter_keys.push_back( L"hypothetisch" );
	counter_keys.push_back( L"historisch" );
	counter_keys.push_back( L"modern, mit Fehlern" );
	counter_keys.push_back( L"hypothetisch, mit Fehlern" );
	counter_keys.push_back( L"historisch, mit Fehlern" );
	counter_keys.push_back( L"Kompositum" );
	counter_keys.push_back( L"Kompositum, mit Fehlern" );
	counter_keys.push_back( L"hypothetisches Kompositum" );
	counter_keys.push_back( L"hypothetisches Kompositum, mit Fehlern" );
	counter_keys.push_back( L"Latein" );
	counter_keys.push_back( L"Latein, mit Fehlern" );

	for( std::vector< std::wstring >::const_iterator key = counter_keys.begin(); key != counter_keys.end(); ++key ) {
	    counter[*key] = 0;
	}

	if( modernDictFile_ == "NONE" ) {
	    counter[L"modern"] = counter[L"modern, mit Fehlern"] = -1;
	}
	if( histDictFile_ == "NONE" ) {
	    counter[L"historisch"] = counter[L"historisch, mit Fehlern"] = -1;
	}


	for( OCRCorrection::Document::iterator token = document.begin(); // for all tokens
	     token != document.end();
	     ++token ) {


	    std::wstring wOCR = token->getWOCR();
	    std::wstring wOCR_lc = wOCR;
	    for( std::wstring::iterator c = wOCR_lc.begin(); c != wOCR_lc.end(); ++c ) {
		*c = std::tolower( *c );
	    }

	    std::wstring wOrig = token->getGroundtruth().getWOrig();
	    std::wstring wOrig_lc = wOrig;
	    for( std::wstring::iterator c = wOrig_lc.begin(); c != wOrig_lc.end(); ++c ) {
		*c = std::tolower( *c );
	    }

	    //std::wcout << "Worig=" << wOrig <<", wOCR=" << wOCR <<  std::endl;

	    // OCRCorrection::Document does not distinguish between short words and non-words. They are all "not normal"
	    bool is_a_word =
		//token->isNormal() ||
		( wOrig_lc.find_first_of( L"abcdefghijklmnopqrstuvwxyzäöüß" ) != std::wstring::npos );

	    ++counter[L"tokens"];


	    cands.reset();
	    if( is_a_word ) dictSearch_.query( wOrig_lc, &cands );

	    // here, a custom-made sort-operator can be passed as 3rd argument
	    std::sort( cands.begin(), cands.end() );

	    std::wstring classified = L"null";


	    if( wOrig.empty() ) {
		classified = L"split_tail";
	    }

	    if( wOCR == L" " ) {
		xml_out
		    << "<token token_id=\"" << counter[L"tokens"] << "\" special_seq=\"space\" isNormal=\"false\" verified=\"false\">" << std::endl
		    <<  "<classified>" << xml_escape( classified ) << "</classified>" << std::endl
		    <<  "<wOCR>" << wOCR << "</wOCR>" << std::endl
		    <<  "<wOCR_lc>" << wOCR_lc << "</wOCR_lc>" << std::endl
		    <<  "<wOrig>" << xml_escape( wOrig ) << "</wOrig>" << std::endl
		    <<  "<wOrig_lc>" << xml_escape( wOrig_lc ) << "</wOrig_lc>" << std::endl;
	    }
	    else if( wOCR == L"\n" ) {
		xml_out
		    << "<token token_id=\"" << counter[L"tokens"] << "\" special_seq=\"newline\" isNormal=\"false\" verified=\"false\">" << std::endl
		    <<  "<classified>" << xml_escape( classified ) << "</classified>" << std::endl
		    <<  "<wOCR>" << wOCR << "</wOCR>" << std::endl
		    <<  "<wOCR_lc>" << wOCR_lc << "</wOCR_lc>" << std::endl
		    <<  "<wOrig>" << xml_escape( wOrig ) << "</wOrig>" << std::endl
		    <<  "<wOrig_lc>" << xml_escape( wOrig_lc ) << "</wOrig_lc>" << std::endl;
	    }
	    else {
		std::wstring is_a_word_string = ( is_a_word )? L"true" : L"false";
		if( classified != L"null" ) {
		    // was set before; do not over-rule
		}
		else if( cands.size() == 0 ) {
		    classified = L"unknown";
		}
		else if( cands.at( 0 ).getInstruction().size() == 0 ) {
		    classified = L"modern";
		}
		else {
		    classified = L"hypothetic";
		}


		ocrInstructions.clear();

		try {
		    instructionComputer.computeInstruction( wOrig_lc, wOCR_lc, &ocrInstructions );
		} catch( std::exception exc ) {
		    std::wcerr << "Dirk_groundtruth: caught exception in csl::...::computeInstruction. Will throw it again..." << std::endl;
		    throw exc;
		}

		//if( ocrInstructions.size() > 10 ) ocrInstructions.resize( 10 ); // is replaced by apropriate method in ComputeInstruction

		enum GuessOCR { NONE, FIRST, DESTROYED };
		GuessOCR guessOCR = NONE;

		// this case differentiation assumes that interactive_ is one of CLICK, GUESS_ALL, ( GUESS_UNAMBIGUOUS )
		if( interactive_ == GUESS_UNAMBIGUOUS ) {
		    throw OCRCorrection::OCRCException( "OCRC::Dirk_groundtruth: option 'UNAMBIGUOUS' is not yet implemented" );
		}
		else if( interactive_ == GUESS_ALL ) {
		    // if instructions include 4 or more patterns (check is for 1st instruction, but they should all have equal length)
		    if( ! ocrInstructions.empty() && ocrInstructions.at(0).size() >= 4 ) {
			guessOCR = DESTROYED;
		    }
		    else {
			guessOCR = FIRST;
		    }
		}
		else {
		    guessOCR = NONE;
		}



		std::wstring verified = ( guessOCR != NONE )? L"true" : L"false";

		xml_out
		    << "<token token_id=\"" << counter[L"tokens"] << "\" isNormal=\""<<is_a_word_string <<"\" verified=\"" << verified << "\">" << std::endl
		    <<  "<classified>" << xml_escape( classified ) << "</classified>" << std::endl
		    <<  "<wOCR>" << xml_escape( wOCR ) << "</wOCR>" << std::endl
		    <<  "<wOCR_lc>" << xml_escape( wOCR_lc ) << "</wOCR_lc>" << std::endl
		    <<  "<wOrig>" << xml_escape( wOrig ) << "</wOrig>" << std::endl
		    <<  "<wOrig_lc>" << xml_escape( wOrig_lc ) << "</wOrig_lc>" << std::endl
		    //                                                        /--> the *DESTROYED* and the "MERGE_SPLIT" option
		    <<  "<ocrInstructions count=\"" << ocrInstructions.size() + 2 << "\">" << std::endl;

		size_t instrCount = 0;
		std::wstring selected = L"false"; // this is just a "safety-first" default setting

		selected = ( guessOCR == DESTROYED ) ? L"true" : L"false";
		xml_out << "<instruction instr_id=\"" << counter[L"tokens"] << "_" << instrCount << "\" selected=\""<< selected << "\">" << xml_escape( L"*DESTROYED*" ) << "</instruction>" << std::endl;
		++instrCount;


		xml_out << "<instruction instr_id=\"" << counter[L"tokens"] << "_" << instrCount << "\" selected=\"false\">" << xml_escape( L"*MERGE_SPLIT*" ) << "</instruction>" << std::endl;
		++instrCount;

		selected = L"false";

		for( std::vector< csl::Instruction >::const_iterator instr = ocrInstructions.begin();
		     instr != ocrInstructions.end();
		     ++instr, ++instrCount ) {

		    if( guessOCR == FIRST ) { // always select the first "real" instruction after merge/split etc.
			selected  = ( instrCount == 2 )? L"true" : L"false";
		    }
		    else {
			if( ( ocrInstructions.size() == 1 ) && ( instr->size() < 3 ) ) selected = L"true";
			else selected = L"false";
		    }

		    xml_out << "<instruction instr_id=\"" << counter[L"tokens"] << "_" << instrCount << "\" selected=\"" << selected << "\">" << xml_escape( instr->toString() ) << "</instruction>" << std::endl;
		}

		xml_out <<  "</ocrInstructions>" << std::endl;
	    }

	    xml_out << "<cands count=\"" << cands.size() << "\">" << std::endl;

	    size_t candCount = 0;
	    std::wstring selected = L"false";
	    for( csl::DictSearch::CandidateSet::const_iterator cand = cands.begin(); cand != cands.end(); ++cand, ++candCount ) {
		if( ( interactive_ == GUESS_ALL ) || ( interactive_ == GUESS_UNAMBIGUOUS && cands.size() == 0 ) ) {
		    selected = ( candCount == 0 )? L"true" : L"false";
		}
		xml_out << "<cand cand_id=\""<< counter[L"tokens"] << "_" << candCount <<"\" selected=\"" << selected << "\">" << *cand << "</cand>" <<std::endl;
	    }
	    xml_out << "</cands>" << std::endl
		    << "</token>" << std::endl;

	} // for each token

	////////////////////////////////////////////////
	//////// XML OUTPUT ///////////////////////////
	////////////////////////////////////////////////

	time_t t = time(NULL);

	outStream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl
		  << "<document>" << std::endl
		  << "<head>" << std::endl
		  <<"<created>"<< asctime(localtime(&t)) <<"</created>"<<std::endl
		  <<"<changed>0</changed>"<<std::endl
		  <<"<finished>false</finished>"<<std::endl
		  <<"</head>" << std::endl;

	outStream << xml_out.str();
	outStream << "</document>"<< std::endl;

    }

    std::wstring xml_escape( std::wstring const& input ) const {
	std::wstring str = input;
	size_t pos = 0;
	while( ( pos = str.find( '&', pos ) ) != std::wstring::npos ) {
	    str.replace( pos, 1, L"&amp;" );
	    ++pos;
	}
	pos = 0;
	while( ( pos = str.find( '>', pos ) ) != std::wstring::npos ) {
	    str.replace( pos, 1, L"&gt;" );
	    ++pos;
	}
	pos = 0;
	while( ( pos = str.find( '<', pos ) ) != std::wstring::npos ) {
    str.replace( pos, 1, L"&lt;" );
	    ++pos;
	}
	return str;
    }

private:
    std::string modernDictFile_;
    std::string histDictFile_;
    std::string patternFile_;
    std::string compoundFile_;

    csl::DictSearch dictSearch_;
    CompoundDictModule* compoundModule_;

    int verbose_;
    Interactive interactive_;
}; // class Dirk

#endif /* OCRCORRECTION_DIRK_H__ */
