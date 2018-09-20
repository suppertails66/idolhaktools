#include "nes/NesPatternTable.h"

namespace Nes {


NesPatternTable::NesPatternTable() { };
  
void NesPatternTable::fromUncompressedData(const BlackT::TByte* data) {
  for (int i = 0; i < numTiles; i++) {
    NesPattern tile;
    data += tile.fromUncompressedData(data);
    tiles_[i] = tile;
  }
}
  
void NesPatternTable::toUncompressedData(BlackT::TByte* data) const {
  for (int i = 0; i < numTiles; i++) {
    data += tiles_[i].toUncompressedData(data);
  }
}

void NesPatternTable::read(BlackT::TStream& ifs) {
  BlackT::TByte buffer[dataSize];
  ifs.read((char*)buffer, dataSize);
  fromUncompressedData(buffer);
}

void NesPatternTable::write(BlackT::TStream& ofs) const {
  BlackT::TByte buffer[dataSize];
  toUncompressedData(buffer);
  ofs.write((char*)buffer, dataSize);
}
  
int NesPatternTable::save(BlackT::TString& data) const {
  BlackT::TByte buffer[dataSize];
  
  toUncompressedData(buffer);
  data += std::string((char*)(buffer), dataSize);
  
  return dataSize;
}

int NesPatternTable::load(const BlackT::TByte* data) {
  fromUncompressedData(data);
  
  return dataSize;
}
  
NesPattern& NesPatternTable::tile(int index) {
  return tiles_[index];
}

const NesPattern& NesPatternTable::tile(int index) const {
  return tiles_[index];
}


};
