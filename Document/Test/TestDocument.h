#ifndef OCRC_TESTDOCUMENT_H
#define OCRC_TESTDOCUMENT_H OCRC_TESTDOCUMENT_H

#include "../Document.h"

#include <cppunit/extensions/HelperMacros.h>


namespace OCRCorrection {
    class TestDocument : public CppUnit::TestFixture  {

	CPPUNIT_TEST_SUITE( TestDocument );
	CPPUNIT_TEST( testBasics );
	CPPUNIT_TEST( testAlignedIn );
	CPPUNIT_TEST( testCandidate );
	CPPUNIT_TEST( testToken );
	CPPUNIT_TEST( testTokenErase );
	CPPUNIT_TEST( testTokenSplit );
	CPPUNIT_TEST( testTokenMerge );
	CPPUNIT_TEST( testHyphenation );
	CPPUNIT_TEST_SUITE_END();
    public:
	TestDocument();

	void run();

	void testBasics();
	void testAlignedIn();
	void testPages();
	void testCandidate();
	void testToken();
	void testTokenErase();
	void testTokenSplit();
	void testTokenMerge();
	void testHyphenation();

    private:
	std::locale loc_;
    };

    CPPUNIT_TEST_SUITE_REGISTRATION( TestDocument );

    TestDocument::TestDocument() {

    }

    void TestDocument::run() {
	testPages();
	testBasics();
	testCandidate();
	testToken();
	testTokenErase();
	testTokenSplit();
    }


    /**
     * test the basic methods
     */
    void TestDocument::testBasics() {
	// Document doc;
	// doc.parseTXT( "../Document/Test/test.txt" );

	// for( Document::iterator tok = doc.begin(); tok != doc.end(); ++tok ) {
	//     std::wcout << L"'" << tok->getWOCR() << L"'" << std::endl;
	// }


    }

    void TestDocument::testAlignedIn() {
	// Document doc;
	// doc.parseAlignedTXT( "../Document/Test/test.align.txt" );

	// for( Document::iterator tok = doc.begin(); tok != doc.end(); ++tok ) {
	//     std::wcout << L"'" << tok->getGroundtruth().getWOrig() << L"'" << L"\t'" << tok->getWOCR() << L"'" << std::endl;
	// }
    }

    void TestDocument::testPages() {
	Document doc;
	doc.newPage( "page1.tif" );
	doc.pushBackToken( L"das", true );
	doc.pushBackToken( L" ", false );
	doc.pushBackToken( L"ist", true );
	doc.pushBackToken( L" ", false );
	doc.pushBackToken( L"Seite", true );
	doc.pushBackToken( L" ", false );
	doc.pushBackToken( L"1", true );
	doc.pushBackToken( L".", false );

	doc.newPage( "page2.tif" );
	doc.pushBackToken( L"das", true );
	doc.pushBackToken( L" ", false );
	doc.pushBackToken( L"ist", true );
	doc.pushBackToken( L" ", false );
	doc.pushBackToken( L"Seite", true );
	doc.pushBackToken( L" ", false );
	doc.pushBackToken( L"2", true );
	doc.pushBackToken( L".", false );

	for( Document::PageIterator pageIt= doc.pagesBegin(); pageIt != doc.pagesEnd(); ++pageIt ) {
                std::wstring wide_imageFile(Utils::utf8(pageIt->getImageFile()));
                //csl::CSLLocale::string2wstring( pageIt->getImageFile(), wide_imageFile );
	    std::wcout << "PAGE: image=" << wide_imageFile << std::endl;

	    for( Document::iterator it = pageIt->begin(); it != pageIt->end(); ++it ) {
		(*it).print();
	    }
	    std::wcout << "PAGE END" << std::endl;
	}

	CPPUNIT_ASSERT( doc.at( 0 ).getPageIndex() == 0 );
	CPPUNIT_ASSERT( doc.at( 3 ).getPageIndex() == 0 );
	CPPUNIT_ASSERT( doc.at( 7 ).getPageIndex() == 0 );
	CPPUNIT_ASSERT( doc.at( 8 ).getPageIndex() == 1 );
	CPPUNIT_ASSERT( doc.at( 12 ).getPageIndex() == 1 );
	CPPUNIT_ASSERT( doc.at( 15 ).getPageIndex() == 1 );
    }

    void TestDocument::testCandidate() {
	Candidate cand;
	cand.parseFromString( L"Churfürstliche:{kurfürstliche+[(k:ch,0)]}+ocr[(c:k,0)],voteWeight=0.65678,levDistance=1", 0 );
	cand.print();
	std::wcout << cand.toString() << std::endl;
    }

