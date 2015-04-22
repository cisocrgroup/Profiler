#ifndef OCRCORRECTION_TOKENIMAGEINFOBOX_H
#define OCRCORRECTION_TOKENIMAGEINFOBOX_H OCRCORRECTION_TOKENIMAGEINFOBOX_H

#include<string>

namespace OCRCorrection {

  /**
   * @brief Holds Coordinates and image file name for tokens
   * @author
   */

  class TokenImageInfoBox {

  public:

    TokenImageInfoBox();

    ~TokenImageInfoBox();
    
    size_t getCoordinate_Left() const;
    void setCoordinate_Left( size_t );

    size_t getCoordinate_Right() const;
    void setCoordinate_Right( size_t );
    
    size_t getCoordinate_Top() const;
    void setCoordinate_Top( size_t );
    
    size_t getCoordinate_Bottom() const;
    void setCoordinate_Bottom( size_t );

  private:

    size_t coordinate_left_;
    size_t coordinate_right_;
    size_t coordinate_top_;
    size_t coordinate_bottom_;
    
  };
}
#endif
