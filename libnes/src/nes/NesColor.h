#ifndef NESCOLOR_H
#define NESCOLOR_H


#include "util/TByte.h"
#include "util/TColor.h"
#include "util/TStream.h"

namespace Nes {


/**
 * An NES hue/value format color.
 */
class NesColor {
public:
  const static int numNativeColors();

  NesColor();
  
  NesColor(BlackT::TByte nativeValue__);
  
  void read(BlackT::TStream& ifs);
  void write(BlackT::TStream& ofs) const;
  
  static BlackT::TByte maxNativeValue();
  
  BlackT::TByte nativeValue() const;
  void setNativeValue(BlackT::TByte nativeValue__);
  
  BlackT::TColor realColor() const;
  BlackT::TColor realColorTCRF() const;
  BlackT::TColor realColorBisquit() const;
  
protected:
  const static int standardBlack_ = 0x0F;

  const static int numNativeColors_ = 64;
  const static int nativeColorMask_ = 0x3F;
  const static BlackT::TColor nativeToRealColor_[numNativeColors_];
  const static BlackT::TColor nativeToRealColorTCRF_[numNativeColors_];
  const static BlackT::TColor nativeToRealColorBisquit_[numNativeColors_];
  
  static BlackT::TByte clipNativeValue(BlackT::TByte value);

  BlackT::TByte nativeValue_;
  
  
};


};


#endif
