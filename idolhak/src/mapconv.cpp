#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TStringConversion.h"
#include "util/TGraphic.h"
#include "util/TPngConversion.h"
#include "util/TThingyTable.h"
#include "nes/NesRom.h"
#include "nes/NesPattern.h"
#include "exception/TGenericException.h"
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>

using namespace std;
using namespace BlackT;
using namespace Nes;

const static int bgChar = 0x00;

string as2bHex(int value) {
  string str = TStringConversion::intToString(value,
          TStringConversion::baseHex).substr(2, string::npos);
  while (str.size() < 2) str = "0" + str;
  return str;
}

string nextAsRaw(TStream& ifs) {
  string str;
  str += "<$" + as2bHex(ifs.readu8()) + ">";
  return str;
}

void convertType1(TStream& ifs, TStream& ofs,
                  int w, int h, int ppuaddr,
                  bool optimize = true) {
  int base = ifs.tell();
  for (int j = 0; j < h; j++) {
    ifs.seek(base + (j * w));
    
    int linebase = ifs.tell();
    int linestart = linebase;
    int lineend = linestart + w;
    
    if (optimize) {
      ifs.seek(lineend);
      ifs.seekoff(-1);
      while ((lineend > linestart) && ((unsigned char)ifs.peek() == bgChar)) {
        --lineend;
        ifs.seekoff(-1);
      }
    
      ifs.seek(linebase);
      while ((linestart < lineend) && ((unsigned char)ifs.peek() == bgChar)) {
        ++linestart;
        ifs.get();
      }
      
      if (linestart >= lineend) continue;
      int linelen = lineend - linestart;
      ifs.seek(linestart);
    }
    
    ofs.writeu16be(ppuaddr + (0x20 * j) + (linestart - linebase));
    for (int i = linestart; i < lineend; i++) {
      ofs.put(ifs.get());
    }
    
    if (j != (h - 1)) ofs.writeu8(0xFE);
  }
  
  // terminator
  ofs.writeu8(0xFF);
}

void convertType3(TStream& ifs, TStream& ofs,
                  int w, int h, int ppuaddr) {
  int totalsize = (w * h) + (h * 3);
  if (totalsize > 0xFF) {
    throw TGenericException(T_SRCANDLINE,
                            "convertType3()",
                            "Source too large");
  }
  ofs.writeu8(totalsize);
  
  for (int j = 0; j < h; j++) {
    ifs.seek(j * w);
    
/*    int linebase = ifs.tell();
    int linestart = linebase;
    int lineend = linestart + w;
    
    ifs.seek(lineend);
    ifs.seekoff(-1);
    while ((lineend > linestart) && ((unsigned char)ifs.peek() == bgChar)) {
      --lineend;
      ifs.seekoff(-1);
    }
  
    ifs.seek(linebase);
    while ((linestart < lineend) && ((unsigned char)ifs.peek() == bgChar)) {
      ++linestart;
      ifs.get();
    }
    
    if (linestart >= lineend) continue;
    int linelen = lineend - linestart;
    ifs.seek(linestart); */
    
    int linelen = w;
    ofs.writeu8(linelen);
//    ofs.writeu16le(ppuaddr + (0x20 * j) + (linestart - linebase));
    ofs.writeu16le(ppuaddr + (0x20 * j));
//    for (int i = linestart; i < lineend; i++) {
    for (int i = 0; i < w; i++) {
      ofs.put(ifs.get());
    }
  }
  
  // terminator
  ofs.writeu8(0xFF);
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    cout << "Idol Hakkenden tilemap preparer" << endl;
    cout << "Usage: " << argv[0]
      << " <outprefix>" << endl;
    
    return 0;
  }
  string outprefix = string(argv[1]);
  
  {
    TIfstream ifs("out/maps_raw/title.bin", ios_base::binary);
    
    {
      TOfstream ofs((outprefix + "title0.bin").c_str(), ios_base::binary);
      convertType1(ifs, ofs, 24, 2, 0x2084, false);
    }
    
    {
      TOfstream ofs((outprefix + "title1.bin").c_str(), ios_base::binary);
      convertType1(ifs, ofs, 24, 2, 0x20C4, false);
    }
    
    {
      TOfstream ofs((outprefix + "title2.bin").c_str(), ios_base::binary);
      convertType1(ifs, ofs, 24, 2, 0x2104, false);
    }
  }
  
  return 0;
}
