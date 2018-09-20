#ifndef NESCOLORDATA_H
#define NESCOLORDATA_H


#include "nes/NesPaletteQuad.h"
#include "util/TByte.h"

namespace Nes {


class NesColorData {
public:
  const static int size = NesPaletteQuad::size * 2;
  const static int numBackgroundColors = 16;
  const static int numSpriteColors = 16;

  NesColorData();
  
  NesPaletteQuad& backgroundPalettes();
  NesPaletteQuad& spritePalettes();
  
  const NesPaletteQuad& backgroundPalettes() const;
  const NesPaletteQuad& spritePalettes() const;
  
  void readFullPaletteSet(const BlackT::TByte* src);
  
  int readFromData(const BlackT::TByte* src);
  int writeToData(BlackT::TByte* dst) const;
  
  NesColor color(int index) const;
  void setColor(int index, NesColor color);
protected:
  NesPaletteQuad backgroundPalettes_;
  NesPaletteQuad spritePalettes_;
  
  void readPaletteQuad(NesPaletteQuad& dst,
                   const BlackT::TByte* src);
};


};


#endif