    void TestDocument::testToken() {
	Document doc;
	doc.newPage( "page1.tif" );
	doc.pushBackToken( L"khurfürstliche", true );

	Candidate cand1, cand2;
	cand1.parseFromString( L"Khurfürstliche:{kurfürstliche+[(k:kh,0)]}+ocr[],voteWeight=0.65678,levDistance=0", 0 );
	cand2.parseFromString( L"Churfürstliche:{kurfürstliche+[(k:ch,0)]}+ocr[(c:k,0)],voteWeight=0.65678,levDistance=1", 0 );

	Token& tok = doc.at( 0 );

	tok.addCandidate( cand1 );
	tok.addCandidate( cand2 );

	for( Token::CandidateIterator it = tok.candidatesBegin(); it != tok.candidatesEnd(); ++it ) {
	    std::wcout << it->toString() << std::endl;
	}

    }

    void TestDocument::testTokenErase() {
	Document doc;
	doc.newPage( "page1.tif" );
	doc.pushBackToken( L"word1", true ); // 0
	doc.pushBackToken( L" ", false );
	doc.pushBackToken( L"word2", true ); // 2
	doc.pushBackToken( L" ", false );
	doc.pushBackToken( L"word3", true ); // 4
	doc.pushBackToken( L" ", false );
	doc.pushBackToken( L"word4", true ); // 6
	doc.pushBackToken( L" ", false );
	doc.pushBackToken( L"word5", true ); // 8
	doc.pushBackToken( L" ", false );
	doc.pushBackToken( L"word6", true ); // 10
	doc.pushBackToken( L" ", false );
	doc.pushBackToken( L"word7", true ); // 12

	doc.newPage( "page2.tif" );
	doc.pushBackToken( L"word8", true ); // 13
	doc.pushBackToken( L" ", false );
	doc.pushBackToken( L"word9", true ); // 15

	CPPUNIT_ASSERT_EQUAL( (size_t)16, doc.getNrOfTokens() );

	doc.eraseToken( 2, 6 ); // delete word2, word3 and the space afterwards

	CPPUNIT_ASSERT_EQUAL( (size_t)12, (size_t)doc.getNrOfTokens() );
	CPPUNIT_ASSERT( L"word4" == doc.at( 2 ).getWOCR() );

	CPPUNIT_ASSERT( (size_t)0 == doc.at( 2 ).getPageIndex() );
	CPPUNIT_ASSERT( (size_t)0 == doc.at( 8 ).getPageIndex() );

	doc.eraseToken( 8, 9 ); // delete word7 at end of page

	CPPUNIT_ASSERT( L"word8" == doc.at( 8 ).getWOCR() );
	CPPUNIT_ASSERT( (size_t)1 == doc.at( 8 ).getPageIndex() );

	doc.eraseToken( 10, 11 ); // delete word9 at end of document
    }


    void TestDocument::testTokenSplit() {
	// split into 2
	Document doc;
	doc.newPage( "page1.tif" );
	doc.pushBackToken( L"splitsplit", true );

	size_t nrOfNewTokens = doc.at( 0 ).handleSplit( L"split split" );
	CPPUNIT_ASSERT_EQUAL( (size_t)2, nrOfNewTokens );
	CPPUNIT_ASSERT( doc.at( 0 ).getWDisplay() == L"split" );
	CPPUNIT_ASSERT( doc.at( 1 ).getWDisplay() == L" " );
	CPPUNIT_ASSERT( doc.at( 2 ).getWDisplay() == L"split" );

	CPPUNIT_ASSERT( doc.at( 0 ).isCorrected() );
	CPPUNIT_ASSERT( doc.at( 2 ).isCorrected() );
	std::wcout << "wOCR: " << doc.at( 2  ).getWOCR() << std::endl;
	std::wcout << "wDisplay: " << doc.at( 2  ).getWDisplay() << std::endl;

	// split into several
	doc.clear();
	doc.newPage( "page1.tif" );
	doc.pushBackToken( L"splitsplatsplutsplot", true );

	nrOfNewTokens = doc.at( 0 ).handleSplit( L"split splat splut splot" );

	CPPUNIT_ASSERT_EQUAL( (size_t)6, nrOfNewTokens );
	CPPUNIT_ASSERT( doc.at( 0 ).getWDisplay() == L"split" );
	CPPUNIT_ASSERT( doc.at( 1 ).getWDisplay() == L" " );
	CPPUNIT_ASSERT( doc.at( 2 ).getWDisplay() == L"splat" );
	CPPUNIT_ASSERT( doc.at( 3 ).getWDisplay() == L" " );
	CPPUNIT_ASSERT( doc.at( 4 ).getWDisplay() == L"splut" );
	CPPUNIT_ASSERT( doc.at( 5 ).getWDisplay() == L" " );
	CPPUNIT_ASSERT( doc.at( 6 ).getWDisplay() == L"splot" );

    }


