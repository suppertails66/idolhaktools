#ifndef NESTILEMAP_H
#define NESTILEMAP_H


#include "util/TByte.h"
#include "util/TTwoDByteArray.h"
#include "util/TGraphic.h"
#include "util/TStream.h"
#include "nes/NesPalette.h"
#include "nes/NesPatternTable.h"

namespace Nes {


class NesTilemap {
public:
  NesTilemap();
  NesTilemap(int width__, int height__);
  
  bool operator==(const NesTilemap& other) const;
  bool operator!=(const NesTilemap& other) const;
  
  void read(BlackT::TStream& ifs, int width__, int height__);
  void write(BlackT::TStream& ofs) const;
  
  int data(int x, int y) const;
  void setData(int x, int y, int d);
  
  int w() const;
  int h() const;
  
  void resize(int width__, int height__);
  
  void drawGrayscale(BlackT::TGraphic& dst,
                       const NesPatternTable& patternTable,
                       int xOffset, int yOffset,
                       bool transparency = false) const;
  
/*  void drawGrayscale(BlackT::TGraphic& dst,
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
  
  void swapColors(BlackT::TByte first, BlackT::TByte second); */
protected:
//  const static int planeByteOffset_ = 8;

  BlackT::TTwoDArray<int> data_;
};


};


#endif
