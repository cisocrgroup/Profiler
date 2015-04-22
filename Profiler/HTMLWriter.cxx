#include "./Profiler.h"

namespace OCRCorrection {

    /**
     * THIS IS FOR DISS EVALUATION! REMOVE IT AFTERWARDS AND 
     * CHANGE BACK THE INIT. OF loc IN THE CONSTRUCTOR BELOW!
     */
    class MyNumPunct : public std::numpunct< wchar_t > {
	wchar_t do_decimal_point() const { return ',';}
    };


    Profiler::HTMLWriter::HTMLWriter() :
	nrOfTokens_( 0 ),
	iterationNumber_( -1 ) {
	std::wcerr << "OCRC::Profiler::HTMLWriter::Constructor" << std::endl;

	limit_voteWeight_ = 0;
	limit_nrOfTextTokens_ = 10000;
	candlistsForAllIterations_ = false;


	// make sure that no thousands separators are printed
	//std::locale loc = std::locale( std::locale( "" ), new std::numpunct< wchar_t >() );
	// std::locale loc = std::locale( std::locale( "" ), new MyNumPunct() );
	candBoxes_.imbue( csl::CSLLocale::Instance() );
	textStream_.imbue( csl::CSLLocale::Instance() );
	topStream_.imbue( csl::CSLLocale::Instance() );
	bottomStream_.imbue( csl::CSLLocale::Instance() );
	statisticStream_.imbue( csl::CSLLocale::Instance() );

    }

    void Profiler::HTMLWriter::readConfiguration( csl::INIConfig const& iniConf ) {
	if( iniConf.hasKey( "HTMLWriter:limit_nrOfTextTokens" ) ) {
	    limit_nrOfTextTokens_ = iniConf.getdouble( "HTMLWriter:limit_nrOfTextTokens" );
	}
	if( iniConf.hasKey( "HTMLWriter:limit_voteWeight" ) ) {
	    limit_voteWeight_ = iniConf.getdouble( "HTMLWriter:limit_voteWeight" );
	}
	if( iniConf.hasKey( "HTMLWriter:candlistsForAllIterations" ) ) {
	    if( iniConf.getstring( "HTMLWriter:candlistsForAllIterations" ) == std::string( "true" ) ) {
		candlistsForAllIterations_ = true;
	    }
	    else if( iniConf.getstring( "HTMLWriter:candlistsForAllIterations" ) == std::string( "false" ) ) {
		candlistsForAllIterations_ = false;
	    }
	    else throw OCRCException( "OCRC::Profiler::HTMLWriter::readConfiguration: 'candlistsForAllIterations' has a value other than 'true' or 'false' in ini file." );
	}
    }


    std::wostream& Profiler::HTMLWriter::topStream() {
	return topStream_;
    }

    std::wostream& Profiler::HTMLWriter::bottomStream() {
	return bottomStream_;
    }

    void Profiler::HTMLWriter::newIteration( int nr, bool doPrintText ) {
	iterationNumber_ = nr;
	doPrintText_ = doPrintText;
	
	nrOfTokens_ = 0;
    }

