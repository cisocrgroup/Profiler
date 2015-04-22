#ifndef OCRC_IBMGTREADER_H
#define OCRC_IBMGTREADER_H OCRC_IBMGTREADER_H

#include<Document/Document.h>

namespace OCRCorrection {

    /**
     * @brief This class parses a specific comma-separated document/groundtruth format
     *        provided by IBM
     */
    class IBMGTReader {
    public:

	IBMGTReader();

	void parse( char const* csvFile, Document* document );

    private:
	void parseLine( std::wstring const& line );
	
	Document* doc_;
	size_t lineCount_;
    };

    
    IBMGTReader::IBMGTReader() :
	doc_( 0 ) {
    }

    void IBMGTReader::parse( char const* csvFile, Document* document ) {
	doc_ = document;
	std::wifstream fileIn( csvFile );
	fileIn.imbue( csl::CSLLocale::Instance() );
	if( ! fileIn.good() ) {
	    throw OCRCException( "OCRC::IBMGTReader::parse: Could not open input file for reading." );
	}
	
	std::wstring line;

	getline( fileIn, line );
	if( line != L"FileName, Ascii-Truth, Hex-Truth, ABBYY-Dict, ABBYY-Valid, ABBYY-OCR-Hex, ABBYY-OCR-Ascii, OCR-Status" ) {
	    throw OCRCException( "OCRC::IBMGTReader::parse: Could not find expected first line of csv." );
	}

	lineCount_ = 1;
	while( getline( fileIn, line ).good() ) {
	    parseLine( line );
	}
    }


    void IBMGTReader::parseLine( std::wstring const& line ) {
	std::vector< std::wstring > fields;

	size_t startPos = 0;
	size_t endPos = 0;

	++lineCount_;

	while( ( endPos = line.find( ',', startPos ) ) != std::wstring::npos ) {
	    fields.push_back( line.substr( startPos, endPos - startPos ) );
	    startPos = endPos + 1;
	}
	fields.push_back( line.substr( startPos ) );

	if( fields.size() != 8 ) {
	    std::wcout << fields.size() << " fields." << std::endl;
	    throw OCRCException( "OCRC::IBMGTReader::parseLine: Could not find 8 fields as expected." );
	}

// 	for( std::vector< std::wstring >::const_iterator it = fields.begin(); it != fields.end(); ++it ) {
// 	    std::wcout << *it << std::endl;
// 	}
// 	std::wcout << "-------" << std::endl;
	
	//   0           1          2            3           4            5               6             7
	//FileName, Ascii-Truth, Hex-Truth, ABBYY-Dict, ABBYY-Valid, ABBYY-OCR-Hex, ABBYY-OCR-Ascii, OCR-Status
	    
	std::wstring wOrig;
	std::wstring wOCR;
	std::wstring preOrig, postOrig, preOCR, postOCR;

	wOCR = fields.at( 6 );
	doc_->cleanupWord( &wOCR, &preOCR, &postOCR );
	wOrig = fields.at( 1 );
	doc_->cleanupWord( &wOrig, &preOrig, &postOrig );
	if( ! ( preOCR.empty() && preOrig.empty() ) ) {
	    std::wcout << line << std::endl;
	    //throw OCRCException( "OCRC::IBMGTReader::parseLine: Did not expect garbage at beginning of word" );
	}

	Token* tok = new Token(*doc_, doc_->getNrOfTokens(), true);
	tok->setWOCR( wOCR );
	tok->getGroundtruth().setWOrig( wOrig );

//	if( fields.at( 4 ) == L"0" ) tok->getAbbyySpecifics().setSuspicious( true ); // Abbyy-Valid
	if( fields.at( 3 ) == L"0" ) tok->getAbbyySpecifics().setSuspicious( true ); // Abbyy-Dict
	tok->getGroundtruth().setVerified( Token::VERIFIED_GUESSED );

	if( ! postOCR.empty() ) {
	    tok->setDontTouch( true );
	}
	doc_->pushBackToken( tok );
	
	if( ! postOCR.empty() ) {
	    tok = new Token(*doc_, doc_->getNrOfTokens(), false );
	    tok->setWOCR( postOCR );
	    tok->getGroundtruth().setWOrig( postOrig );
	    doc_->pushBackToken( tok );
	}
	

	doc_->pushBackToken( L" ",  false );

    }
    

}

#endif
