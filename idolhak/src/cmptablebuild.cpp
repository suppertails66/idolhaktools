#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include "util/TThingyTable.h"
#include "util/TStringConversion.h"
#include <iostream>

using namespace std;
using namespace BlackT;

const static int cmpRangeLow = 0xC8;
const static int cmpRangeHigh = 0xF8;
const static int cmpRangeLimit = cmpRangeHigh + 1;

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "Idol Hakkenden text compression table builder" << endl;
    cout << "Usage: " << argv[0] << " <intable> <outfile>" << endl;
    
    return 0;
  }
  
  TThingyTable table;
  table.readSjis(argv[1]);
  TBufStream ofs(0x1000);
  
  for (int i = cmpRangeLow; i < cmpRangeLimit; i++) {
    if (!table.hasEntry(i)) {
      ofs.writeu16be(0x0);
      continue;
    }
    
    string value = table.getEntry(i);
    if (value.size() != 2) {
      ofs.writeu16be(0x0);
      continue;
    }
    
    for (unsigned int j = 0; j < value.size(); j++) {
      string c = value.substr(j, 1);
//      if (!table.hasEntry(c)) {
//        cerr << "Error: entry "
//          << TStringConversion::intToString(i, TStringConversion::baseHex)
//          << " uses characters not in table" << endl;
//        return 1;
//      }
      
      TThingyTable::MatchResult result = table.matchTableEntry(c);
      if (result.id == -1) {
        cerr << "Error: entry "
          << TStringConversion::intToString(i, TStringConversion::baseHex)
          << " uses characters not in table" << endl;
        return 1;
      }
      
      ofs.writeu8(result.id);
    }
  }
  
  ofs.save(argv[2]);
  
  return 0;
}
