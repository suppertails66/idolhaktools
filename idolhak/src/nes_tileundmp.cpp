#include "nes/NesPattern.h"
#include "nes/NesRom.h"
#include "util/TOpt.h"
#include "util/TBufStream.h"
#include "util/TStringConversion.h"
#include "util/TGraphic.h"
#include "util/TPngConversion.h"
#include <string>
#include <iostream>

const static int defaultTilesPerRow = 16;

using namespace std;
using namespace BlackT;
using namespace Nes;

int main(int argc, char* argv[]) {
  if (argc < 4) {
    cout << "Graphic -> NES pattern data converter" << endl;
    cout << "Usage: <infile> <numtiles> <outfile>" << endl;
    cout << "Options: " << endl;
    cout << "  -w   Set width of output in tiles (default: 16)" << endl;
    
    return 0;
  }
  
  int numTiles = TStringConversion::stringToInt(string(argv[2]));
  char* outfile = argv[3];
  
  int tilesPerRow = defaultTilesPerRow;
  TOpt::readNumericOpt(argc, argv, "-w", &tilesPerRow);
  
//  int width = tilesPerRow * NesPattern::width;
//  int height = numTiles / tilesPerRow;
//  if ((numTiles % tilesPerRow) != 0) ++height;
//  height *= NesPattern::height;
  
  TGraphic g;
  TPngConversion::RGBAPngToGraphic(string(argv[1]), g);
  
  TBufStream ofs(0x100000);
  for (int i = 0; i < numTiles; i++) {
    int x = (i % tilesPerRow) * NesPattern::width;
    int y = (i / tilesPerRow) * NesPattern::height;
    
    NesPattern pattern;
    pattern.fromGrayscaleGraphic(g, x, y);
    pattern.write(ofs);
  }
  
  ofs.save(outfile);
  
  return 0;
}
