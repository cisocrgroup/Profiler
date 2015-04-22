#ifndef OCRCORRECTION_TOKENIMAGEINFOBOX_TCC
#define OCRCORRECTION_TOKENIMAGEINFOBOX_TCC OCRCORRECTION_TOKENIMAGEINFOBOX_TCC

#include "./TokenImageInfoBox.h"

namespace OCRCorrection {
  
  TokenImageInfoBox::TokenImageInfoBox() : 
    coordinate_left_ (-2),
    coordinate_right_ (-2),
    coordinate_top_ (-2),
    coordinate_bottom_(-2)
  { }

  TokenImageInfoBox::~TokenImageInfoBox() { 
  }


  size_t TokenImageInfoBox::getCoordinate_Left() const {
    return coordinate_left_;
  }
  void TokenImageInfoBox::setCoordinate_Left( size_t i ) {
    coordinate_left_ = i;
  }
  
  size_t TokenImageInfoBox::getCoordinate_Right() const {
    return coordinate_right_;
  }
  void TokenImageInfoBox::setCoordinate_Right( size_t i ) {
    coordinate_right_ = i;
  }
  
  size_t TokenImageInfoBox::getCoordinate_Top() const {
    return coordinate_top_;
  }
  void TokenImageInfoBox::setCoordinate_Top( size_t i ) {
    coordinate_top_ = i;
  }
  
  size_t TokenImageInfoBox::getCoordinate_Bottom() const {
    return coordinate_bottom_;
  }
  void TokenImageInfoBox::setCoordinate_Bottom( size_t i ) {
    coordinate_bottom_ = i;
  }
  
} // ns OCRCorrection
#endif
