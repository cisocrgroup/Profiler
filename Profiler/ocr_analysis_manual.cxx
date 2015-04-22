#include<iostream>
#include<string>
#include<sstream>
#include<csl/TransTable/TransTable.h>
#include<csl/DictSearch/DictSearch.h>
#include<Document/Document.h>
#include<Document/Document.h>
#include <time.h>

class Dirk {
public:
    Dirk( std::string const& modernDicFile,
	  std::string const& histDicFile,
	  std::string const& patternFile,
	  int verbose = 0 ):
	modernDicFile_( modernDicFile ),
	histDicFile_( histDicFile ),
	patternFile_( patternFile ),
	compoundFile_( "NONE" ),
	verbose_( verbose ) {
	
    }

    void processText( std::string const& source, std::wostream& output ) {
	OCRCorrection::Document document;
    
	document.parseTXT( source.c_str() );
    
    
	if( modernDicFile_ != "NONE" ) {
	    dictSearch_.getConfigModern().setDict( modernDicFile_.c_str() );
	    dictSearch_.getConfigModern().setDLevWordlengths();
	}
	if( histDicFile_ != "NONE" ) {
	    dictSearch_.getConfigHistoric().setDict( histDicFile_.c_str() );
	    dictSearch_.getConfigHistoric().setDLevWordlengths();
	}
	if( patternFile_ != "NONE" ) {
	    dictSearch_.initHypothetic( patternFile_.c_str() );
	    dictSearch_.getConfigHypothetic().setDLev( 1 );
	    dictSearch_.getConfigHypothetic().setMaxNrOfPatterns( 3 );
	}
    
	////// COMPOUND INIT /////////////////
	typedef csl::TransTable< csl::TT_PERFHASH, uint16_t, uint32_t > TransTable_t;
	TransTable_t compoundDic;
	csl::Vaam< TransTable_t >* compoundVaam = 0;
	if( compoundFile_ != "NONE" ) {
	    compoundDic.loadFromFile( "/mounts/data/proj/impact/software/Vaam/compounds.tt" );
	
	    csl::Vaam< TransTable_t >* compoundVaam = new csl::Vaam< TransTable_t >( compoundDic, "/mounts/data/proj/impact/software/Vaam/patterns.lexgui.txt" );
	    compoundVaam->setMaxNrOfPatterns( 3 );
	    compoundVaam->setDistance( 0 );
	}
    
	csl::DictSearch::CandidateSet cands;
    
	std::wostringstream html_out;
    
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

	for( std::vector< std::wstring >::const_iterator key = counter_keys.begin(); key != counter_keys.end(); ++key ) {
	    counter[*key] = 0;
	}

	if( modernDicFile_ == "NONE" ) {
	    counter[L"modern"] = counter[L"modern, mit Fehlern"] = -1;
	}
	if( histDicFile_ == "NONE" ) {
	    counter[L"historisch"] = counter[L"historisch, mit Fehlern"] = -1;
	}

    

	for( OCRCorrection::Document::iterator token = document.begin(); // for all tokens
	     token != document.end();
	     ++token ) {

	
	    std::wstring lowercased = token->getWOCR();
	    for( std::wstring::iterator c = lowercased.begin(); c != lowercased.end(); ++c ) {
		*c = std::tolower( *c );
	    }
	
	    // OCRCorrection::Document does not distinguish between short words and non-words. They are all "not normal"
	    bool is_a_word = 
		token->isNormal() || 
		( lowercased.find_first_of( L"abcdefghijklmnopqrstuvwxyzäöüß" ) != std::wstring::npos );
	
	    if( is_a_word ) {
		++counter[L"tokens"];
	    }
	
	    std::wstring color = L"black";
	    std::wstring bgcolor = L"white";
	    std::wstring description = L"";

	
	    if( is_a_word ) {
		cands.reset();
		dictSearch_.query( lowercased, &cands );
		std::sort( cands.begin(), cands.end() );

		if( cands.empty() ) {
		    if( compoundFile_ != "NONE" ) {
			// query the compound dictionary including variants
			csl::Vaam< TransTable_t >::CandidateReceiver compoundCands;
			compoundVaam->query( lowercased, &compoundCands );
		    
			if( compoundCands.empty() ) {
			    color = L"purple";
			    description = L"unbekannt";
			    ++counter[description];
			}
			else { // there were results from the compound dic
			    std::sort( compoundCands.begin(), compoundCands.end() );
			
			    csl::Interpretation const& firstCand = compoundCands.at( 0 );
			    if( firstCand.getInstruction().empty() ) { // if modern compound
				color = L"black";
				description = L"Kompositum";
				if( firstCand.getLevDistance() > 0 ) { // erroneous
				    bgcolor = L"#eeeeee";
				    description.append( L", mit Fehlern" );
				    ++counter[description];
				}
				else ++counter[description];
			    }
			    else {
				color = L"red";
				description = L"hypothetisches Kompositum";
				if( firstCand.getLevDistance() > 0 ) { // erroneous
				    bgcolor = L"#eeeeee";
				    description.append( L", mit Fehlern" );
				    ++counter[description];
				}
				else ++counter[description];
			    }
			
			    std::wcout<< token->getWOCR() << "\t" << firstCand << std::endl;
			}
		    }
		    else { // compounds are de-activated
			color = L"purple";
			description = L"unbekannt";
			++counter[description];

			std::wcout<< token->getWOCR() << "\t?:?+[?],dist=?(?)" << std::endl;
		    }
		} // if cands.empty()
		else { // there are interpretations

		    csl::DictSearch::Interpretation const& firstCand = cands.at( 0 );

		    if( firstCand.getDictID() == csl::DictSearch::MODERN ) {
			if( firstCand.getInstruction().empty() ) { // if modern word
			    color = L"black";
			    description = L"modern";
			    if( firstCand.getLevDistance() > 0 ) { // erroneous
				bgcolor = L"#eeeeee";
				description.append( L", mit Fehlern" );
				++counter[description];
			    }
			    else ++counter[description];
			}
			else { // else: hypothetic
			    color = L"red";
			    description = L"hypothetisch";
			    if( firstCand.getLevDistance() > 0 ) { // erroneous
				bgcolor = L"#eeeeee";
				description.append( L", mit Fehlern" );
				++counter[description];
			    }
			    else ++counter[description];
			}
		    }
		    else if( firstCand.getDictID() == csl::DictSearch::HISTORIC ) {
			color = L"orange";
			description = L"historisch";
			if( firstCand.getLevDistance() > 0 ) { // erroneous
			    bgcolor = L"#eeeeee";
			    description.append( L", mit Fehlern" );
			    ++counter[description];
			}
			else ++counter[description];
		    }

		    std::wcout<< token->getWOCR() << "\t" << firstCand << std::endl;

		}
	    } // normal

	    if( token->getWOCR() == L"\n" ) {
		html_out<<L"<br>"<<std::endl;
	    }
	    else {
		html_out << "<a href='javascript:show_hide(\"candlist_"<<counter[L"tokens"]<<"\")' style='color:"<<color<<" ;background-color:"<<bgcolor<<"; text-decoration:none' title='"<<description<<"'>" <<token->getWOCR() << "</a>";
	    }

	} // for each token


    }

private:
    std::string modernDicFile_;
    std::string histDicFile_;
    std::string patternFile_;
    std::string compoundFile_;
    
    csl::DictSearch dictSearch_; 
    

    int verbose_;
};

int main( int argc, char** argv ) {
    std::locale::global( std::locale( "" ) );
    
    if( argc != 5 ) {
	std::wcerr << "Use like: ocr_analysis <modernDic> <histdic> <patternFile> <txtFile>";
	exit( 1 );
    }
    
    std::string modernDicFile = argv[1];
    std::string histDicFile = argv[2];
    std::string patternFile = argv[3];
    std::string compoundFile = "NONE";
    std::string txtFile = argv[4];

    


    /******* CONFIGURATION **************/
    if( modernDicFile == "DEFAULT" ) modernDicFile = "/mounts/data/proj/impact/software/Vaam/cislexAll_plus_dCorrect_kompAbschl.fbdic";
    if( histDicFile == "DEFAULT" ) histDicFile = "/mounts/Users/student/uli/implement/cxx/ocr_analysis/historic.fbdic";
    if( patternFile == "DEFAULT" ) patternFile = "/mounts/data/proj/impact/software/Vaam/patterns.lexgui.txt";
    /************************************/
    

    Dirk dirk( modernDicFile, histDicFile, patternFile );
    dirk.processText( txtFile, std::wcout );

}

