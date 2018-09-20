#include "nes/NesColor.h"

namespace Nes {


NesColor::NesColor()
  : nativeValue_(standardBlack_) { };

NesColor::NesColor(BlackT::TByte nativeValue__)
  : nativeValue_(clipNativeValue(nativeValue__)) { };

void NesColor::read(BlackT::TStream& ifs) {
  nativeValue_ = ifs.readu8le();
}

void NesColor::write(BlackT::TStream& ofs) const {
  ofs.writeu8le(nativeValue_);
}
  
const int NesColor::numNativeColors() {
  return numNativeColors_;
}
  
BlackT::TByte NesColor::maxNativeValue() {
  return numNativeColors_;
}

BlackT::TByte NesColor::nativeValue() const {
  return nativeValue_;
}

void NesColor::setNativeValue(BlackT::TByte nativeValue__) {
  nativeValue_ = clipNativeValue(nativeValue__);
}

BlackT::TColor NesColor::realColor() const {
  return nativeToRealColor_[nativeValue_];
}

BlackT::TColor NesColor::realColorTCRF() const {
  return nativeToRealColorTCRF_[nativeValue_];
}

BlackT::TColor NesColor::realColorBisquit() const {
  return nativeToRealColorBisquit_[nativeValue_];
}
  
BlackT::TByte NesColor::clipNativeValue(BlackT::TByte value) {
  return (value & nativeColorMask_);
}
  
const BlackT::TColor NesColor::nativeToRealColor_[numNativeColors_] =
  // FCEUX default palette
  {
    BlackT::TColor(116, 116, 116), 
    BlackT::TColor(36, 24, 140), 
    BlackT::TColor(0, 0, 168), 
    BlackT::TColor(68, 0, 156), 
    BlackT::TColor(140, 0, 116), 
    BlackT::TColor(168, 0, 16), 
    BlackT::TColor(164, 0, 0), 
    BlackT::TColor(124, 8, 0), 
    BlackT::TColor(64, 44, 0), 
    BlackT::TColor(0, 68, 0), 
    BlackT::TColor(0, 80, 0), 
    BlackT::TColor(0, 60, 20), 
    BlackT::TColor(24, 60, 92), 
    BlackT::TColor(0, 0, 0), 
    BlackT::TColor(0, 0, 0), 
    BlackT::TColor(0, 0, 0), 
    BlackT::TColor(188, 188, 188), 
    BlackT::TColor(0, 112, 236), 
    BlackT::TColor(32, 56, 236), 
    BlackT::TColor(128, 0, 240), 
    BlackT::TColor(188, 0, 188), 
    BlackT::TColor(228, 0, 88), 
    BlackT::TColor(216, 40, 0), 
    BlackT::TColor(200, 76, 12), 
    BlackT::TColor(136, 112, 0), 
    BlackT::TColor(0, 148, 0), 
    BlackT::TColor(0, 168, 0), 
    BlackT::TColor(0, 144, 56), 
    BlackT::TColor(0, 128, 136), 
    BlackT::TColor(0, 0, 0), 
    BlackT::TColor(0, 0, 0), 
    BlackT::TColor(0, 0, 0), 
    BlackT::TColor(252, 252, 252), 
    BlackT::TColor(60, 188, 252), 
    BlackT::TColor(92, 148, 252), 
    BlackT::TColor(204, 136, 252), 
    BlackT::TColor(244, 120, 252), 
    BlackT::TColor(252, 116, 180), 
    BlackT::TColor(252, 116, 96), 
    BlackT::TColor(252, 152, 56), 
    BlackT::TColor(240, 188, 60), 
    BlackT::TColor(128, 208, 16), 
    BlackT::TColor(76, 220, 72), 
    BlackT::TColor(88, 248, 152), 
    BlackT::TColor(0, 232, 216), 
    BlackT::TColor(120, 120, 120), 
    BlackT::TColor(0, 0, 0), 
    BlackT::TColor(0, 0, 0), 
    BlackT::TColor(252, 252, 252), 
    BlackT::TColor(168, 228, 252), 
    BlackT::TColor(196, 212, 252), 
    BlackT::TColor(212, 200, 252), 
    BlackT::TColor(252, 196, 252), 
    BlackT::TColor(252, 196, 216), 
    BlackT::TColor(252, 188, 176), 
    BlackT::TColor(252, 216, 168), 
    BlackT::TColor(252, 228, 160), 
    BlackT::TColor(224, 252, 160), 
    BlackT::TColor(168, 240, 188), 
    BlackT::TColor(176, 252, 204), 
    BlackT::TColor(156, 252, 240), 
    BlackT::TColor(196, 196, 196), 
    BlackT::TColor(0, 0, 0), 
    BlackT::TColor(0, 0, 0)
  };
  
const BlackT::TColor NesColor::nativeToRealColorTCRF_[numNativeColors_] =
  // "TCRF" palette
  {
    BlackT::TColor(82, 82, 82), 
    BlackT::TColor(1, 26, 81), 
    BlackT::TColor(15, 15, 101), 
    BlackT::TColor(35, 6, 99), 
    BlackT::TColor(54, 3, 75), 
    BlackT::TColor(64, 4, 38), 
    BlackT::TColor(63, 9, 4), 
    BlackT::TColor(50, 19, 0), 
    BlackT::TColor(31, 32, 0), 
    BlackT::TColor(11, 42, 0), 
    BlackT::TColor(0, 47, 0), 
    BlackT::TColor(0, 46, 10), 
    BlackT::TColor(0, 38, 45), 
    BlackT::TColor(0, 0, 0), 
    BlackT::TColor(0, 0, 0), 
    BlackT::TColor(0, 0, 0), 
    BlackT::TColor(160, 160, 160), 
    BlackT::TColor(30, 74, 157), 
    BlackT::TColor(56, 55, 188), 
    BlackT::TColor(88, 40, 184), 
    BlackT::TColor(117, 33, 148), 
    BlackT::TColor(132, 35, 92), 
    BlackT::TColor(130, 46, 36), 
    BlackT::TColor(111, 63, 0), 
    BlackT::TColor(81, 82, 0), 
    BlackT::TColor(49, 99, 0), 
    BlackT::TColor(26, 107, 5), 
    BlackT::TColor(14, 105, 46), 
    BlackT::TColor(16, 92, 104), 
    BlackT::TColor(0, 0, 0), 
    BlackT::TColor(0, 0, 0), 
    BlackT::TColor(0, 0, 0), 
    BlackT::TColor(254, 255, 255), 
    BlackT::TColor(105, 158, 252), 
    BlackT::TColor(137, 135, 255), 
    BlackT::TColor(174, 118, 255), 
    BlackT::TColor(206, 109, 241), 
    BlackT::TColor(224, 112, 178), 
    BlackT::TColor(222, 124, 112), 
    BlackT::TColor(200, 145, 62), 
    BlackT::TColor(166, 167, 37), 
    BlackT::TColor(129, 186, 40), 
    BlackT::TColor(99, 196, 70), 
    BlackT::TColor(84, 193, 125), 
    BlackT::TColor(86, 179, 192), 
    BlackT::TColor(60, 60, 60), 
    BlackT::TColor(0, 0, 0), 
    BlackT::TColor(0, 0, 0), 
    BlackT::TColor(254, 255, 255), 
    BlackT::TColor(190, 214, 253), 
    BlackT::TColor(204, 204, 255), 
    BlackT::TColor(221, 196, 255), 
    BlackT::TColor(234, 192, 249), 
    BlackT::TColor(242, 193, 223), 
    BlackT::TColor(241, 199, 194), 
    BlackT::TColor(232, 208, 170), 
    BlackT::TColor(217, 218, 157), 
    BlackT::TColor(201, 226, 158), 
    BlackT::TColor(188, 230, 174), 
    BlackT::TColor(180, 229, 199), 
    BlackT::TColor(181, 223, 228), 
    BlackT::TColor(169, 169, 169), 
    BlackT::TColor(0, 0, 0), 
    BlackT::TColor(0, 0, 0)
  };

const BlackT::TColor NesColor::nativeToRealColorBisquit_[numNativeColors_] =
  // Color values generated by Bisqwit's NES color generator
  {
    BlackT::TColor(82, 82, 82), 
    BlackT::TColor(1, 26, 81), 
    BlackT::TColor(15, 15, 101), 
    BlackT::TColor(35, 6, 99), 
    BlackT::TColor(54, 3, 75), 
    BlackT::TColor(64, 4, 38), 
    BlackT::TColor(63, 9, 4), 
    BlackT::TColor(50, 19, 0), 
    BlackT::TColor(31, 32, 0), 
    BlackT::TColor(11, 42, 0), 
    BlackT::TColor(0, 47, 0), 
    BlackT::TColor(0, 46, 10), 
    BlackT::TColor(0, 38, 45), 
    BlackT::TColor(0, 0, 0), 
    BlackT::TColor(0, 0, 0), 
    BlackT::TColor(0, 0, 0), 
    BlackT::TColor(160, 160, 160), 
    BlackT::TColor(30, 74, 157), 
    BlackT::TColor(56, 55, 188), 
    BlackT::TColor(88, 40, 184), 
    BlackT::TColor(117, 33, 148), 
    BlackT::TColor(132, 35, 92), 
    BlackT::TColor(130, 46, 36), 
    BlackT::TColor(111, 63, 0), 
    BlackT::TColor(81, 82, 0), 
    BlackT::TColor(49, 99, 0), 
    BlackT::TColor(26, 107, 5), 
    BlackT::TColor(14, 105, 46), 
    BlackT::TColor(16, 92, 104), 
    BlackT::TColor(0, 0, 0), 
    BlackT::TColor(0, 0, 0), 
    BlackT::TColor(0, 0, 0), 
    BlackT::TColor(254, 255, 255), 
    BlackT::TColor(105, 158, 252), 
    BlackT::TColor(137, 135, 255), 
    BlackT::TColor(174, 118, 255), 
    BlackT::TColor(206, 109, 241), 
    BlackT::TColor(224, 112, 178), 
    BlackT::TColor(222, 124, 112), 
    BlackT::TColor(200, 145, 62), 
    BlackT::TColor(166, 167, 37), 
    BlackT::TColor(129, 186, 40), 
    BlackT::TColor(99, 196, 70), 
    BlackT::TColor(84, 193, 125), 
    BlackT::TColor(86, 179, 192), 
    BlackT::TColor(60, 60, 60), 
    BlackT::TColor(0, 0, 0), 
    BlackT::TColor(0, 0, 0), 
    BlackT::TColor(254, 255, 255), 
    BlackT::TColor(190, 214, 253), 
    BlackT::TColor(204, 204, 255), 
    BlackT::TColor(221, 196, 255), 
    BlackT::TColor(234, 192, 249), 
    BlackT::TColor(242, 193, 223), 
    BlackT::TColor(241, 199, 194), 
    BlackT::TColor(232, 208, 170), 
    BlackT::TColor(217, 218, 157), 
    BlackT::TColor(201, 226, 158), 
    BlackT::TColor(188, 230, 174), 
    BlackT::TColor(180, 229, 199), 
    BlackT::TColor(181, 223, 228), 
    BlackT::TColor(169, 169, 169), 
    BlackT::TColor(0, 0, 0), 
    BlackT::TColor(0, 0, 0)
  };
  
