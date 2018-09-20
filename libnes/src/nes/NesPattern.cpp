#include "nes/NesPattern.h"
#include "util/TStringConversion.h"
#include "exception/TGenericException.h"
#include <iostream>

using namespace BlackT;

namespace Nes {


NesPattern::NesPattern()
  : data_(width, height) { };

bool NesPattern::operator==(const NesPattern& other) const {
  for (int j = 0; j < height; j++) {
    for (int i = 0; i < width; i++) {
      if (data_.data(i, j) != other.data_.data(i, j)) return false;
    }
  }
  
  return true;
}

bool NesPattern::operator!=(const NesPattern& other) const {
  return (!(*this == other));
}

BlackT::TByte NesPattern::data(int x, int y) const {
  return data_.data(x, y);
}
  
void NesPattern::setData(int x, int y, BlackT::TByte d) {
  data_.setDataClipped(x, y, d);
}
  
int NesPattern::fromUncompressedData(const BlackT::TByte* src) {

  for (int j = 0; j < height; j++) {
    // Row data from plane 1 (low bit)
    BlackT::TByte b1 = *src;
    // Row data from plane 2 (high bit)
    BlackT::TByte b2 = *(src + planeByteOffset_);
    
    for (int i = 0; i < width; i++) {
      BlackT::TByte d = 0;
      
      int shift = (width - i - 1);
      
      BlackT::TByte mask = (0x01 << shift);
      
      // Extract bits from planes and combine into a single byte
      d |= (b1 & mask) >> shift;
      
      // negative shift widths aren't supported? really?
      if (shift > 0) {
        d |= (b2 & mask) >> (shift - 1);
      }
      else {
        d |= (b2 & mask) << -(shift - 1);
      }
      
      data_.data(i, j) = d;
    }
    
    ++src;
  }
  
  return size;
}
  
int NesPattern::toUncompressedData(BlackT::TByte* dst) const {

  for (int j = 0; j < height; j++) {
    // Row data for plane 1 (low bit)
    BlackT::TByte* b1 = dst;
    // Row data for plane 2 (high bit)
    BlackT::TByte* b2 = dst + planeByteOffset_;
    
    (*b1) = 0;
    (*b2) = 0;
    
    for (int i = 0; i < width; i++) {
      BlackT::TByte d = data_.data(i, j);
      
      int shift = (width - i - 1);
      
      BlackT::TByte mask = (0x01 << shift);
      
      if ((d & 0x01) != 0) {
        (*b1) |= mask;
      }
      
      if ((d & 0x02) != 0) {
        (*b2) |= mask;
      }
    }
    
    ++dst;
  }
  
  return size;
}
  
void NesPattern::read(BlackT::TStream& ifs) {
  char buffer[size];
  ifs.read(buffer, size);
  fromUncompressedData((TByte*)buffer);
}

void NesPattern::write(BlackT::TStream& ofs) const {
  char buffer[size];
  toUncompressedData((TByte*)buffer);
  ofs.write(buffer, size);
}
  
void NesPattern::drawGrayscale(BlackT::TGraphic& dst,
                            int xOffset, int yOffset,
                            bool transparency) const {
//  dst.clear(BlackT::TColor(0, 0, 0, BlackT::TColor::fullAlphaOpacity));
  
  for (int j = 0; j < height; j++) {
    for (int i = 0; i < width; i++) {
      BlackT::TByte c = data_.data(i, j) << 6;
      
      BlackT::TColor realcolor(c, c, c, BlackT::TColor::fullAlphaOpacity);
      
      if (transparency && (c == 0)) {
        realcolor.setA(BlackT::TColor::fullAlphaTransparency);
      }
      
      // background?
      // ...
      
      dst.setPixel(i + xOffset, j + yOffset,
                   realcolor);
    }
  }
}
  
void NesPattern::drawGrayscale(BlackT::TGraphic& dst,
                            bool transparency) const {
  if ((dst.w() != width) || (dst.h() != height)) {
    dst = BlackT::TGraphic(width, height);
  }
  
  drawGrayscale(dst, 0, 0, transparency);
}
  
void NesPattern::fromGrayscaleGraphic(const BlackT::TGraphic& src,
                   int xOffset, int yOffset) {
  for (int j = 0; j < height; j++) {
    for (int i = 0; i < width; i++) {
      int x = i + xOffset;
      int y = j + yOffset;
    
      BlackT::TByte raw = src.getPixel(x, y).r();
      data_.data(i, j) = (raw >> 6);
    }
  }
}

void NesPattern::fromPalettizedGraphic(const BlackT::TGraphic& src,
                   const NesPalette& pal,
                   int xOffset, int yOffset) {
  for (int j = 0; j < height; j++) {
    for (int i = 0; i < width; i++) {
      int x = i + xOffset;
      int y = j + yOffset;
    
      BlackT::TColor color = src.getPixel(x, y);
      bool found = true;
//      BlackT::TByte raw = 0;
      for (int k = 0; k < pal.size; k++) {
        if (pal.color(k).realColor() == color) {
          found = true;
//          raw = pal.color(i).nativeValue();
//          data_.data(i, j) = pal.color(i).nativeValue();
//          BlackT::TByte raw = pal.color(i).nativeValue();
          data_.data(i, j) = k;
          break;
        }
      }
      
      if (!found) {
        throw TGenericException(T_SRCANDLINE,
                                "NesPattern::fromPalettizedGraphic()",
                                "No match for color ("
                                  + TStringConversion::intToString(
                                      color.r(), TStringConversion::baseHex)
                                  + ", "
                                  + TStringConversion::intToString(
                                      color.g(), TStringConversion::baseHex)
                                  + ", "
                                  + TStringConversion::intToString(
                                      color.b(), TStringConversion::baseHex)
                                  + ") at ("
                                  + TStringConversion::intToString(
                                      x, TStringConversion::baseHex)
                                  + ", "
                                  + TStringConversion::intToString(
                                      y, TStringConversion::baseHex)
                                  + ")");
      }
    }
  }
}
  
void NesPattern::drawPalettized(BlackT::TGraphic& dst,
                    int xOffset, int yOffset,
                    const NesPalette& src,
                    bool transparency) const {
//  dst.clear(BlackT::TColor(0, 0, 0, BlackT::TColor::fullAlphaOpacity));
  
  for (int j = 0; j < height; j++) {
    for (int i = 0; i < width; i++) {
      BlackT::TByte index = data_.data(i, j);
      NesColor color = src.color(index);
      BlackT::TColor realcolor = color.realColor();
      
//      realcolor.setA(BlackT::TColor::fullAlphaOpacity);
      if (transparency && (index == 0)) {
        realcolor.setA(BlackT::TColor::fullAlphaTransparency);
      }
      
      // background?
      // ...
      
      dst.setPixel(i + xOffset, j + yOffset,
                   realcolor);
    }
  }
}
  
void NesPattern::drawPalettized(BlackT::TGraphic& dst,
                    const NesPalette& src,
                    bool transparency) const {
  if ((dst.w() != width) || (dst.h() != height)) {
    dst = BlackT::TGraphic(width, height);
  }
  
//  dst.clear(BlackT::TColor(0, 0, 0, BlackT::TColor::fullAlphaOpacity));
  
  drawPalettized(dst, 0, 0, src, transparency);
}
  
void NesPattern::swapColors(BlackT::TByte first, BlackT::TByte second) {
  for (int j = 0; j < height; j++) {
    for (int i = 0; i < width; i++) {
      if (data(i, j) == first) {
        setData(i, j, second);
      }
      else if (data(i, j) == second) {
        setData(i, j, first);
      }
    }
  }
}


};
