#include "util/TBufStream.h"
#include "util/TStringConversion.h"
#include "util/TOpt.h"
#include <iostream>

using namespace std;
using namespace BlackT;

int main(int argc, char* argv[]) {
/*  if (argc < 5) {
    cout << "Idol Hakkenden ROM prefinalizer" << endl;
    cout << "Usage: " << argv[0] << " <infile> <outfile>" << endl;
    
    return 0;
  } */
  
  TBufStream ifs(1);
  ifs.open("idolhak_noheader.nes");
  ifs.seek(0x1402E);
  for (int i = 0; i < 83; i++) {
    int val = ifs.readu16le();
    cout << ".dw #$" << hex << val << endl;
  }
  
/*  char* infile = argv[1];
  char* outfile = argv[2];
  
  TBufStream ifs(1);
  file.open(infile);
  
  
  // copy out 
  TBufStream ofs(1);
  patch.open(outfile); */
  
  return 0;
}
