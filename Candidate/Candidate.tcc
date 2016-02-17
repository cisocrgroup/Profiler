#ifndef OCRCORRECTION_CANDIDATE_TCC
#define OCRCORRECTION_CANDIDATE_TCC OCRCORRECTION_CANDIDATE_TCC

/******************************************************/
/***********  IMPORTANT *******************************/
/*                                                    */
/*   DO NOT INCLUDE THIS HEADER FILE ANYWHERE         */
/*                                                    */
/*   Include Document.h instead                       */
/******************************************************/

namespace OCRCorrection {


    void Candidate::print( std::wostream& os ) const {
	os << getWord() << ":{" << getBaseWord() << "+";
	getInstruction().print( os );
	os << "}+ocr";
	getOCRTrace().print( os );
	os << ",voteWeight=" << getVoteWeight()
	   << ",levDistance=" << getLevDistance();
    }


    size_t Candidate::parseFromString( std::wstring const& str, size_t offset ) {
	size_t colon_and_bracket = str.find( L":{", offset );
	if( colon_and_bracket == str.npos ) throw OCRCException( "OCRC::Candidate::parseFromString: Found no colon_and_bracket" );
	setWord( str.substr( offset, colon_and_bracket - offset ) );

	offset = colon_and_bracket + 2;
	size_t plus = str.find( '+', offset );
	if( plus == str.npos ) throw OCRCException( "OCRC::Candidate::parseFromString: Found no plus" );
	setBaseWord( str.substr( offset, plus - colon_and_bracket - 2 ) );

	offset = plus + 1;

	offset = getInstruction().parseFromString( str, offset );
	if( str.substr( offset, 5 ) != L"}+ocr" ) throw OCRCException( "OCRC::Candidate::parseFromString: Found no '}+ocr'" );
	offset += 5;
	offset = getOCRTrace().parseFromString( str, offset );

	if( str.substr( offset, 12 ) != L",voteWeight=" ) throw OCRCException( "OCRC::Candidate::parseFromString: Found no ',voteWeight='" );
	offset += 12;

	size_t levDistancePos = str.find( L",levDistance=", offset );
	if( levDistancePos == str.npos ) throw OCRCException( "OCRC::Candidate::parseFromString: Found no ',levDistance='" );
	std::wstring voteWeightString = str.substr( offset, levDistancePos - offset );
	if( voteWeightString == L"nan" || voteWeightString == L"-nan" ) {
	    setVoteWeight( 0 );
	}
	else {
	    setVoteWeight( csl::CSLLocale::string2number< float >( voteWeightString ) );
	}

	offset = levDistancePos + 13;
	size_t end = str.find_first_not_of( L"0123456789.+-e", offset );
	if( end == str.npos ) end = str.size();
	setLevDistance( csl::CSLLocale::string2number< float >( str.substr( offset, end - offset ) ) );

	setBaseWordScore( -1 );

	return end;

    }



} // ns OCRCorrection


#endif