    void Profiler::HTMLWriter::registerToken( 
	Profiler_Token const& token,
	Evaluation_Token const& evalToken,
	std::vector< Profiler_Interpretation > const& candlist ) {
	
	if( ! candlist.empty() && doPrintText_ ) {
	    Profiler_Interpretation const& cand = candlist.at( 0 );
	    for( csl::Trace::const_iterator pat = cand.getHistTrace().begin();
		 pat != cand.getHistTrace().end();
		 ++pat ) {
		histPatterns2tokenNumbers_[*pat].push_back( token.getTokenNr() );
	    }
	    for( csl::Trace::const_iterator pat = cand.getOCRTrace().begin();
		 pat != cand.getOCRTrace().end();
		 ++pat ) {
		ocrPatterns2tokenNumbers_[*pat].push_back( token.getTokenNr() );
	    }
	}
	
	
	///////// candlists
	if( ( candlistsForAllIterations_ || doPrintText_ ) && // should we print candlists at all?
	    ! token.isShort() && token.isNormal() ) {         // is it a token that has a proper candlist?
	    
	    double histPrecisionNumerator = evalToken.getCounter( L"guessed_histTrace_nonempty_good" );
	    double histPrecisionDenominator = evalToken.getCounter( L"guessed_histTrace_nonempty" );
	    double histRecallNumerator = evalToken.getCounter( L"guessed_histTrace_nonempty_good" );
	    double histRecallDenominator = evalToken.getCounter( L"shouldDetectHistPattern" );

	    double ocrPrecisionNumerator = evalToken.getCounter( L"guessed_ocrTrace_nonempty_good" );
	    double ocrPrecisionDenominator = evalToken.getCounter( L"guessed_ocrTrace_nonempty" );
	    double ocrRecallNumerator = evalToken.getCounter( L"guessed_ocrTrace_nonempty_good" );
	    double ocrRecallDenominator = evalToken.getCounter( L"shouldDetectOCRPattern" );
	    
	    
	    // the buttons to switch between candBoxes for all iterations 
	    candBoxes_ <<"<div id='candlist_"<< iterationNumber_ << "_" << token.getTokenNr() <<"' class='candBox'>" << std::endl;
	    candBoxes_ << "<a style='position:absolute;right:2px;top:2px;font-size:160%;text-decoration:none;' href='javascript:hide(\"candBoxes\")' title='close box'>&#x2612;</a> " << std::endl;
	    
	    if( candlistsForAllIterations_ ) {
		candBoxes_ << "Iteration: "<< iterationNumber_ << std::endl;

		candBoxes_ <<"<a href='javascript:" 
			   << "show(\"candBoxes\",\"candlist_"<< iterationNumber_ - 1 << "_" << token.getTokenNr() << "\")" 
			   << "'>&lt;&lt;</a> | "
			   <<"<a href='javascript:" 
			   << "show(\"candBoxes\",\"candlist_"<< iterationNumber_ + 1 << "_" << token.getTokenNr() << "\")" 
			   << "'>&gt;&gt;</a>" 
			   << std::endl;
	    }
	    else {
		candBoxes_ << "Iteration: "<< iterationNumber_ << std::endl;
	    }

	    candBoxes_
		<< "<p>Interpretations for <b>"<< xml_escape( token.getWOCR_lc() ) <<"</b>:</p>" << std::endl
		<< "<div class='section'><span class='headline'>Groundtruth info</span><br>"
		<< "GT wOrig = " << token.getGroundtruth().getWOrig_lc() << "<br>" << std::endl
		<< "GT interpretation = { " << token.getGroundtruth().getBaseWord() << "+" << token.getGroundtruth().getHistTrace()
		<< " } + ocr" << token.getGroundtruth().getOCRTrace() << "<br>" << std::endl
		<< "GT classified = " << token.getGroundtruth().getClassified() << "<br>" << std::endl
		<< "</div>" << std::endl
		<< "<div class='section'><span class='headline'>Precision/ Recall</span><br>"
		<< "<table style='border:0px solid #bbbbbb'><tr><td>" << std::endl
		<< "Hist Precision: " << std::setiosflags( std::ios::fixed ) << std::setprecision( 2 ) << histPrecisionNumerator << "/" << histPrecisionDenominator << "<br>" << std::endl
		<< "Hist Recall: " << std::setiosflags( std::ios::fixed ) << std::setprecision( 2 ) << histRecallNumerator << "/" << histRecallDenominator << "<br>" << std::endl
		<< "OCR Precision: " << std::setiosflags( std::ios::fixed ) << std::setprecision( 2 ) << ocrPrecisionNumerator << "/" << ocrPrecisionDenominator << "<br>" << std::endl
		<< "OCR Recall: " << std::setiosflags( std::ios::fixed ) << std::setprecision( 2 ) << ocrRecallNumerator << "/" << ocrRecallDenominator << "<br>" << std::endl
		<< "</td><td>" << std::endl
		<< "</td></tr></table>" << std::endl
		<< "</div>" << std::endl
		<< "<div class='section'><span class='headline'>Suspiciousness</span><br>"
		<< "abbyySuspicious = " << ( token.getAbbyySpecifics().isSuspicious() ? L"true" : L"false" ) << "<br>"
		<< "profilerSuspicious = " << ( token.isSuspicious() ? L"true" : L"false" ) << "<br>"
		<< "</div>" << std::endl
		<< "<div class='section'><span class='headline'>Prob. Mass Corrective Factor</span><br>"
		<< "probNormalizationFactor / (doc_length/wOCRFreq) = " 
		<< token.getProbNormalizationFactor() <<  "/" << ( (double)10000 / token.getWOCRFreq() ) 
		<< " = " << token.getProbNormalizationFactor() / ( (double)10000 / token.getWOCRFreq() ) << "<br>" << std::endl
		<< "</div>" << std::endl
		<< "<div class='section'><span class='headline'>Interpretations</span><br>"
		<< "(total: "<<candlist.size()<<" interpretations)<br>"<<std::endl;



	    
	    size_t candCount = 0;
	    for( std::vector< Profiler_Interpretation >::const_iterator cand = candlist.begin(); cand != candlist.end(); ++cand ) {
		if( ( candCount > 0 ) && ( cand->getVoteWeight() < limit_voteWeight_ ) ) {
		    break; 
		}
		std::wstring style = (candCount % 2 ) ? L"cand_odd" : L"cand_even";
		candBoxes_ << "<div class='" << style << "'>" ;
		cand->print( candBoxes_ );
		candBoxes_ << ",voteWeight=" << cand->getVoteWeight() << std::endl;
		candBoxes_ << "</div>" <<std::endl;
		++candCount;
	    }
	    candBoxes_<<"</div>" << std::endl 
		      <<"</div><!--/candlist-->" << std::endl;
	}
	
	
	
	
	// build text token
	if( doPrintText_ ) { 
	    if( nrOfTokens_ < limit_nrOfTextTokens_ ) {
	    
		if( token.getWOCR() == L"\n" ) {
		    textStream_<<L"<br>"<<std::endl;
		}
		else if( token.isNormal() ) {
		    std::wstring color = L"black";
		    std::wstring bgcolor = L"white";
		    std::wstring border = L"none";
		    std::wstring other = L"text-decoration:none;";
		    std::wstring description = L"blabla";
		    
		    //                                                        
		    adjustLayout( token, candlist,  &color, &border, &bgcolor, &other, &description );
		    
		    textStream_ << "<a href='javascript:show_hide(\"candBoxes\",\"candlist_"<< iterationNumber_ << L"_" << token.getTokenNr() <<"\")' style='color:"
				<<color<<" ;background-color:"<<bgcolor<<"; border: " << border << "; " << other << "' title='"<<description
				<<"'>" << xml_escape( token.getWOCR_lc() ) << "</a>";
		}
		else {
		    textStream_ << "<span style='color: purple; background-color: white; text-decoration:none;' title='not normal'>" 
				<< xml_escape( token.getWOCR() ) << "</span>";
		}
	    }
	    else if( nrOfTokens_ == limit_nrOfTextTokens_ ) {
		textStream_ << "<span style='color: silver;'> [Displayed text is clipped here. " 
			    << "See statistics below on how many tokens were profiled.]</span>";
	    }
	}

	++nrOfTokens_;
    } // registerToken()
    
