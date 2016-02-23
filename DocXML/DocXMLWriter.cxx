#ifndef OCRC_DOCXMLWRITER_CXX
#define OCRC_DOCXMLWRITER_CXX OCRC_DOCXMLWRITER_CXX

#include "./DocXMLWriter.h"

namespace OCRCorrection {
    void DocXMLWriter::writeXML( Document const& doc, std::wostream& xml_out ) const {

	// at least try to make sure that the wostream is imbued with a decent locale
	if( ! ( std::use_facet< std::numpunct< wchar_t > >( xml_out.getloc() ).decimal_point() == '.' ) ) {
	    throw OCRCException( "OCRC::DocXMLWriter::writeXML: The specified wostream object uses a locale which does not use '.' as decimal point." );
	}

	xml_out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl
		<< "<document>" << std::endl
	    ;

	if( ! doc.hasPages() ) {
	    throw OCRCException( "OCRC::DocXMLWriter:;writeXML: This module works only for Document objects containing pages. This document has no page information." );
	}

	size_t tokenCount = 0;
	for( Document::const_PageIterator pageIt= doc.pagesBegin(); pageIt != doc.pagesEnd(); ++pageIt ) {
            xml_out << "<page imageFile=\"" << Utils::utf8(pageIt->getImageFile()) /*csl::CSLLocale::string2wstring( pageIt->getImageFile() )*/ << "\" sourceFile=\"" << Utils::utf8(pageIt->getSourceFile()) /*csl::CSLLocale::string2wstring( pageIt->getSourceFile() )*/<< "\">" <<std::endl;

	    for( Document::const_iterator token = pageIt->begin(); token != pageIt->end(); ++token ) {
		if( token->getWOCR() == L" " ) {
		    xml_out
			<< "<token token_id=\"" << tokenCount << "\" special_seq=\"space\" isNormal=\"false\">" << std::endl
		        << "<ext_id>" << token->getExternalId() << "</ext_id>" << std::endl
			<<  "<wOCR>" << xml_escape( token->getWOCR() ) << "</wOCR>" << std::endl
			<<  "<wOCR_lc>" << xml_escape( token->getWOCR_lc() ) << "</wOCR_lc>" << std::endl
			<<  "<wCorr>" << xml_escape( token->getWCorr() ) << "</wCorr>" << std::endl;

		    if( token->hasGroundtruth() ) {
			xml_out
			    << "<groundtruth verified=\"false\">" << std::endl
			    << "<classified>" << xml_escape( token->getGroundtruth().getClassified() ) << "</classified>" << std::endl
			    << "<wOrig>" << xml_escape( token->getGroundtruth().getWOrig() ) << "</wOrig>" << std::endl
			    << "<wOrig_lc>" << xml_escape( token->getGroundtruth().getWOrig_lc() ) << "</wOrig_lc>" << std::endl
			    << "</groundtruth>" << std::endl;
		    }
		    xml_out
			<< "</token>" << std::endl;
		}
		else if( token->getWOCR() == L"\n" ) {
		    xml_out
			<< "<token token_id=\"" << tokenCount << "\" special_seq=\"newline\" isNormal=\"false\">" << std::endl
		        << "<ext_id>" << token->getExternalId() << "</ext_id>" << std::endl
			<< "<wOCR>" << xml_escape( token->getWOCR() )<< "</wOCR>" << std::endl
			<< "<wOCR_lc>" << xml_escape( token->getWOCR_lc() ) << "</wOCR_lc>" << std::endl
			<< "<wCorr>" << xml_escape( token->getWCorr() ) << "</wCorr>" << std::endl;

		    if( token->hasGroundtruth() ) {
			xml_out
			    << "<groundtruth verified=\"false\">" << std::endl
			    << " <classified>" << xml_escape( token->getGroundtruth().getClassified() ) << "</classified>" << std::endl
			    << " <wOrig>" << xml_escape( token->getGroundtruth().getWOrig() ) << "</wOrig>" << std::endl
			    << " <wOrig_lc>" << xml_escape( token->getGroundtruth().getWOrig_lc() ) << "</wOrig_lc>" << std::endl
			    << "</groundtruth>" << std::endl;
		    }
		    xml_out	<< "</token>" << std::endl;
		}
		else {
		    std::wstring isNormal_string = (token->isNormal() )? L"true" : L"false";
		    xml_out
			<< "<token token_id=\"" << tokenCount << "\" isNormal=\""<< isNormal_string << "\">" << std::endl
		        << "<ext_id>" << token->getExternalId() << "</ext_id>" << std::endl
			<< "<wOCR>" << xml_escape( token->getWOCR() ) << "</wOCR>" << std::endl
			<< "<wOCR_lc>" << xml_escape( token->getWOCR_lc() ) << "</wOCR_lc>" << std::endl
			<< "<wCorr>" << xml_escape( token->getWCorr() ) << "</wCorr>" << std::endl;

		    for( Token::CandidateIterator cand = token->candidatesBegin();
			 cand != token->candidatesEnd();
			 ++cand ) {
			xml_out
			    <<  "<cand>"
			    << xml_escape( cand->toString() )
			    << "</cand>" << std::endl;
		    }

		    if( token->getTokenImageInfoBox() ) {
			xml_out
			    << "<coord l=\"" << token->getTokenImageInfoBox()->getCoordinate_Left() << "\""
			    << " t=\"" << token->getTokenImageInfoBox()->getCoordinate_Top() << "\""
			    << " r=\"" << token->getTokenImageInfoBox()->getCoordinate_Right() << "\""
			    << " b=\"" << token->getTokenImageInfoBox()->getCoordinate_Bottom() << "\""
			    << "/>"
			    << std::endl;
		    }

		    xml_out
			<< "<abbyy_suspicious value=\"" << ( token->getAbbyySpecifics().isSuspicious() ? L"true" : L"false" ) << "\"/>" << std::endl;

		    if( token->hasGroundtruth() ) {
			xml_out
			    << "<groundtruth verified=\"" << ( (token->getGroundtruth().getVerified())? L"true" : L"false" ) << "\">" << std::endl
			    << " <classified>" << xml_escape( token->getGroundtruth().getClassified() ) << "</classified>" << std::endl
			    << " <wOrig>" << xml_escape( token->getGroundtruth().getWOrig() ) << "</wOrig>" << std::endl
			    << " <wOrig_lc>" << xml_escape( token->getGroundtruth().getWOrig_lc() ) << "</wOrig_lc>" << std::endl
			    << " <baseWord>" << xml_escape( token->getGroundtruth().getBaseWord() ) << "</baseWord>" << std::endl
			    << " <histTrace>" << xml_escape( token->getGroundtruth().getHistTrace() ) << "</histTrace>" << std::endl
			    << " <ocrTrace>" << xml_escape( token->getGroundtruth().getOCRTrace() ) << "</ocrTrace>" << std::endl
			    << "</groundtruth>" << std::endl;
		    }
		    xml_out
			<< "</token>" << std::endl;
		}
		++tokenCount;
	    } // for all tokens of one page
	    xml_out << "</page>" << std::endl;
	} // for all pages
	xml_out << "</document>" << std::endl;
    }


    void DocXMLWriter::writeXML( Document const& doc, std::string const& filename ) const {
	std::wofstream of( filename.c_str() );
	//of.imbue( csl::CSLLocale::Instance() );
	if( ! of.good() ) {
	    throw OCRCException( "OCRC::DocXMLWriter::writeXML: Could not open file for writing" );
	}
	DocXMLWriter writer;
	writer.writeXML( doc, of );
	std::wcerr << "OCRC::DocXMLWriter::writeXML: xml written" << std::endl;

	of.close();
    }


}
#endif
