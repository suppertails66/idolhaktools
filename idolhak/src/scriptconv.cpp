#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TGraphic.h"
#include "util/TStringConversion.h"
#include "util/TPngConversion.h"
#include "util/TCsv.h"
#include "util/TThingyTable.h"
#include "nes/NesPattern.h"
#include "idolhak/IdolHakScriptReader.h"
#include <string>
#include <vector>
#include <iostream>

using namespace std;
using namespace BlackT;
using namespace Nes;

const static int numBanks = 4;
const static int numSets = 9;
//const static int splitPoint = 3;

//const static int baseAddr = 0x8000;

const static int scriptBaseAddr = 0x0010;
const static int maxScriptBankSize = 0x3FD0 - 0x10;

TThingyTable table;
TThingyTable nameTable;
std::vector<IdolHakScriptReader::ResultCollection> sets;

/*void generateRegionSubset(int start, int end, TStream& ofs) {
  // skip region pointer table to fill in later
  int regionTablePos = ofs.tell();
  ofs.seekoff(numScriptRegions * 2);
  
  // compute local offsets of all strings
  
  std::vector< std::vector<int> > regionStringOffsets;
  regionStringOffsets.resize(numScriptRegions);
    
  int stringOffset = 0;
  int stringPointerCount = 0;
  for (int i = 0; i < numScriptRegions; i++) {
    if ((i < start) || (i >= end)) {
      continue;
    }
  
//    TBufStream stringOfs(0x10000);
    IdolHakScriptReader::ResultCollection& srcStrings
      = regions[i];
    
    std::vector<int>& rawStringOffsets = regionStringOffsets[i];
//    rawStringOffsets.resize(srcStrings.size() + 1);
    rawStringOffsets.resize(srcStrings.size());
    
    // dummy zero entry
//    rawStringOffsets[0] = stringOffset;
//    ++stringPointerCount;
    
    for (unsigned int j = 0; j < srcStrings.size(); j++) {
//      rawStringOffsets[j + 1] = stringOffset;
      rawStringOffsets[j] = stringOffset;
      stringOffset += srcStrings[j].str.size();
      
      ++stringPointerCount;
    }
  }
  
  // table of region start offsets
  
  std::vector<int> stringTableOffsets;
  stringTableOffsets.resize(numScriptRegions);
  
  // write string offset tables
  
  ofs.seek(numScriptRegions * 2);
  for (int i = 0; i < numScriptRegions; i++) {
    if ((i < start) || (i >= end)) {
//      stringTableOffsets[i] = 0xFFFF;
      stringTableOffsets[i] = ofs.tell();
      continue;
    }
    
    stringTableOffsets[i] = ofs.tell();
    std::vector<int>& stringOffsets = regionStringOffsets[i];
    
    for (unsigned int j = 0; j < stringOffsets.size(); j++) {
      // write string offset
      ofs.writeu16le(stringOffsets[j]
                     + baseAddr
                     + (numScriptRegions * 2)
                     + (stringPointerCount * 2));
    }
  }
  
  // write strings
  
  for (int i = 0; i < numScriptRegions; i++) {
    if ((i < start) || (i >= end)) {
      continue;
    }
    
//    std::cerr << regionStringOffsets[0].size() << endl;
    
    IdolHakScriptReader::ResultCollection& srcStrings
      = regions[i];
    
    for (unsigned int j = 0; j < srcStrings.size(); j++) {
      for (unsigned int k = 0; k < srcStrings[j].str.size(); k++) {
        ofs.put(srcStrings[j].str[k]);
      }
    }
  }
  
  // write region start offset table
  
  ofs.seek(regionTablePos);
  for (int i = 0; i < numScriptRegions; i++) {
    if ((i < start) || (i >= end)) {
//      ofs.writeu16le(0xFFFF);
      ofs.writeu16le(stringTableOffsets[i]
                     + baseAddr);
      continue;
    }
    
//    for (unsigned int j = 0; j < stringTableOffsets.size(); j++) {
      ofs.writeu16le(stringTableOffsets[i]
                     + baseAddr);
//    }
  }
} */

