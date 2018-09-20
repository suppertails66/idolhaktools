#include "nes/NesPalette.h"
#include "exception/OutOfRangeIndexException.h"

namespace Nes {


NesPalette::NesPalette() { };

void NesPalette::read(BlackT::TStream& ifs) {
  for (int i = 0; i < numColors_; i++) {
    colors_[i].read(ifs);
  }
}

void NesPalette::write(BlackT::TStream& ofs) const {
  for (int i = 0; i < numColors_; i++) {
    colors_[i].write(ofs);
  }
}
  
int NesPalette::numColors() const {
  return numColors_;
}

NesColor NesPalette::color(int index) const {
  if ((index < 0) || (index >= numColors_)) {
    throw OutOfRangeIndexException(T_SRCANDLINE,
                                   "NesPalette::color(int)",
                                   index);
  }

  return colors_[index];
}

NesColor& NesPalette::colorRef(int index) {
  if ((index < 0) || (index >= numColors_)) {
    throw OutOfRangeIndexException(T_SRCANDLINE,
                                   "NesPalette::color(int)",
                                   index);
  }

  return colors_[index];
}

void NesPalette::setColor(int index, NesColor color) {
  if ((index < 0) || (index >= numColors_)) {
    throw OutOfRangeIndexException(T_SRCANDLINE,
                                   "NesPalette::setColor(int,NesColor)",
                                   index);
  }
  
  colors_[index] = color;
}


};
