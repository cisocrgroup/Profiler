#ifndef CSL_TRANSTABLE_CELL_H
#define CSL_TRANSTABLE_CELL_H CSL_TRANSTABLE_CELL_H


namespace csl {
    /**
     * Represents one unit of the sparse table implementation in TransTable  
     * \author Uli Reffle <uli@reffle.de>
     * \date Jan 2007
     */

    template< TransTableType tttype, typename InternalCharType__, typename SizeType__ >
    class Cell;

}

#include "./Cell_standard.h"
#include "./Cell_perfHash.h"


#endif
