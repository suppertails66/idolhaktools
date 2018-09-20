#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include "nes/NesRom.h"
#include <cstring>
#include <iostream>

using namespace std;
using namespace BlackT;
using namespace Nes;

const static int outputChrBanks = 16;
const static int outputChrSize = 0x20000;

const static int outputPrgBanks = 0x10;
const static int prgBankSize = 0x4000;
const static int outputPrgSize = (outputPrgBanks * prgBankSize);
const static int outputRomSize = outputPrgSize + outputChrSize;
const static int oldFixedBankPos = 0x1C000;
const static int newFixedBankPos = 0x3C000;

// the game assumes the mmc1 control/banking registers are undefined
// at boot, and therefore the fixed bank may not be set up; the basic
// initialization code must therefore be in every bank.
// the reset vector is BFD0, and the game expects the following sequence
// to appear there.
// note also that the first byte of every bank must be 80-FF so the serial
// register can be reset.
// 
//003FD0  78         sei 
//003FD1  D8         cld 
//; reset MMC1 serial register; 0000 == $80
//003FD2  EE 00 80   inc $8000
//; set initial mapper register states?
//003FD5  20 51 DD   jsr $DD51
//; set PPUCTRL
//003FD8  A9 00      lda #$00
//003FDA  8D 00 20   sta $2000
//; jump to boot code
//003FDD  4C 01 C0   jmp $C001
const static char bankCode[]
  = "\x78\xD8\xEE\x00\x80\x20\x51\xDD\xA9\x00\x8D\x00\x20\x4C\x01\xC0";

int main(int argc, char* argv[]) {
  if (argc < 4) {
    cout << "Idol Hakkenden ROM finalizer" << endl;
    cout << "Usage: " << argv[0]
      << " <inprg> <inchr> <outrom>" << endl;
    
    return 0;
  }
  
  // open ROM
  
  NesRom rom = NesRom(string(argv[1]));
  TBufStream ifs(rom.size());
  rom.toStream(ifs);
  
  // expand ROM
  
  ifs.setCapacity(outputRomSize);
  ifs.seek(ifs.size());
  ifs.alignToBoundary(outputRomSize);
  ifs.seek(0);
  
  // enforce reset banking restrictions
  for (int i = 0; i < outputPrgBanks; i++) {
    ifs.seek((i * prgBankSize) + 0x3FD0);
    ifs.write(bankCode, (sizeof(bankCode) / sizeof(char)) - 1);
    ifs.seek((i * prgBankSize));
    ifs.put(0x80);
  }
  
  // write CHR
  
  {
    TBufStream chrifs(1);
    chrifs.open(argv[2]);
    ifs.seek(outputPrgSize);
    ifs.writeFrom(chrifs, outputChrSize);
  }
  
  // write to file with iNES header
  
  ifs.seek(0);
  rom.changeSize(outputRomSize);
  rom.fromStream(ifs);
  rom.exportToFile(string(argv[3]), outputPrgBanks, outputChrBanks,
                   NesRom::nametablesVertical,
                   NesRom::mapperMmc1,
                   false);
  
  return 0;
} 
