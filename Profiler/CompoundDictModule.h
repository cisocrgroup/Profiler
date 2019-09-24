#include<TransTable/TransTable.h>
#include<Vaam/Vaam.h>
#include<DictSearch/DictSearch.h>


class CompoundDictModule : public csl::DictSearch::iDictModule {
private:
    typedef csl::TransTable< csl::TT_PERFHASH, uint16_t, uint32_t > TransTable_t;

public:
    CompoundDictModule( std::string const& dicFile ) :
	name_( L"compound" ),
	compoundVaam_( 0 ),
	priority_( 1 ) {

	compoundDic_.loadFromFile( dicFile.c_str() );

	compoundVaam_ = new csl::Vaam< TransTable_t >( compoundDic_, "/mounts/data/proj/impact/software/Vaam/patterns.lexgui.txt" );
	compoundVaam_->setMaxNrOfPatterns( 0 );
	compoundVaam_->setDistance( 0 );
    }

    virtual bool query( std::wstring const& query, csl::DictSearch::iResultReceiver* answers ) {

	// query the compound dictionary including variants
	return compoundVaam_->query( query, answers );
    }

    std::wstring const& getName() const {
	return name_;
    }

    int getPriority() const {
	return priority_;
    }

    void setPriority( int p ) {
	priority_ = p;
    }

private:
    std::wstring name_;
    TransTable_t compoundDic_;
    csl::Vaam< TransTable_t >* compoundVaam_;
    int priority_;
};