    void Profiler::HTMLWriter::registerStatistics( std::map< std::wstring, double > const& counter,
						   GlobalProfile const& globalProfile,
						   Evaluation const& evaluation ) {

	
	statisticStream_ << "<div id='statBox_iter"<< iterationNumber_ << "' class='statBox'>" << std::endl
			 <<"<a style='text-decoration:none;' href='javascript:" 
			 << "show(\"statBoxes\",\"statBox_iter"<< iterationNumber_ - 1  << "\")" 
			 << "'>&#x21E6;</a> | iteration number " << iterationNumber_ << " | " 
			 <<"<a style='text-decoration:none;' href='javascript:" 
			 << "show(\"statBoxes\",\"statBox_iter"<< iterationNumber_ + 1 << "\")" 
			 << "'>&#x21e8;</a>" 
			 << std::endl
	    
			 << "<h1>Statistics</h1>" << std::endl

			 << "<h2>Word counts</h2>" << std::endl
			 << "<table>" << std::endl;
	
	std::vector< std::wstring > counter_keys;
	counter_keys.push_back( L"normalAndLongTokens" );
	counter_keys.push_back( L"notNormal" );
	counter_keys.push_back( L"short" );
 	counter_keys.push_back( L"wasProfiled" );
	counter_keys.push_back( L"guessed_histTrace_nonempty" );
	counter_keys.push_back( L"guessed_ocrTrace_nonempty" );
	


	for( std::vector< std::wstring >::const_iterator key = counter_keys.begin(); key != counter_keys.end(); ++key ) {
	    statisticStream_ << "<tr><td>" << *key << "</td><td align='right'>" << Utils::queryConstMap< std::wstring, double >( counter, *key, 0 ) << "</td></tr>" << std::endl;
	}

	statisticStream_<<"</table>" << std::endl
	
			<< "<h2>Error rate considerations</h2>" << std::endl
			<< " #erroneous_tokens/#profiled_tokens = "
			<< Utils::queryConstMap< std::wstring, double >( counter, L"guessed_ocrTrace_nonempty", 0 ) << "/" << Utils::queryConstMap< std::wstring, double >( counter, L"wasProfiled", 0 )
			<< " = " << Utils::queryConstMap< std::wstring, double >( counter, L"guessed_ocrTrace_nonempty", 0 ) / Utils::queryConstMap< std::wstring, double >( counter, L"wasProfiled", 0 )
			<< std::endl;
	
	statisticStream_ << "<h2>Dictionary Distribution</h2>" << std::endl;
	statisticStream_ << "<table>" << std::endl
			 << "<tr><th>Dictionary</th><th>#</th><th>Proportion</th></tr>" << std::endl;
	for( std::map< std::wstring, GlobalProfile::DictDistributionPair >::const_iterator it = globalProfile.dictDistribution_.begin();
	     it!= globalProfile.dictDistribution_.end();
	     ++ it ) {
	    
	    statisticStream_ << "<tr>"
			     << "<td>" << it->first << "</td>"
			     << "<td>" << it->second.frequency << "</td>"
			     << "<td>" << ( it->second.proportion * 100 ) << " %</td>"
			     << "</tr>"
			     <<  std::endl;

	}
	statisticStream_ << "</table>" << std::endl;
	    
	    
	evaluation.writeStatistics( statisticStream_, iterationNumber_ );

	    
	//evaluation.writeHistPatternPrecisionRecall( histPatternProbabilities, statisticStream_ );
	// statisticStream_ << "<pre>" << std::endl; 
	// evaluation.writeOCRPatternPrecisionRecall( ocrPatternProbabilities, statisticStream_ );
	// statisticStream_ << "</pre>" << std::endl; 
	    
	statisticStream_ <<"<a style='text-decoration:none;' href='javascript:" 
			 << "show(\"statBoxes\",\"statBox_iter"<< iterationNumber_ - 1  << "\")" 
			 << "'>&#x21e6;</a> | iteration number " << iterationNumber_ << " | " 
			 <<"<a style='text-decoration:none;' href='javascript:" 
			 << "show(\"statBoxes\",\"statBox_iter"<< iterationNumber_ + 1 << "\")" 
			 << "'>&#x21e8;</a>" 
			 << std::endl;
	    
	statisticStream_ << "<h2>List comparison</h2>" << std::endl;
	statisticStream_ << "<h3>Hist</h3>" << std::endl;

	statisticStream_ << "<table border='1'>" << std::endl;
	statisticStream_ << "<tr><td>nrOfHistPatterns</td><td id=\"DATA_nrOfHistPatterns_it" << iterationNumber_ << "\">" << globalProfile.histPatternProbabilities_.size() << "</td></tr>" << std::endl;
	statisticStream_ << "</table>" << std::endl;

	printListComparison( evaluation.topKListStats_hist_, evaluation.getHistPatternProbabilities(), globalProfile.histPatternProbabilities_, L"hist" );

	statisticStream_ <<"<a style='text-decoration:none;' href='javascript:" 
			 << "show(\"statBoxes\",\"statBox_iter"<< iterationNumber_ - 1  << "\")" 
			 << "'>&#x21e6;</a> | iteration number " << iterationNumber_ << " | " 
			 <<"<a style='text-decoration:none;' href='javascript:" 
			 << "show(\"statBoxes\",\"statBox_iter"<< iterationNumber_ + 1 << "\")" 
			 << "'>&#x21e8;</a>" 
			 << std::endl;

	statisticStream_ << "<h3>OCR</h3>" << std::endl;
	statisticStream_ << "<table border='1'>" << std::endl;
	statisticStream_ << "<tr><td>nrOfOCRPatterns</td><td id=\"DATA_nrOfOCRPatterns_it" << iterationNumber_ << "\">" << globalProfile.ocrPatternProbabilities_.size() << "</td></tr>" << std::endl;
	statisticStream_ << "</table>" << std::endl;

	printListComparison( evaluation.topKListStats_ocr_, evaluation.getOCRPatternProbabilities(), globalProfile.ocrPatternProbabilities_, L"ocr"  );

	statisticStream_ <<"<a style='text-decoration:none;' href='javascript:" 
			 << "show(\"statBoxes\",\"statBox_iter"<< iterationNumber_ - 1  << "\")" 
			 << "'>&#x21e6;</a> | iteration number " << iterationNumber_ << " | " 
			 <<"<a style='text-decoration:none;' href='javascript:" 
			 << "show(\"statBoxes\",\"statBox_iter"<< iterationNumber_ + 1 << "\")" 
			 << "'>&#x21e8;</a>" 
			 << std::endl;

	statisticStream_ << "</div>" << std::endl;
	statisticStream_ << "<script>show(\"statBoxes\", \"statBox_iter" << iterationNumber_ << "\")</script>" << std::endl;
	
    }

