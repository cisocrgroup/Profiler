#ifndef OCRCORRECTION_DIRK_H__
#define OCRCORRECTION_DIRK_H__

#include<string>
#include<sstream>
#include<TransTable/TransTable.h>
#include<DictSearch/DictSearch.h>
#include<Document/Document.h>
#include <time.h>

#include "CompoundDictModule.h"


class Dirk {
public:
    Dirk():
	compoundModule_( 0 )
	{

    }

    ~Dirk() {
	if( compoundModule_ ) delete( compoundModule_ );
    }

    csl::DictSearch& getDictSearch() {
	return dictSearch_;
    }

    void processText( std::string const& source, std::wostream& outStream ) {
	OCRCorrection::Document document;

	document.parseTXT( source.c_str() );


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


	    if( ! token->isNormal() ) {
		color = L"silver";
		if( is_a_word ) {
		    description = L"kurz";
		    ++counter[description];
		}
		else {
		    description = L"not normal";
		}
	    }
	    else { // normal
		cands.reset();
		dictSearch_.query( lowercased, &cands );

		// here, a custom-made sort-operator can be passed as 3rd argument
		std::sort( cands.begin(), cands.end() );

		if( cands.empty() ) {
		    color = L"purple";
		    description = L"unbekannt";
		    ++counter[description];
		}
		else { // there are interpretations

		    html_out
			<<"<div id='candlist_"<<counter[L"tokens"]<<"' style='position:fixed; top:10px;left:10px;background-color:#ccccff; border: 1px solid silver; display:none' onClick='hide(\"candlist_"<<counter[L"tokens"]<<"\")'>" << std::endl
			<<"<p>Kandidaten/Interpretationen fuer <b>"<<lowercased<<"</b>:</p>" << std::endl
			<<"(insgesamt: "<<cands.size()<<" Kandidaten)<br>"<<std::endl
			<<"<pre>" << std::endl;

 		    for( csl::DictSearch::CandidateSet::const_iterator cand = cands.begin(); cand != cands.end(); ++cand ) {
			if( cand - cands.begin() > 30 ) break;
 			html_out <<*cand << std::endl;
 		    }
		    html_out<<"</pre></div>"<<std::endl;

		    csl::DictSearch::Interpretation const& firstCand = cands.at( 0 );

		    if( firstCand.getDictModule().getName() == L"modern" ) {
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
		    else if( firstCand.getDictModule().getName() == L"hist" ) {
			color = L"orange";
			description = L"historisch";
			if( firstCand.getLevDistance() > 0 ) { // erroneous
			    bgcolor = L"#eeeeee";
			    description.append( L", mit Fehlern" );
			    ++counter[description];
			}
			else ++counter[description];
		    }
		    else if( firstCand.getDictModule().getName() == L"compound" ) {
			color = L"green";
			description = L"Kompositum";
			if( firstCand.getLevDistance() > 0 ) { // erroneous
			    bgcolor = L"#eeeeee";
			    description.append( L", mit Fehlern" );
			    ++counter[description];
			}
			else ++counter[description];
		    }
		    else if( firstCand.getDictModule().getName() == L"latin" ) {
			color = L"brown";
			description = L"Latein";
			if( firstCand.getLevDistance() > 0 ) { // erroneous
			    bgcolor = L"#eeeeee";
			    description.append( L", mit Fehlern" );
			    ++counter[description];
			}
			else ++counter[description];
		    }
		    else if( firstCand.getDictModule().getName() == L"geo" ) {
			color = L"brown";
			description = L"geo";
			if( firstCand.getLevDistance() > 0 ) { // erroneous
			    bgcolor = L"#eeeeee";
			    description.append( L", mit Fehlern" );
			    ++counter[description];
			}
			else ++counter[description];
		    }


		} // there are interpretations
	    } // normal

	    if( token->getWOCR() == L"\n" ) {
		html_out<<L"<br>"<<std::endl;
	    }
	    else {
		html_out << "<a href='javascript:show_hide(\"candlist_"<<counter[L"tokens"]<<"\")' style='color:"<<color<<" ;background-color:"<<bgcolor<<"; text-decoration:none' title='"<<description<<"'>" <<token->getWOCR() << "</a>";
	    }

	} // for each token


	////////////////////////////////////////////////
	//////// HTML OUTPUT ///////////////////////////
	////////////////////////////////////////////////

	outStream<<"<html>" << std::endl
		  <<"<head>"<<std::endl
		  <<"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl
		  <<"<title>OCR Analyse</title>" << std::endl
		  <<"<script language='javascript'>" << std::endl
		  <<"function show(id) {document.getElementById(id).style.display='block';}" << std::endl
		  <<"function hide(id) {document.getElementById(id).style.display='none'}" << std::endl
		  <<"function show_hide(id) {" << std::endl
		  <<"  if(document.getElementById(id).style.display == 'none') {" << std::endl
		  <<"    document.getElementById(id).style.display = 'block'; " << std::endl
		  <<"  } else {" << std::endl
		  <<"    document.getElementById(id).style.display = 'none';" << std::endl
		  <<"  }" << std::endl
		  <<"}" << std::endl
		  <<"</script>" << std::endl
		  <<"</head>" << std::endl
		  <<"<body>" << std::endl;

	time_t t = time(NULL);
	outStream<<"created at:"<<asctime(localtime(&t))<<std::endl;
	outStream<<"<table border='1'>" << std::endl;
	for( std::vector< std::wstring >::const_iterator key = counter_keys.begin(); key != counter_keys.end(); ++key ) {
	    outStream << "<tr><td>" << *key << "</td><td align='right'>" << counter[*key] << "</td></tr>" << std::endl;
	    //wprintf( L"%20ls%5d\n", key->c_str(), counter[*key] );
	}
	outStream<<"</table>" << std::endl;

	outStream << html_out.str();
	outStream << "</body></html>"<< std::endl;

    }

private:
    std::string modernDictFile_;
    std::string histDictFile_;
    std::string patternFile_;
    std::string compoundFile_;

    csl::DictSearch dictSearch_;
    CompoundDictModule* compoundModule_;

    int verbose_;
}; // class Dirk

#endif /* OCRCORRECTION_DIRK_H__ */
