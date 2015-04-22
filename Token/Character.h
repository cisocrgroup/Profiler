#ifndef OCRCORRECTION_CHARACTER_H
#define OCRCORRECTION_CHARACTER_H OCRCORRECTION_CHARACTER_H

namespace OCRCorrection {
    
    /**
     * @brief represents one character in the OCRC Document model.
     *        
     */
    class Character {
    public:
	/**
	 * @brief This constructor initialises all coordinates to 0
	 */
	Character() :
	    c_( 0 ),
	    left_( 0 ),
	    top_( 0 ),
	    right_( 0 ),
	    bottom_( 0 ),
	    isAbbyySuspicious_( false )
	    {
	}

	/**
	 * @brief This constructor initialises all coordinates and the suspicious flag according to the given values
	 */
	Character( wchar_t c, size_t left, size_t top, size_t right, size_t bottom, bool isAbbyySuspicious = false ) :
	    c_( c ),
	    left_( left ),
	    top_( top ),
	    right_( right ),
	    bottom_( bottom ),
	    isAbbyySuspicious_( isAbbyySuspicious )
	    {

	}

	/**
	 * @brief sets or un-sets the suspicious flag
	 */
	void setSuspicious( bool b = true ) {
	    isAbbyySuspicious_ = b;
	}

	/**
	 * @brief returns true iff the character is marked as suspicious
	 **/
	bool isAbbyySuspicious() const {
	    return isAbbyySuspicious_;
	}

	/**
	 * @brief returns the plain character
	 */
	wchar_t getChar() const {
	    return c_;
	}

       
	/**
	 * @brief returns the left coordinate
	 */
	size_t getLeft() const { return left_; }

	/**
	 * @brief returns the right coordinate
	 */
	size_t getRight() const { return right_; }

	/**
	 * @brief returns the top coordinate
	 */
	size_t getTop() const { return top_; }

	/**
	 * @brief returns the bottom coordinate
	 */
	size_t getBottom() const { return bottom_; }
	

    private:
	wchar_t c_;
	size_t left_, top_, right_, bottom_;
	bool isAbbyySuspicious_;
    };
}

#endif