    void Profiler::HTMLWriter::printListComparison( Evaluation::TopKListStatistics const& topKListStats, 
						    PatternContainer const& groundtruthPatterns, 
						    PatternContainer const& profiledPatterns,
						    std::wstring const& description
	) {
	


	
	// create sorted lists from the pattern maps
	std::vector< std::pair< csl::Pattern, double > > patterns_profiled_sorted;
	profiledPatterns.sortToVector( &patterns_profiled_sorted );
	
	std::vector< std::pair< csl::Pattern, double > > patterns_gt_sorted;
	groundtruthPatterns.sortToVector( &patterns_gt_sorted );
	
	std::vector< std::pair< csl::Pattern, double > >::const_iterator profIterator = patterns_profiled_sorted.begin();
	std::vector< std::pair< csl::Pattern, double > >::const_iterator gtIterator = patterns_gt_sorted.begin();
	
	
	statisticStream_ << "<table border='1'>" << std::endl;
	statisticStream_ << "<tr><th>#</th><th colspan='2'>Profiler</th><th colspan='2'>Groundtruth</th><th>% matches in top k</th></tr>" << std::endl;
	
	statisticStream_ << "<tr>" << std::endl;
	size_t rank = 0;
	while( ( ( profIterator != patterns_profiled_sorted.end() ) || ( gtIterator != patterns_gt_sorted.end() ) ) 
	       // && rank < 30 
	    ) {
	    
	    statisticStream_ << "<tr><td>" << ++rank << "</td>" <<  std::endl;
	    if( profIterator != patterns_profiled_sorted.end() ) {
		statisticStream_ << "<td>" 
				 << profIterator->first.toString()
				 << "</td><td>" 
				 <<  std::setiosflags( std::ios::fixed ) << std::setprecision( 4 ) << profiledPatterns.getWeight( profIterator->first ) << ", "
				 <<  std::setiosflags( std::ios::fixed ) << std::setprecision( 2 ) << profIterator->second  
				 << "</td>" 
				 << std::endl;
		++profIterator;
	    }
	    else {
		statisticStream_ << "<td/><td/>" << std::endl;
	    }
	    if( gtIterator != patterns_gt_sorted.end() ) {
		statisticStream_ << "<td>" 
				 << gtIterator->first.toString()
				 << "</td><td>" 
				 << setiosflags( std::ios::fixed ) << std::setprecision( 4 ) << groundtruthPatterns.getWeight( gtIterator->first ) << ",&nbsp;"
				 << setiosflags( std::ios::fixed ) << std::setprecision( 2 ) << gtIterator->second  
				 << "</td>" 
				 << std::endl;
		++gtIterator;
	    }
	    else {
		statisticStream_ << "<td/><td/>" << std::endl;
	    }
	    try {
		statisticStream_ << "<td id=\"DATA_topkmatches_" << description << "_it" << iterationNumber_ << "_rank" << rank << "\">" << topKListStats.matchesInTopK_.at( rank ) << "</td>" << std::endl;
	    }
	    catch( std::exception exc ) {
		statisticStream_ << "<td>" << exc.what() << "</td>" << std::endl;
		statisticStream_ << "<td></td>" << std::endl;
	    }
		
	    statisticStream_ << "</tr>" << std::endl;
	}
		
	statisticStream_ << "</table>" << std::endl;
    }

