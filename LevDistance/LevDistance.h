#ifndef CSL_LEVDISTANCE_H
#define CSL_LEVDISTANCE_H CSL_LEVDISTANCE_H




namespace csl {

    /**
     * @brief compute standard levenshtein distance
     */
    class LevDistance {
    public:
	LevDistance() {
	    matrixSize_ = 10;
	    resize( matrixSize_ );
	}

	inline size_t getLevDistance( std::wstring const& w1, std::wstring const& w2 ) {
	    if( w1.size() + 1 > matrixSize_ ) {
		resize( w1.size() + 1 );
	    }
	    if( w2.size() + 1 > matrixSize_ ) {
		resize( w2.size() + 1 );
	    }

	    clearMatrix();

	    // init
	    for( size_t i = 0; i <= w1.size(); ++i ) {
		matrix_.at( i ).at( 0 ) = i;
	    }
	    for( size_t i = 0; i <= w2.size(); ++i ) {
		matrix_.at( 0 ).at( i ) = i;
	    }


	    // fill matrix
	    for( size_t i = 1; i <= w1.size(); ++i ) {
		for( size_t j = 1; j <= w2.size(); ++j ) {
		    // match
		    if( w1.at( i -1 ) == w2.at( j -1 ) ) {
			matrix_.at( i ).at( j ) = matrix_.at( i-1 ).at( j-1 );
		    }
		    else {
			size_t min = matrix_.at( i-1 ).at( j-1 ) + 1;
			if( matrix_.at( i ).at( j-1 ) + 1 < min ) min = matrix_.at( i ).at( j-1 ) + 1;
			if( matrix_.at( i-1 ).at( j ) + 1 < min ) min = matrix_.at( i-1 ).at( j ) + 1;

			matrix_.at( i ).at( j ) = min;
		    }
		}
	    }	    

	    //printMatrix();
	    return matrix_.at( w1.size() ).at( w2.size() );

	}


	void printMatrix() const {
	    for( std::vector< std::vector< size_t > >::const_iterator row = matrix_.begin(); row != matrix_.end(); ++row ) {
		for( std::vector< size_t >::const_iterator cell = row->begin(); cell != row->end(); ++cell ) {
		    std::wcout << *cell << "\t";
		}
		std::wcout << std::endl;
	    }
	}

    private:

	void resize( size_t size ) {
	    matrix_.resize( size );
	    for( std::vector< std::vector< size_t > >::iterator row = matrix_.begin(); row != matrix_.end(); ++row ) {
		row->resize( size );
	    }
	    matrixSize_ = size;
	    clearMatrix();
	}

	void clearMatrix() {
	    for( std::vector< std::vector< size_t > >::iterator row = matrix_.begin(); row != matrix_.end(); ++row ) {
		for( std::vector< size_t >::iterator cell = row->begin(); cell != row->end(); ++cell ) {
		    *cell = 0;
		}
	    }
	}



	std::vector< std::vector< size_t > > matrix_;
	size_t matrixSize_;
	
    };

}

#endif