  // Table values taken from nesdev who took them from blargg who
  // made them up or something
/*  {
    BlackT::TColor(84, 84, 84, BlackT::TColor::fullAlphaOpacity),
    BlackT::TColor(0, 30, 116, BlackT::TColor::fullAlphaOpacity),
    BlackT::TColor(8, 16, 144, BlackT::TColor::fullAlphaOpacity),
    BlackT::TColor(48, 0, 136, BlackT::TColor::fullAlphaOpacity),
    BlackT::TColor(68, 0, 100, BlackT::TColor::fullAlphaOpacity),
    BlackT::TColor(92, 0, 48, BlackT::TColor::fullAlphaOpacity),
    BlackT::TColor(0, 48, 84, BlackT::TColor::fullAlphaOpacity),
    BlackT::TColor(84, 4, 0, BlackT::TColor::fullAlphaOpacity),
    BlackT::TColor(60, 24, 0, BlackT::TColor::fullAlphaOpacity),
    BlackT::TColor(32, 42, 0, BlackT::TColor::fullAlphaOpacity),
    BlackT::TColor(8, 58, 0, BlackT::TColor::fullAlphaOpacity),
    BlackT::TColor(0, 64, 0, BlackT::TColor::fullAlphaOpacity),
    BlackT::TColor(0, 60, 0, BlackT::TColor::fullAlphaOpacity),
    BlackT::TColor(0, 50, 60, BlackT::TColor::fullAlphaOpacity),
    BlackT::TColor(0, 0, 0, BlackT::TColor::fullAlphaOpacity),
    BlackT::TColor(0, 0, 0, BlackT::TColor::fullAlphaOpacity),
    BlackT::TColor(0, 0, 0, BlackT::TColor::fullAlphaOpacity),
    
    BlackT::TColor(152, 150, 152, BlackT::TColor::fullAlphaOpacity),
    BlackT::TColor(8, 76, 116, BlackT::TColor::fullAlphaOpacity),
    BlackT::TColor(48, 50, 144, BlackT::TColor::fullAlphaOpacity),
    BlackT::TColor(92, 30, 136, BlackT::TColor::fullAlphaOpacity),
    BlackT::TColor(136, 20, 100, BlackT::TColor::fullAlphaOpacity),
    BlackT::TColor(160, 20, 48, BlackT::TColor::fullAlphaOpacity),
    BlackT::TColor(152, 34, 84, BlackT::TColor::fullAlphaOpacity),
    BlackT::TColor(120, 60, 0, BlackT::TColor::fullAlphaOpacity),
    BlackT::TColor(84, 90, 0, BlackT::TColor::fullAlphaOpacity),
    BlackT::TColor(40, 114, 0, BlackT::TColor::fullAlphaOpacity),
    BlackT::TColor(8, 124, 0, BlackT::TColor::fullAlphaOpacity),
    BlackT::TColor(0, 118, 0, BlackT::TColor::fullAlphaOpacity),
    BlackT::TColor(0, 102, 0, BlackT::TColor::fullAlphaOpacity),
    BlackT::TColor(0, 0, 0, BlackT::TColor::fullAlphaOpacity),
    BlackT::TColor(0, 0, 0, BlackT::TColor::fullAlphaOpacity),
    BlackT::TColor(0, 0, 0, BlackT::TColor::fullAlphaOpacity),
  }; */


};
