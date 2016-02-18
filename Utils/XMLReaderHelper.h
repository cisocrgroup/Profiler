#ifndef OCRC_XMLREADERHELPER_H
#define OCRC_XMLREADERHELPER_H OCRC_XMLREADERHELPER_H

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>

namespace OCRCorrection {

    class XMLReaderHelper {
    public:

	XMLReaderHelper() :
	    externalHandler_( 0 )
	    {
	    }

	void enter( XMLCh const* initName ) {
	    char* name_cstr = xercesc::XMLString::transcode(initName);
	    stack_.push_back( name_cstr );
	    xercesc::XMLString::release( &name_cstr );


	}


	void leave( XMLCh const* initName ) {
	    char* name_cstr = xercesc::XMLString::transcode(initName);

	    if( stack_.empty() ) {
	    	throw OCRCException( std::string( "OCRC::PatternContainerXMLReader::unexpected closing tag (depth=0) " )+ name_cstr );
	    }

	    if( strcmp( name().c_str(), name_cstr ) ) {
		throw OCRCException( std::string( "OCRC::PatternContainerXMLReader::unexpected closing tag " )+ name_cstr );
	    }

	    xercesc::XMLString::release( &name_cstr );


	    if( externalHandlerDepth_ == depth() ) {
		externalHandler_ = 0;
		externalHandlerDepth_ = (size_t)-1;
	    }


	    stack_.resize( stack_.size() - 1 );
	}

	/**
	 * @brief returns true iff a container with the given name is currently open.
	 */
	bool hasAncestor( std::string const& container ) const {
	    return ( std::find( stack_.begin(), stack_.end(), container ) != stack_.end() );
	}

	/**
	 * @brief returns true iff a container with the given name is the direct parent of the currently open container.
	 */
	bool hasParent( std::string const& container ) const {
	    return ( stack_.at( stack_.size() - 2 ) == container );
	}

	std::string const& name() const {
	    return stack_.back();
	}

	size_t depth() const {
	    return stack_.size();
	}

	bool empty() const {
	    return stack_.empty();
	}

	void setExternalHandler( xercesc::HandlerBase* externalHandler ) {
	    externalHandler_ = externalHandler;
	    externalHandlerDepth_ = depth();
	}

	xercesc::HandlerBase* getExternalHandler() {
	    return externalHandler_;
	}


	static bool hasAttribute( xercesc::AttributeList& attrs, std::string const& name ) {
	    return ( attrs.getValue( name.c_str() ) != 0 );
	}

	/**
	 * @param[in] name  name of the attribute
	 * @param[in] attr  the attributeList as provided by the xerces parser
	 * @return    a copy(!) of the specified attribute value as std::wstring
	 *
	 * This involves a lot of copying of strings and is quite un-efficient...
	 *
	 */
	static std::wstring getAttributeWideValue( xercesc::AttributeList& attrs, std::string const& name ) {
	    char* value_cstr = xercesc::XMLString::transcode( attrs.getValue( name.c_str() ) );
	    std::wstring value = Utils::utf8(value_cstr);
	    //csl::CSLLocale::string2wstring( value_cstr, value );
	    xercesc::XMLString::release( &value_cstr );
	    return value;
	}

	static std::string getAttributeValue( xercesc::AttributeList& attrs, std::string const& name ) {
	    if( ! hasAttribute( attrs, name ) ) {
		throw OCRCException( std::string( "OCRC::XMLReaderHelper::getAttributeValue: no such attribute: " ) + name );
	    }
	    char* value_cstr = xercesc::XMLString::transcode( attrs.getValue( name.c_str() ) );
	    std::string value( value_cstr );
	    xercesc::XMLString::release( &value_cstr );
	    return value;
	}




    private:
	std::vector< std::string > stack_;

	xercesc::HandlerBase* externalHandler_;

	/**
	 * @brief Specifies the depth where the externalHandler was introduced
	 */
	size_t externalHandlerDepth_;
    };

}


#endif
