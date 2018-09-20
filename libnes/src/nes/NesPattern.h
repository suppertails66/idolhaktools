#ifndef NESPATTERN_H
#define NESPATTERN_H


#include "util/TByte.h"
#include "util/TTwoDByteArray.h"
#include "util/TGraphic.h"
#include "util/TStream.h"
#include "nes/NesPalette.h"

namespace Nes {


class NesPattern {
public:
  const static int width = 8;
  const static int height = 8;
  const static int w = width;
  const static int h = height;
  const static int size = 16;
  
  NesPattern();
  
  bool operator==(const NesPattern& other) const;
  bool operator!=(const NesPattern& other) const;
  
  int fromUncompressedData(const BlackT::TByte* src);
  
  int toUncompressedData(BlackT::TByte* dst) const;
  
  void read(BlackT::TStream& ifs);
  void write(BlackT::TStream& ofs) const;
  
  BlackT::TByte data(int x, int y) const;
  
  void setData(int x, int y, BlackT::TByte d);
  
  void drawGrayscale(BlackT::TGraphic& dst,
                     int xOffset, int yOffset,
                     bool transparency = false) const;
  
  void drawGrayscale(BlackT::TGraphic& dst,
                     bool transparency = false) const;
  
  void fromGrayscaleGraphic(const BlackT::TGraphic& src,
                     int xOffset = 0, int yOffset = 0);
  
  void fromPalettizedGraphic(const BlackT::TGraphic& src,
                     const NesPalette& pal,
                     int xOffset = 0, int yOffset = 0);
  
  void drawPalettized(BlackT::TGraphic& dst,
                      int xOffset, int yOffset,
                      const NesPalette& src,
                      bool transparency = false) const;
  
  void drawPalettized(BlackT::TGraphic& dst,
                      const NesPalette& src,
                      bool transparency = false) const;
  
  void swapColors(BlackT::TByte first, BlackT::TByte second);
protected:
  const static int planeByteOffset_ = 8;

  BlackT::TTwoDByteArray data_;
};


};


#endif