    void TestDocument::testTokenMerge() {
	Document doc;
	doc.newPage( "page1.tif" );
	doc.pushBackToken( L"tok0", true );
	doc.pushBackToken( L" ", false );
	doc.pushBackToken( L"tok1", true );
	doc.pushBackToken( L" ", false );
	doc.pushBackToken( L"tok2", true );
	doc.pushBackToken( L",", false );
	doc.pushBackToken( L"tok3", true );
	doc.pushBackToken( L" ", false );
	doc.pushBackToken( L"tok4", true );
	doc.pushBackToken( L" ", false );
	doc.pushBackToken( L"tok5", true );
	doc.pushBackToken( L" ", false );
	doc.pushBackToken( L"tok6", true );
	doc.pushBackToken( L" ", false );
	doc.pushBackToken( L"tok7", true );
	doc.pushBackToken( L"\n", true );
	doc.pushBackToken( L"tok8", true );

	// merge <space> away
	doc.at( 0 ).mergeRight();
	CPPUNIT_ASSERT( std::wstring( L"tok0tok1" ) == doc.at( 0 ).getWDisplay() );
	CPPUNIT_ASSERT( std::wstring( L" " ) == doc.at( 1 ).getWDisplay() );
	CPPUNIT_ASSERT( std::wstring( L"tok2" ) == doc.at( 2 ).getWDisplay() );

	// merge with comma
	doc.at( 2 ).mergeRight();
	CPPUNIT_ASSERT( std::wstring( L"tok2," ) == doc.at( 2 ).getWDisplay() );
	CPPUNIT_ASSERT( std::wstring( L"tok3" ) ==  doc.at( 3 ).getWDisplay() );


	// merge with a specified nr of 4 tokens
	doc.at( 3 ).mergeRight( 4 );
	CPPUNIT_ASSERT( std::wstring( L"tok3tok4tok5" ) == doc.at( 3 ).getWDisplay() );
	CPPUNIT_ASSERT( std::wstring( L" " ) ==  doc.at( 4 ).getWDisplay() );
	CPPUNIT_ASSERT( std::wstring( L"tok6" ) ==  doc.at( 5 ).getWDisplay() );

	// stop merge at newline
	doc.at( 5 ).mergeRight( 5 );
	CPPUNIT_ASSERT( std::wstring( L"tok6tok7" ) == doc.at( 5 ).getWDisplay() );
	CPPUNIT_ASSERT( std::wstring( L"\n" ) ==  doc.at( 6 ).getWDisplay() );
	CPPUNIT_ASSERT( std::wstring( L"tok8" ) ==  doc.at( 7 ).getWDisplay() );





    }



    void TestDocument::testHyphenation() {
	Document doc;
	doc.newPage( "page1.tif" );
	doc.pushBackToken( L"der", true );
	doc.pushBackToken( L" ", false );
	doc.pushBackToken( L"zeilen", true );
	doc.pushBackToken( L"-", false ); // using "-"
	doc.pushBackToken( L"\n", false );
	doc.pushBackToken( L"umbruch", true );
	doc.pushBackToken( L"und", true );
	doc.pushBackToken( L" ", false );
	doc.pushBackToken( L"zeilen", true );
	doc.pushBackToken( L"\u00AC", false ); // using the "not" sign, unicode U+00AC
	doc.pushBackToken( L"\n", false );
	doc.pushBackToken( L"umbruch", true );
	doc.pushBackToken( L"und", true );
	doc.pushBackToken( L"zeilen", true );
	doc.pushBackToken( L"\u2E17", false );  // Double Oblique Hyphen
	doc.pushBackToken( L"\n", false );
	doc.pushBackToken( L"umbruch", true );


	doc.findHyphenation();
	CPPUNIT_ASSERT( doc.at( 2 ).hasProperty( Token::HYPHENATION_1ST ) );
	CPPUNIT_ASSERT( doc.at( 2 ).getHyphenationMerged() == L"zeilenumbruch" );
	CPPUNIT_ASSERT( doc.at( 3 ).hasProperty( Token::HYPHENATION_MARK ) );
	CPPUNIT_ASSERT( doc.at( 5 ).hasProperty( Token::HYPHENATION_2ND ) );
	CPPUNIT_ASSERT( doc.at( 5 ).getHyphenationMerged() == L"zeilenumbruch" );

	CPPUNIT_ASSERT( doc.at( 8 ).hasProperty( Token::HYPHENATION_1ST ) );
	CPPUNIT_ASSERT( doc.at( 8 ).getHyphenationMerged() == L"zeilenumbruch" );
	CPPUNIT_ASSERT( doc.at( 9 ).hasProperty( Token::HYPHENATION_MARK ) );
	CPPUNIT_ASSERT( doc.at( 11 ).hasProperty( Token::HYPHENATION_2ND ) );
	CPPUNIT_ASSERT( doc.at( 11 ).getHyphenationMerged() == L"zeilenumbruch" );

	CPPUNIT_ASSERT( doc.at( 13 ).hasProperty( Token::HYPHENATION_1ST ) );
	CPPUNIT_ASSERT( doc.at( 13 ).getHyphenationMerged() == L"zeilenumbruch" );
	CPPUNIT_ASSERT( doc.at( 14 ).hasProperty( Token::HYPHENATION_MARK ) );
	CPPUNIT_ASSERT( doc.at( 16 ).hasProperty( Token::HYPHENATION_2ND ) );
	CPPUNIT_ASSERT( doc.at( 16 ).getHyphenationMerged() == L"zeilenumbruch" );
    }

}
#endif
