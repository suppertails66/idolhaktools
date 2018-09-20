#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include "nes/NesRom.h"
#include <iostream>

using namespace std;
using namespace BlackT;
using namespace Nes;

const static int outputPrgBanks = 0x10;
const static int prgBankSize = 0x4000;
const static int outputRomSize = outputPrgBanks * prgBankSize;
const static int oldFixedBankPos = 0x1C000;
const static int newFixedBankPos = 0x3C000;

const static int chrOffset = 0x20000;
const static int chrSize = 0x20000;

int main(int argc, char* argv[]) {
  if (argc < 4) {
    cout << "Idol Hakkenden ROM preparer" << endl;
    cout << "Usage: " << argv[0]
      << " <inrom> <outprg> <outchr>" << endl;
    
    return 0;
  }
  
  // open ROM
  
  NesRom rom = NesRom(string(argv[1]));
  TBufStream ifs(rom.size());
  rom.toStream(ifs);
  
  // copy out CHR
  {
    ifs.seek(chrOffset);
    TBufStream ofs(chrSize);
    ofs.writeFrom(ifs, chrSize);
    ofs.save(argv[3]);
  }
  
  // expand ROM
  
  ifs.setCapacity(outputRomSize);
  ifs.seek(ifs.size());
  ifs.alignToBoundary(outputRomSize);
  ifs.seek(0);
  
  // copy fixed bank to final bank
  
  ifs.seek(newFixedBankPos);
  ifs.write((char*)rom.directRead(oldFixedBankPos), prgBankSize);
  
  // write to file (no iNES header)
  
  ifs.save(argv[2]);
  
/*  // write to file with iNES header
  
  ifs.seek(0);
  rom.changeSize(outputRomSize);
  rom.fromStream(ifs);
  rom.exportToFile(string(argv[2]), outputPrgBanks, 0,
                   NesRom::nametablesVertical,
                   NesRom::mapperMmc1,
                   false); */
  
  return 0;
} 
