#ifndef CSL_TRANSTABLE_H
#define CSL_TRANSTABLE_H CSL_TRANSTABLE_H

#include "../Global.h"

namespace csl {
    typedef enum {
	TT_STANDARD,
	TT_PERFHASH
    } TransTableType;

    template< TransTableType tttype = TT_PERFHASH, typename InternalCharType__ = wchar_t, typename SizeType__ = uint32_t >
    class TransTable;
}

#include "./TransTable_standard.h"
#include "./TransTable_perfHash.h"

#endif
