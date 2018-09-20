#include "nes/NesRom.h"
#include "util/TFileManip.h"
#include "exception/TGenericException.h"
#include "util/ByteConversion.h"
#include "util/TStringConversion.h"
#include <cstring>
#include <fstream>

using namespace BlackT;

namespace Nes {


const BlackT::TByte NesRom::inesIdentifier[inesIdentifierSize_]
  = { 'N', 'E', 'S', 0x1A };
  
NesRom::NesRom(const BlackT::TString& filename)
  : rom_(),
    mapperType_(mapperUxRom) {
  readFromFile(filename);
}
  
void NesRom::exportToFile(const BlackT::TString& filename,
                    int numPrgBanks,
                    int numChrBanks,
                    NametableArrangementFlag nametablesFlag,
                    MapperType mapperNum,
                    bool hasBatteryRam) {
  std::ofstream ofs(filename, std::ios_base::binary
                                | std::ios_base::trunc);
  
  BlackT::TByte buffer[ByteSizes::uint32Size];
  
  // iNES header
  
  // "NES" identifier
  ofs.write((const char*)inesIdentifier, inesIdentifierSize_);
  
  // ROM size
  ByteConversion::toBytes(numPrgBanks,
                          buffer,
                          ByteSizes::uint8Size,
                          EndiannessTypes::little,
                          SignednessTypes::nosign);
  ofs.write((char*)buffer, ByteSizes::uint8Size);
  
  ByteConversion::toBytes(numChrBanks,
                          buffer,
                          ByteSizes::uint8Size,
                          EndiannessTypes::little,
                          SignednessTypes::nosign);
  ofs.write((char*)buffer, ByteSizes::uint8Size);
  
  // Flags
  BlackT::TByte flags6 = 0;
  BlackT::TByte flags7 = 0;
  
  if (hasBatteryRam) {
    flags6 |= 0x02;
  }
  
  flags6 |= nametablesFlag;
  flags6 |= ((mapperNum & 0x0F) << 4);
  flags7 |= ((mapperNum & 0xF0) << 4);
  
  ofs.write((char*)(&flags6), ByteSizes::uint8Size);
  ofs.write((char*)(&flags7), ByteSizes::uint8Size);
  
  // Bytes 8-15 aren't implemented
  for (int i = 8; i < 16; i++) {
    char c = 0x00;
    ofs.write((char*)(&c), ByteSizes::uint8Size);
  }
  
  // Data
  ofs.write((char*)(rom_.data()), rom_.size());
}
  
const BlackT::TByte* NesRom::directRead(int address) const {
  return rom_.data() + address;
}

BlackT::TByte* NesRom::directWrite(int address) {
  return rom_.data() + address;
}

void NesRom::toStream(BlackT::TStream& ofs) const {
  ofs.write((char*)rom_.data(), size());
}

void NesRom::fromStream(BlackT::TStream& ifs) {
  rom_.resize(ifs.size());
  ifs.read((char*)rom_.data(), size());
}
  
NesRom::MapperType NesRom::mapperType() const {
  return mapperType_;
}

void NesRom::setMapperType(MapperType mapperType__) {
  mapperType_ = mapperType__;
}

void NesRom::readFromFile(const BlackT::TString& filename) {
  if (!BlackT::TFileManip::fileExists(filename)) {
    throw TGenericException(T_SRCANDLINE,
                           "NesRom::readFromFile(const BlackT::TString&)",
                           "Tried to read nonexistent file");
  }
  
  std::ifstream ifs(filename,
                    std::ios_base::binary);
  int fileSize = BlackT::TFileManip::getFileSize(ifs);
  
  if (fileSize < minFileSize_) {
    throw TGenericException(T_SRCANDLINE,
                           "NesRom::readFromFile(const BlackT::TString&)",
                           "Tried to read invalid-sized file");
  }
  
  // Check for presence of iNES header
  BlackT::TByte inesCheck[inesIdentifierSize_];
  ifs.read((char*)inesCheck, inesIdentifierSize_);
  
  if (std::memcmp(inesIdentifier, inesCheck, inesIdentifierSize_)
        == 0) {
    // Read iNES data?
    // ...
    
    // Skip to start of actual data
    ifs.seekg(inesHeaderOffset_ + inesHeaderSize_);
  }
  else {
    // No header: return to start of file
    ifs.seekg(0);
  }
  
  // Compute ROM size (accounting for presence or absence of header)
  int romSize = fileSize - ifs.tellg();
  rom_.resize(romSize);
  
  // Read entire file
  ifs.read((char*)(rom_.data()), romSize);
  
}
  
int NesRom::size() const {
  return rom_.size();
}
  
void NesRom::changeSize(int newSize) {
  BlackT::TArray<BlackT::TByte> newData(newSize);
  
  int amount = (newSize < rom_.size()) ? newSize : rom_.size();
  
  std::memcpy(newData.data(),
              rom_.data(),
              amount);
  
  rom_ = newData;
}


};
