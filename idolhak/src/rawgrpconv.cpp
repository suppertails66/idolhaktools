#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TStringConversion.h"
#include "util/TGraphic.h"
#include "util/TPngConversion.h"
#include "util/TThingyTable.h"
#include "util/TParse.h"
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

TBufStream patchifs(1);
TGraphic grp;
TBufStream ofs(1);

int main(int argc, char* argv[]) {
  if (argc < 5) {
    cout << "Raw NES graphic patcher" << endl;
    cout << "Usage: " << argv[0]
      << " <ingraphic> <patchfile> <basefile> <outfile>" << endl;
    
    return 0;
  }

  TPngConversion::RGBAPngToGraphic(string(argv[1]), grp);
  patchifs.open(argv[2]);
  ofs.open(argv[3]);
  
  int w = (grp.w() / NesPattern::w);
  int h = (grp.h() / NesPattern::h);
  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
      int x = (i * NesPattern::w);
      int y = (j * NesPattern::h);
      
      NesPattern pattern;
      pattern.fromGrayscaleGraphic(grp, x, y);
      
      int patternNum = TParse::matchInt(patchifs);
      ofs.seek(patternNum * NesPattern::size);
      pattern.write(ofs);
    }
  }
  
  ofs.save(argv[4]);
  
  return 0;
}