void packScripts(const IdolHakScriptReader::ResultCollection& set,
                   TStream& ofs, int slotBase = 0x8000, int basePos = 0x0000) {
  int indexBase = ofs.tell();
  int putpos = ofs.tell() + (set.size() * 2);
  for (unsigned int i = 0; i < set.size(); i++) {
    const IdolHakScriptReader::ResultString& resultString = set[i];
    ofs.seek(indexBase + (i * 2));
    ofs.writeu16le((putpos % 0x4000) + slotBase + basePos);
    ofs.seek(putpos);
    ofs.write(resultString.str.c_str(), resultString.str.size());
    putpos += resultString.str.size();
  }
}

void writeScriptsRaw(const IdolHakScriptReader::ResultCollection& set,
                   TStream& ofs) {
  for (unsigned int i = 0; i < set.size(); i++) {
    const IdolHakScriptReader::ResultString& resultString = set[i];
    ofs.write(resultString.str.c_str(), resultString.str.size());
  }
}

void writeAsmSet(const IdolHakScriptReader::ResultCollection& set,
                 string baseName, string baseBinName,
                 string outIncName) {
  std::ofstream ofs(outIncName.c_str());
  
  for (unsigned int i = 0; i < set.size(); i++) {
    string name = baseName
                  + TStringConversion::intToString(i);
    ofs << ".dw " << name << endl;
  }
  
  for (unsigned int i = 0; i < set.size(); i++) {
    TBufStream binofs(0x1000);
    string name = baseName
                  + TStringConversion::intToString(i);
    string binname = baseBinName
                + TStringConversion::intToString(i)
                + ".bin";
    
    const IdolHakScriptReader::ResultString& resultString = set[i];
    binofs.write(resultString.str.c_str(), resultString.str.size());
    
    ofs << name << ":" << endl;
    ofs << "  .incbin \"" << binname << "\"" << endl;
    
    binofs.save(binname.c_str());
  }
  
}

void addSet(string filename) {
  cout << "adding set " << filename << endl;
  
  IdolHakScriptReader::ResultCollection set;
//  TIfstream ifs((filename).c_str(), ios_base::binary);
  TBufStream ifs(1);
  ifs.open((filename).c_str());
  IdolHakScriptReader(ifs, set, table, nameTable, false, true)();
  sets.push_back(set);
}