    void Profiler::HTMLWriter::print( std::wostream& os ) {
	
	os << "<html>" << std::endl
	   << "<head>"<<std::endl
	   << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl
	   << "<title>Profiler Visualisation</title>" << std::endl
	   << "<style type='text/css'>" << std::endl
	   << "h1 {font-size: 125%; font-weight:bold;}" << std::endl
	   << "h2 {font-size: 110%; font-weight:bold; border-bottom:1px solid silver; margin-top:20px;}" << std::endl
	   << "h3 {font-size: 100%; font-weight:bold;}" << std::endl
	   << "div.text {border: 1px solid #dddddd; margin:3px;padding:3px;}" << std::endl
	   << "div.cand_odd {font-size:0.9em;}" << std::endl
	   << "div.cand_even {font-size:0.9em;background-color:#eeeeff;}" << std::endl
	   << "div.candBox {position:fixed; top:10px;left:10px;background-color:#f5f5f5; border: 3px solid silver; display:none}" << std::endl
	   << "div.candBox div.section {background-color:#eeeeff;border:1px solid white; margin:5px}" << std::endl
	   << "div.candBox div.section span.headline {background-color:#efefff;font-variant:small-caps; font-size:smaller; font-weight:bold}" << std::endl
	   << "div.statBox {background-color:white;width:100%; padding:3px; border: 1px solid silver; display:none}" << std::endl
	   << "div.statBox table {width:50%;}" << std::endl
	   << "div.statBox td {background-color:f7f7f7; border: 1px solid silver;margin:0px}" << std::endl
	   << "div.statBox th {background-color:f7f7f7; border: 1px solid silver;margin:0px}" << std::endl
	   << "</style>" << std::endl
	   << "<script language='javascript'>" << std::endl;
	
	os << "var open = new Array();" << std::endl
	   << "open[\"candBoxes\"] = null;" << std::endl
	   << "open['statBoxes'] = null;" << std::endl
	   << "function show(area, id) { var obj=document.getElementById(id); if(obj) { hide(area); obj.style.display='block';open[area]=id} }" << std::endl
	   << "function hide(area) {if(open[area]) {document.getElementById(open[area]).style.display='none';} open[area]=null;}" << std::endl
	   << "function show_hide(area,id) {" << std::endl
	   << "  if( open[area] != id ) {" << std::endl
	   << "    show(area,id); " << std::endl
	   << "  } else {" << std::endl
	   << "    hide(area);" << std::endl
	   << "  }" << std::endl
	   << "}" << std::endl
	   << "</script>" << std::endl
	   << "</head>" << std::endl
	   << "<body>" << std::endl
	   << topStream_.str() << std::endl << std::endl

	    // legend for word markup
	   << "<div style='position:fixed;top:10px;right:10px;background-color:white;border:1px solid red;'>" << std::endl
	   << "<span style='border: 3px solid red'>thcil</span> - erroneous word<br>" << std::endl
	   << "<span style='background-color: #ffff00 '>thcil</span> - Only Abbyy suspicious<br>" << std::endl
	   << "<span style='background-color: #eeeeee '>thcil</span> - Only Profiler suspicious<br>" << std::endl
	   << "<span style='background-color: #ffbe00 '>thcil</span> - Both  suspicious<br>" << std::endl
	   << "</div>" << std::endl
	   << std::endl
	    


	   << "<div class='text'>" << std::endl
	   << textStream_.str() 
	   << "</div>" << std::endl 
	   << std::endl << std::endl 
	   << statisticStream_.str()
	   << candBoxes_.str()
	   << std::endl << std::endl 
	   << bottomStream_.str();
	

	os << "<h1>All hist patterns and where they are detected" << std::endl;
	os << "<table border='1'>" << std::endl;
	for( std::map< csl::Pattern, std::vector< int > >::const_iterator pat = histPatterns2tokenNumbers_.begin();
	     pat != histPatterns2tokenNumbers_.end();
	     ++pat ) {
	    os << "<tr><td>" << pat->first.toString() << "</td><td>" << std::endl;
	    for( std::vector< int >::const_iterator nr = pat->second.begin();
		 nr != pat->second.end();
		 ++nr ) {
		
		os << "<a href='javascript:show_hide(\"candBoxes\",\"candlist_"<< iterationNumber_ << L"_" << *nr <<"\")' >" << *nr << "</a>, ";
	    }
	    os << "</td></tr>" << std::endl;
	}
	os << "</table>" << std::endl;

	os << "<h1>All ocr patterns and where they are detected" << std::endl;
	os << "<table border='1'>" << std::endl;
	for( std::map< csl::Pattern, std::vector< int > >::const_iterator pat = ocrPatterns2tokenNumbers_.begin();
	     pat != ocrPatterns2tokenNumbers_.end();
	     ++pat ) {
	    os << "<tr><td>" << pat->first.toString() << "</td><td>" << std::endl;
	    for( std::vector< int >::const_iterator nr = pat->second.begin();
		 nr != pat->second.end();
		 ++nr ) {
		
		os << "<a href='javascript:show_hide(\"candBoxes\",\"candlist_"<< iterationNumber_ << L"_" << *nr <<"\")' >" << *nr << "</a>, ";
	    }
	    os << "</td></tr>" << std::endl;
	}
	os << "</table>" << std::endl;



	os << "</body></html>"<< std::endl;

	topStream_.str( L"" );
	textStream_.str( L"" );
	candBoxes_.str( L"" );
	bottomStream_.str( L"" );
	nrOfTokens_ = 0;
    }

