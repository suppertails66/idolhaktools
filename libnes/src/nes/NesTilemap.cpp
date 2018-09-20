#include "nes/NesTilemap.h"
#include "util/TStringConversion.h"
#include "exception/TGenericException.h"
#include <iostream>

using namespace BlackT;

namespace Nes {


NesTilemap::NesTilemap() { };

NesTilemap::NesTilemap(int width__, int height__)
  : data_(width__, height__) { };

bool NesTilemap::operator==(const NesTilemap& other) const {
  for (int j = 0; j < data_.h(); j++) {
    for (int i = 0; i < data_.w(); i++) {
      if (data_.data(i, j) != other.data_.data(i, j)) return false;
    }
  }
  
  return true;
}

bool NesTilemap::operator!=(const NesTilemap& other) const {
  return (!(*this == other));
}

int NesTilemap::data(int x, int y) const {
  return data_.data(x, y);
}
  
void NesTilemap::setData(int x, int y, int d) {
  data_.setDataClipped(x, y, d);
}

int NesTilemap::w() const {
  return data_.w();
}

int NesTilemap::h() const {
  return data_.h();
}
  
void NesTilemap::read(BlackT::TStream& ifs, int width__, int height__) {
  resize(width__, height__);
  
  for (int j = 0; j < data_.h(); j++) {
    for (int i = 0; i < data_.w(); i++) {
      data_.data(i, j) = ifs.readu8();
    }
  }
}

void NesTilemap::write(BlackT::TStream& ofs) const {
  for (int j = 0; j < data_.h(); j++) {
    for (int i = 0; i < data_.w(); i++) {
      ofs.writeu8(data_.data(i, j));
    }
  }
}

void NesTilemap::resize(int width__, int height__) {
  data_.resize(width__, height__);
}

void NesTilemap::drawGrayscale(BlackT::TGraphic& dst,
                     const NesPatternTable& patternTable,
                     int xOffset, int yOffset,
                     bool transparency) const {
  for (int j = 0; j < data_.h(); j++) {
    for (int i = 0; i < data_.w(); i++) {
      int x = (i * NesPattern::w) + xOffset;
      int y = (j * NesPattern::h) + yOffset;
      
      int id = data_.data(i, j);
      const NesPattern& pattern = patternTable.tile(id);
      pattern.drawGrayscale(dst, x, y, transparency);
    }
  }
}


};
