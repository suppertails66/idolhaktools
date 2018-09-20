#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TGraphic.h"
#include "util/TStringConversion.h"
#include "util/TPngConversion.h"
#include "util/TCsv.h"
#include "util/TSoundFile.h"
#include "nes/NesPattern.h"
#include <string>
#include <iostream>

using namespace std;
using namespace BlackT;
using namespace Nes;

const static int charsPerRow = 16;
int charW = NesPattern::w;
int charH = NesPattern::h;

void readPattern(const TGraphic& g,
                 int offsetX, int offsetY,
                 NesPattern& dst) {
  for (int j = 0; j < NesPattern::h; j++) {
    for (int i = 0; i < NesPattern::w; i++) {
      int x = offsetX + i;
      int y = offsetY + j;
      
      TColor color = g.getPixel(x, y);
      if ((color.a() != TColor::fullAlphaTransparency)
          && (color.r() >= 0x80)) {
        dst.setData(i, j, 0x01);
      }
      else {
        dst.setData(i, j, 0x03);
      }
    }
  }
}

int main(int argc, char* argv[]) {
  if (argc < 5) {
    cout << "Ripple Island 8x8 font builder" << endl;
    cout << "Usage: " << argv[0]
      << " <infile> <numchars> <outfile> <mode> [prepatterns]" << endl;
    
    return 0;
  }
  
  string infile = string(argv[1]);
  int numchars = TStringConversion::stringToInt(string(argv[2]));
  string outfile = string(argv[3]);
  string mode = string(argv[4]);
  bool prepatterns = false;
  if (argc >= 6) {
    if (string(argv[5]).compare("prepatterns") == 0) prepatterns = true;
  }
  
  if (mode.compare("short") == 0) {
    charH = 8;
  }
  else if (mode.compare("tall") == 0) {
    charH = 16;
  }
  
  TGraphic g;
  TPngConversion::RGBAPngToGraphic(infile, g);
  
  TOfstream ofs(outfile.c_str(), ios_base::binary);

  // first 4 "characters" are solid colors used for backgrounds
  if (prepatterns) {
    ofs.write("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
              16);
    ofs.write("\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x00\x00\x00\x00\x00\x00\x00\x00",
              16);
    ofs.write("\x00\x00\x00\x00\x00\x00\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
              16);
    ofs.write("\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
              16);
  }
  
  for (int i = 0; i < numchars; i++) {
    int srcX = (i % charsPerRow) * charW;
    int srcY = (i / charsPerRow) * charH;
    
    if (mode.compare("short") == 0) {
      NesPattern pattern;
      readPattern(g, srcX, srcY, pattern);
      pattern.write(ofs);
    }
    else if (mode.compare("tall") == 0) {
      NesPattern pattern;
      readPattern(g, srcX, srcY, pattern);
      pattern.write(ofs);
      readPattern(g, srcX, srcY + NesPattern::height, pattern);
      pattern.write(ofs);
    }
  }
  
  return 0;
}
