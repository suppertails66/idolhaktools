#ifndef NESPATTERNTABLE_H
#define NESPATTERNTABLE_H


#include "util/TByte.h"
#include "util/TString.h"
#include "util/TStream.h"
#include "nes/NesPattern.h"

namespace Nes {


class NesPatternTable {
public:
  const static int numTiles = 0x100;
  const static int dataSize = numTiles * NesPattern::size;

  NesPatternTable();
  
  void fromUncompressedData(const BlackT::TByte* data);
  void toUncompressedData(BlackT::TByte* data) const;
  
  void read(BlackT::TStream& ifs);
  void write(BlackT::TStream& ofs) const;
  
  int save(BlackT::TString& data) const;
  
  int load(const BlackT::TByte* data);
  
  NesPattern& tile(int index);
  
  const NesPattern& tile(int index) const;
protected:
  NesPattern tiles_[numTiles];
  
};


};


#endif
