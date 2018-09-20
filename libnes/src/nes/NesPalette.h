#ifndef NESPALETTE_H
#define NESPALETTE_H


#include "nes/NesColor.h"
#include "util/TStream.h"

namespace Nes {


/**
 * A 4-color NES palette.
 */
class NesPalette {
public:
  const static int size = 4;

  NesPalette();
  
  void read(BlackT::TStream& ifs);
  void write(BlackT::TStream& ofs) const;
  
  int numColors() const;
  
  NesColor color(int index) const;
  NesColor& colorRef(int index);
  void setColor(int index, NesColor color);
protected:
  const static int numColors_ = 4;
  
  NesColor colors_[numColors_];
  
};


};


#endif