int main(int argc, char* argv[]) {
  if (argc < 4) {
    cout << "Idol Hakkenden script converter" << endl;
    cout << "Usage: " << argv[0]
      << " <inprefix> <table> <charatable> <outprefix>" << endl;
    
    return 0;
  }
  
  std::string prefix = std::string(argv[1]);
  std::string outprefix = std::string(argv[4]);
  
//  table.readUtf8(string(argv[2]));
  table.readSjis(string(argv[2]));
  
  nameTable.readSjis(string(argv[3]));

  // screw passing in parameters, I have to rewrite this damn thing
  // every time anyway
  TThingyTable table_nocmp;
  table_nocmp.readSjis("table/idolhak_en_nocmp.tbl");
  TThingyTable table_cred;
  table_cred.readSjis("table/idolhak_cred_en.tbl");

//  sets.resize(numSets);
  
/*  int setnum = 0;
  for (int i = 0; i < numBanks; i++) {
    for (int j = 0; j < 2; j++) {
      TIfstream ifs((prefix + "bank" + TStringConversion::intToString(i)
                      + "-"
                      + TStringConversion::intToString(j)
                      + ".txt").c_str(), ios_base::binary);
      
      IdolHakScriptReader(ifs, sets[setnum++], table, nameTable)();
    }
  } */
  
//  {
//    TIfstream ifs((prefix + "bank0-0.txt").c_str(), ios_base::binary);
//  }

  addSet(prefix + "bank0.txt");
  addSet(prefix + "bank1.txt");
  addSet(prefix + "bank2.txt");
  addSet(prefix + "bank3.txt");
  addSet(prefix + "bank4.txt");
  addSet(prefix + "bank5.txt");
  
  for (unsigned int i = 0; i < sets.size(); i++) {
    TBufStream ofs(0x10000);
    packScripts(sets[i], ofs, 0x8000, 0x0010);
    
    if (ofs.size() >= maxScriptBankSize) {
      cerr << "Error: section " << i << " too big ("
        << ofs.size() << " bytes, max " << maxScriptBankSize << ")" << endl;
      return 1;
    }
    
//    ofs.write(sets[i].c_str(), sets[i].size());
    ofs.save((outprefix + "script_" + TStringConversion::intToString(i)
                + ".bin").c_str());
  }
  
  {
    TBufStream ifs(1);
    ifs.open((prefix + "menus" + ".txt").c_str());
    IdolHakScriptReader::ResultCollection set;
    IdolHakScriptReader(ifs, set, table_nocmp, table_nocmp, false)();
    
    TBufStream ofs(0x10000);
    writeScriptsRaw(set, ofs);
    ofs.save((outprefix + "menus"
                + ".bin").c_str());
  }
  
  {
    TBufStream ifs(1);
    ifs.open((prefix + "credits" + ".txt").c_str());
    IdolHakScriptReader::ResultCollection set;
    IdolHakScriptReader(ifs, set, table_cred, table_cred, false)();
    
    writeAsmSet(set,
                "creditsMap",
                outprefix + "credits/credmap_",
                outprefix + "credits/credits.inc");
    
/*    std::ofstream ofs((outprefix + "credits/" + "credits.inc").c_str());
    
    for (unsigned int i = 0; i < set.size(); i++) {
      string name = "creditsMap"
                    + TStringConversion::intToString(i);
      ofs << ".dw " << name << endl;
    }
    
    for (unsigned int i = 0; i < set.size(); i++) {
      TBufStream binofs(0x1000);
      string name = "creditsMap"
                    + TStringConversion::intToString(i);
      string binname = (outprefix + "credits/" + "credmap_"
                  + TStringConversion::intToString(i)
                  + ".bin");
      
      const IdolHakScriptReader::ResultString& resultString = set[i];
      binofs.write(resultString.str.c_str(), resultString.str.size());
      
      ofs << name << ":" << endl;
      ofs << "  .incbin \"" << binname << "\"" << endl;
      
      binofs.save(binname.c_str());
    } */
  }
  
  {
    TBufStream ifs(1);
    ifs.open((prefix + "karaoke.txt").c_str());
    IdolHakScriptReader::ResultCollection set;
    IdolHakScriptReader(ifs, set, table_nocmp, table_nocmp, false)();
    
    writeAsmSet(set,
                "karaokeEnMap",
                outprefix + "credits/karaenmap_",
                outprefix + "credits/karaoke_en.inc");
    
  }
  
  {
    TBufStream ifs(1);
    ifs.open((prefix + "intro.txt").c_str());
    IdolHakScriptReader::ResultCollection set;
    IdolHakScriptReader(ifs, set, table_nocmp, table_nocmp, false)();
    
    writeAsmSet(set,
                "introMap",
                outprefix + "maps/intromap_",
                outprefix + "maps/intro.inc");
    
  }
  
  {
    TBufStream ifs(1);
    ifs.open((prefix + "password.txt").c_str());
    IdolHakScriptReader::ResultCollection set;
    IdolHakScriptReader(ifs, set, table_nocmp, table_nocmp, false)();
    
    writeAsmSet(set,
                "passwordMap",
                outprefix + "maps/passwordmap_",
                outprefix + "maps/password.inc");
    
  }
  
  for (int i = 1; i <= 5; i++) {
    string numstr = TStringConversion::intToString(i);
    {
      TBufStream ifs(1);
      ifs.open((prefix + "chapter" + numstr + ".txt").c_str());
      IdolHakScriptReader::ResultCollection set;
      IdolHakScriptReader(ifs, set, table_nocmp, table_nocmp, false)();
      
      writeAsmSet(set,
                  "chapter" + numstr + "Map",
                  outprefix + "maps/chapter" + numstr + "map_",
                  outprefix + "maps/chapter" + numstr + ".inc");
      
    }
  }
  
  {
    TBufStream ifs(1);
    ifs.open((prefix + "cybergong_inout" + ".txt").c_str());
    IdolHakScriptReader::ResultCollection set;
    IdolHakScriptReader(ifs, set, table_nocmp, table_nocmp, false)();
    
    TBufStream ofs(0x10000);
    writeScriptsRaw(set, ofs);
    ofs.save((outprefix + "cybergong_inout"
                + ".bin").c_str());
  }
  
  {
    TBufStream ifs(1);
    ifs.open((prefix + "tamashikaya" + ".txt").c_str());
    IdolHakScriptReader::ResultCollection set;
    IdolHakScriptReader(ifs, set, table_nocmp, table_nocmp, false)();
    
    TBufStream ofs(0x10000);
    writeScriptsRaw(set, ofs);
    ofs.save((outprefix + "tamashikaya"
                + ".bin").c_str());
  }
  
  {
    TBufStream ifs(1);
    ifs.open((prefix + "kakinabe_market" + ".txt").c_str());
    IdolHakScriptReader::ResultCollection set;
    IdolHakScriptReader(ifs, set, table_nocmp, table_nocmp, false)();
    
    TBufStream ofs(0x10000);
    writeScriptsRaw(set, ofs);
    ofs.save((outprefix + "kakinabe_market"
                + ".bin").c_str());
  }
  
  {
    TBufStream ifs(1);
    ifs.open((prefix + "polynabe_office" + ".txt").c_str());
    IdolHakScriptReader::ResultCollection set;
    IdolHakScriptReader(ifs, set, table_nocmp, table_nocmp, false)();
    
    TBufStream ofs(0x10000);
    writeScriptsRaw(set, ofs);
    ofs.save((outprefix + "polynabe_office"
                + ".bin").c_str());
  }
  
  {
    TBufStream ifs(1);
    ifs.open((prefix + "castle" + ".txt").c_str());
    IdolHakScriptReader::ResultCollection set;
    IdolHakScriptReader(ifs, set, table_nocmp, table_nocmp, false)();
    
    TBufStream ofs(0x10000);
    writeScriptsRaw(set, ofs);
    ofs.save((outprefix + "castle"
                + ".bin").c_str());
  }
  
  {
    TBufStream ifs(1);
    ifs.open((prefix + "pose_attr3" + ".txt").c_str());
    IdolHakScriptReader::ResultCollection set;
    IdolHakScriptReader(ifs, set, table_nocmp, table_nocmp, false)();
    
    TBufStream ofs(0x10000);
    writeScriptsRaw(set, ofs);
    ofs.save((outprefix + "pose_attr3"
                + ".bin").c_str());
  }
  
  {
    TBufStream ifs(1);
    ifs.open((prefix + "pose_attr_right" + ".txt").c_str());
    IdolHakScriptReader::ResultCollection set;
    IdolHakScriptReader(ifs, set, table_nocmp, table_nocmp, false)();
    
    TBufStream ofs(0x10000);
    writeScriptsRaw(set, ofs);
    ofs.save((outprefix + "pose_attr_right"
                + ".bin").c_str());
  }
  
/*  for (int i = 0; i < 2; i++)
  {
    TIfstream ifs((prefix + "menus_"
      + TStringConversion::intToString(i)
      + ".txt").c_str(), ios_base::binary);
    IdolHakScriptReader::ResultCollection set;
    IdolHakScriptReader(ifs, set, table, table, false)();
    
    TBufStream ofs(0x10000);
    if (i == 0)
      packScripts(set, ofs, 0x8000);
    else
      packScripts(set, ofs, 0x8C00);
    ofs.save((outprefix + "menus_" + TStringConversion::intToString(i)
                + ".bin").c_str());
  }
  
  {
    TIfstream ifs((prefix + "menus_cmd"
      + ".txt").c_str(), ios_base::binary);
    IdolHakScriptReader::ResultCollection set;
    IdolHakScriptReader(ifs, set, table, table, false)();
    
    TBufStream ofs(0x10000);
    packScripts(set, ofs, 0x9000);
    ofs.save((outprefix + "menus_cmd"
                + ".bin").c_str());
  }
  
  {
    TIfstream ifs((prefix + "tilemaps"
      + ".txt").c_str(), ios_base::binary);
    IdolHakScriptReader::ResultCollection set;
    IdolHakScriptReader(ifs, set, table, table, false)();
    
    TBufStream ofs(0x10000);
    packScripts(set, ofs, 0x8000);
    ofs.save((outprefix + "tilemaps1"
                + ".bin").c_str());
  }
  
  {
    TIfstream ifs((prefix + "tilemaps2"
      + ".txt").c_str(), ios_base::binary);
    IdolHakScriptReader::ResultCollection set;
    IdolHakScriptReader(ifs, set, table, table, false)();
    
    TBufStream ofs(0x10000);
    packScripts(set, ofs, 0x8400);
    ofs.save((outprefix + "tilemaps2"
                + ".bin").c_str());
  }
  
  {
    TIfstream ifs((prefix + "tilemaps3"
      + ".txt").c_str(), ios_base::binary);
    IdolHakScriptReader::ResultCollection set;
    IdolHakScriptReader(ifs, set, table, table, false)();
    
    TBufStream ofs(0x10000);
    packScripts(set, ofs, 0x9000);
    ofs.save((outprefix + "tilemaps3"
                + ".bin").c_str());
  }
  
  {
    TIfstream ifs((prefix + "tilemaps4"
      + ".txt").c_str(), ios_base::binary);
    IdolHakScriptReader::ResultCollection set;
    IdolHakScriptReader(ifs, set, table, table, false)();
    
    TBufStream ofs(0x10000);
    packScripts(set, ofs, 0x9F00);
    ofs.save((outprefix + "tilemaps4"
                + ".bin").c_str());
  }
  
  {
    TIfstream ifs((prefix + "tilemaps_intro"
      + ".txt").c_str(), ios_base::binary);
    IdolHakScriptReader::ResultCollection set;
    IdolHakScriptReader(ifs, set, table, table, false)();
    
    TBufStream ofs(0x10000);
    packScripts(set, ofs, 0x8000);
    ofs.save((outprefix + "tilemaps_intro"
                + ".bin").c_str());
  }
  
  {
    TIfstream ifs((prefix + "intro_text"
      + ".txt").c_str(), ios_base::binary);
    IdolHakScriptReader::ResultCollection set;
    IdolHakScriptReader(ifs, set, table, table, false)();
    
    TBufStream ofs(0x10000);
    packScripts(set, ofs, 0xBF00);
    ofs.save((outprefix + "intro_text"
                + ".bin").c_str());
  } */
  
/*  TBufStream ofs1(0x10000);
  TBufStream ofs2(0x10000);
  TBufStream ofs3(0x10000);
  TBufStream ofs4(0x10000);
  
//  generateRegionSubset(0, splitPoint, ofs1);
//  generateRegionSubset(splitPoint, numScriptRegions, ofs2);
  generateRegionSubset(0, 2, ofs1);
  generateRegionSubset(2, 3, ofs2);
  generateRegionSubset(3, 5, ofs3);
  generateRegionSubset(5, numScriptRegions, ofs4);
  
  ofs1.save((outprefix + "script_0.bin").c_str());
  ofs2.save((outprefix + "script_1.bin").c_str());
  ofs3.save((outprefix + "script_2.bin").c_str());
  ofs4.save((outprefix + "script_3.bin").c_str()); */
  
/*  TOfstream ofs(argv[3], ios_base::binary);
  for (IdolHakScriptReader::ResultCollection::iterator it = results.begin();
       it != results.end();
       ++it) {
    string result = it->str;
    for (unsigned int i = 0; i < result.size(); i++) {
      ofs.put(result[i]);
    }
  } */
  
  return 0;
}
