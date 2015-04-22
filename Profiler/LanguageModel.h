

#include<csl/DictSearch/DictSearch.h>


class LanguageModel {
public:
    LanguageModel() {
    }

    
    float getScore( csl::DictSearch::Interpretation ipt ) {
	return 0.5;
    }

    

private:
//    csl::MinDic< int >& histFreq_;
//    csl::MinDic< int >& modernFreq_;

};