    void Profiler::HTMLWriter::adjustLayout( Profiler_Token const& token, std::vector< Profiler_Interpretation > const& candlist, std::wstring* color, std::wstring* border, std::wstring* bgcolor, std::wstring* other, std::wstring* description ) {
	
	if( candlist.size() == 0 ) {
	    if( token.isNormal() ) {
		if( token.isShort() ) {
		    *color = L"#888888";
		    *bgcolor= L"white";
		    *description = L"short";
		}
		else {
		    *color = L"purple";
		    *bgcolor= L"white";
		    *description = L"unknown";
		}
	    }
	    else {
		*color = L"#888888";
		*bgcolor= L"white";
		*description = L"not normal";
	    }
	}
	else {
	    Profiler_Interpretation const& cand = candlist.at( 0 );

	    
	    *description = cand.getDictModule().getName();
	    
	    if( cand.getHistTrace().empty() ) { // if modern word
		*color = L"black";
	    }
	    else { // else: hypothetic
		*color = L"red";
	    }

	    if( ! cand.getOCRTrace().empty() ) { 
		*bgcolor = L"cccccc";
	    }
	    
	} // there are cands

	if( token.isDontTouch() ) {
	    *other = L"text-decoration:line-through";
	    description->append( L", DONT TOUCH" );
	}

	// THIS OVERRIDES OTHER SPECIFICATIONS!!!
	// *bgcolor = L"white";
	// if( token.getGroundtruth().getWOrig_lc() != token.getWOCR_lc() ) {
	//     *border = L"3px solid red";
	//     description->append( L", OCR-Fehler" );
	// }

	// if( token.getAbbyySpecifics().isSuspicious() && token.isSuspicious() ) {
	//     *bgcolor = L"#ffbe00"; // both suspicious
	//     description->append( L", both suspicious" );
	// } 
	// else if( token.getAbbyySpecifics().isSuspicious() ) {
	//     *bgcolor = L"ffff00"; // only abbyy suspicous
	//     description->append( L", ABBYY suspicious" );
	// }
	// else if( token.isSuspicious() ) {
	//     *bgcolor = L"#eeeeee";  // only profiler suspicous
	//     description->append( L", Profiler suspicious" );
	// }
	// else if( token.getAbbyySpecifics().isSuspicious() ) {
	//     *bgcolor = L"ffff00"; // only abbyy suspicous
	//     description->append( L", ABBYY suspicious" );
	// }


    }

    std::wstring Profiler::HTMLWriter::xml_escape( std::wstring const& input ) const {
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

}
