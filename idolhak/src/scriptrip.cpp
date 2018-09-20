#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TStringConversion.h"
#include "util/TGraphic.h"
#include "util/TPngConversion.h"
#include "util/TThingyTable.h"
#include "nes/NesRom.h"
#include "nes/NesPattern.h"
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>

using namespace std;
using namespace BlackT;
using namespace Nes;

TThingyTable table;
TThingyTable charaTable;

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

void ripScript(TStream& ifs, ostream& ofs, int limit = -1, bool hasId = true) {
  string charaname;
  string terminatorString;
  bool limitReached = false;
  
  if ((limit != -1) && (ifs.tell() >= limit)) goto done;
  
//  if (hasId)
//  {
//    TThingyTable::MatchResult match = charaTable.matchId(ifs);
//    charaname = charaTable.getEntry(match.id);
//    ofs << charaname << endl;
//  }
  
  if (hasId) {
    // if top bit of first byte is set, 2-byte sequence
    // (??? and speaker graphic ID?)
    if (((unsigned char)ifs.peek() & 0x80) != 0) {
      ofs << nextAsRaw(ifs);
      ofs << nextAsRaw(ifs);
      ofs << endl;
    }
    else {
      ofs << nextAsRaw(ifs);
      ofs << endl;
    }
  }
  
  ofs << "// ";
  
  while (true) {
//    TByte next = ifs.peek();
    
//    if (next == 0xFF) {
//      ofs << endl << endl << "#END()" << endl << endl;
//      break;
//    }
    
    if ((limit != -1) && (ifs.tell() >= limit)) {
      limitReached = true;
      break;
    }
    
    TThingyTable::MatchResult match = table.matchId(ifs);
    
    if (match.id == -1) {
//      ofs << "[???]";
      ofs << "<$" << as2bHex(ifs.readu8()) << ">";
    }
    else {
      
      string str = table.getEntry(match.id);
//      ofs << str;
      
      // terminator
      if (match.id == 0xFF) {
        terminatorString = str;
        ofs << str;
        ofs << endl;
        break;
      }
      // new speaker?
      else if (match.id == 0xF9) {
        
        string spkrstring;
        
        // if top bit of first byte is set, 2-byte sequence
        // (??? and speaker graphic ID?)
        if (((unsigned char)ifs.peek() & 0x80) != 0) {
          spkrstring += nextAsRaw(ifs);
          spkrstring += nextAsRaw(ifs);
        }
        else {
          spkrstring += nextAsRaw(ifs);
        }
        
        // commented-out
//        ofs << spkrstring << endl;
//        ofs << endl << endl;
        
        // non-commented
        ofs << endl;
        ofs << str;
        ofs << spkrstring << endl;
        ofs << "// ";
      }
      else if (match.id == 0xFA) {
        
        string spkrstring;
        spkrstring += nextAsRaw(ifs);
        
        // non-commented
        ofs << endl;
        ofs << endl;
        ofs << endl;
        ofs << str;
        ofs << spkrstring << endl;
        ofs << endl;
        ofs << "// ";
      }
      else if (match.id == 0xFC) {
        
        string spkrstring;
        spkrstring += nextAsRaw(ifs);
        
        // non-commented
        ofs << endl;
        ofs << endl;
        ofs << endl;
        ofs << str;
        ofs << spkrstring << endl;
        ofs << endl;
        ofs << "// ";
      }
      else if ((match.id == 0xFB) || (match.id == 0xFD)) {
        // FB == ?
        // FD == new box
        
        ofs << str;
//        ofs << endl << endl << "// ";
        ofs << endl << endl << endl;
        ofs << str << endl << endl;
        ofs << "// ";
      }
      // start new script (1b scriptnum within same region?)
//      else if (match.id == 0xFE) {
//        string output;
//        output += "<$" + as2bHex(ifs.readu8()) + ">";
//        ofs << output << endl;
//        terminatorString = str + output;
//        break;
//      }
      // linebreak
      else if (match.id == 0xFE) {
        ofs << str;
        ofs << endl;
        ofs << "// ";
      }
      else {
        ofs << str;
      }
    }
  }
  
  if (limitReached) {
    ofs << endl;
  }

//  ofs << charaname << endl;
  
  if (!terminatorString.empty()) {
    ofs << endl << endl << terminatorString;
  }
  
done:
  
  ofs << endl << endl << "#END()" << endl << endl;
}

void ripBank(TStream& ifs, ostream& ofs,
             int tableStart, int numEntries,
             int bankEnd = -1,
             bool hasId = true,
             int bankOffset = 0x8000) {
  int bankBase = (tableStart / 0x4000) * 0x4000;
  if (bankEnd == -1) bankEnd = bankBase + 0x4000;
  for (int i = 0; i < numEntries; i++) {
    ifs.seek(tableStart + (i * 2));
    int ptr = ifs.readu16le();
    int addr = (ptr - bankOffset) + bankBase;
    
    int limit;
    if (i != (numEntries - 1)) {
      limit = ((ifs.readu16le()) - ptr) + addr;
      if (limit <= addr) {
        cerr << "WARNING: zero length at " << hex << i << endl;
      }
    }
    else {
      limit = bankEnd;
    }
    limit = bankEnd;
    
    ifs.seek(addr);
    ofs << "// Script "
      << TStringConversion::intToString(tableStart,
          TStringConversion::baseHex)
      << "-"
      << TStringConversion::intToString(i,
          TStringConversion::baseHex)
      << " ("
      << TStringConversion::intToString(addr,
          TStringConversion::baseHex)
      << ")" << endl;
//    ripScript(ifs, ofs, limit, hasId, terminator, linebreak);
    ripScript(ifs, ofs, limit, hasId);
  }
  
  
}

void ripTilemap(TStream& ifs, ostream& ofs, TThingyTable& table,
                bool raw = false, bool implicitLinebreakAddr = false) {
  bool readFirstAddr = false;
  while (true) {
  
    ofs << "// ";
    
    // ppuaddr
    string addrstr;
    if (!implicitLinebreakAddr || !readFirstAddr) {
      addrstr += nextAsRaw(ifs);
      addrstr += nextAsRaw(ifs);
      ofs << addrstr;
      readFirstAddr = true;
    }
    
    // data
    while (true) {
      int next = (unsigned char)ifs.peek();
      if (next >= 0xFE) break;
      
      if (raw) {
        ofs << nextAsRaw(ifs);
        continue;
      }
      
      TThingyTable::MatchResult match = table.matchId(ifs);
      
      if (match.id == -1) {
  //      ofs << "[???]";
        ofs << "<$" << as2bHex(ifs.readu8()) << ">";
      }
      else {
        string str = table.getEntry(match.id);
        ofs << str;
      }
      
    }
    
    ofs << endl;
    ofs << addrstr;
//    ofs << "a";
    ofs << endl;
    
    unsigned char next = ifs.peek();
    if (next == 0xFF) {
      ofs << nextAsRaw(ifs);
      break;
    }
    else {
      ofs << nextAsRaw(ifs);
      ofs << endl;
    }
  }
  
  ofs << endl << endl;
  ofs << "#END()" << endl << endl;
}

int main(int argc, char* argv[]) {
  if (argc < 5) {
    cout << "Idol Hakkenden script extractor" << endl;
    cout << "Usage: " << argv[0]
      << " <rom> <table> <charatable> <outprefix>" << endl;
    
    return 0;
  }
  
  table.readSjis(string(argv[2]));
  charaTable.readSjis(string(argv[3]));
  string outprefix = string(argv[4]);
  
  TBufStream ifs(1);
  ifs.open(argv[1]);
  
/*  {
    cerr << "bank 0" << endl;
    ofstream ofs((outprefix + "bank0.txt").c_str(), ios_base::binary);
    ripBank(ifs, ofs, 0x1AEF, 0x21, -1, true);
  }
  
  {
    cerr << "bank 1" << endl;
    ofstream ofs((outprefix + "bank1.txt").c_str(), ios_base::binary);
    ripBank(ifs, ofs, 0x51B1, 0xEE, -1, true);
  }
  
  {
    cerr << "bank 2" << endl;
    ofstream ofs((outprefix + "bank2.txt").c_str(), ios_base::binary);
    ripBank(ifs, ofs, 0x90A4, 0xFD, -1, true);
  }
  
  {
    cerr << "bank 3" << endl;
    ofstream ofs((outprefix + "bank3.txt").c_str(), ios_base::binary);
    ripBank(ifs, ofs, 0xCF9A, 0xF4, -1, true);
  }
  
  {
    cerr << "bank 4" << endl;
    ofstream ofs((outprefix + "bank4.txt").c_str(), ios_base::binary);
    ripBank(ifs, ofs, 0x1104C, 0xE4, -1, true);
  }
  
  {
    cerr << "bank 5" << endl;
    ofstream ofs((outprefix + "bank5.txt").c_str(), ios_base::binary);
    ripBank(ifs, ofs, 0x15449, 0xEF, -1, true);
  }
  
  {
    ofstream ofs((outprefix + "menus.txt").c_str(), ios_base::binary);
    for (int i = 0; i < 0xA0; i++) {
      int pos = 0x1D8BF + (i * 7);
      
      ofs << "// Menu "
        << TStringConversion::intToString(i,
            TStringConversion::baseHex)
        << " ("
        << TStringConversion::intToString(pos,
            TStringConversion::baseHex)
        << ")" << endl;
      
      ifs.seek(pos);
      ripScript(ifs, ofs, pos + 7, false);
    }
  }
  
  {
    ofstream ofs((outprefix + "karaoke.txt").c_str(), ios_base::binary);
    ripBank(ifs, ofs, 0x192, 0x3A, -1, false);
  }
  
  {
    ofstream ofs((outprefix + "credits.txt").c_str(), ios_base::binary);
//    ripBank(ifs, ofs, 0x192, 0x3A, -1, false);
    
    TThingyTable credtbl1;
    credtbl1.readSjis("table/idolhak_cred1.tbl");
    TThingyTable credtbl2;
    credtbl2.readSjis("table/idolhak_cred2.tbl");
    TThingyTable credtbl3;
    credtbl3.readSjis("table/idolhak_cred3.tbl");
    TThingyTable credtbl4;
    credtbl4.readSjis("table/idolhak_cred4.tbl");
    
    for (int i = 0; i < 0x22; i++) {
      ifs.seek(0x3EDD + i);
      int chrIdx = ifs.readu8();
      
      TThingyTable* srctbl = NULL;
      switch (chrIdx) {
      case 0x01:
        srctbl = &credtbl1;
        break;
      case 0x09:
        srctbl = &credtbl2;
        break;
      case 0x0D:
        srctbl = &credtbl3;
        break;
      case 0x08:
        srctbl = &credtbl4;
        break;
      default:
        cerr << "Error: illegal staff roll CHR page " << chrIdx;
        return 1;
        break;
      }
      
      ifs.seek(0x3C44 + (i * 2));
      int ptr = ifs.readu16le();
      int addr = (ptr - 0x8000) + ((0x3C44 / 0x4000) * 0x4000);
      
      ofs << "// Credits entry "
        << TStringConversion::intToString(i,
            TStringConversion::baseHex)
        << " ("
        << TStringConversion::intToString(addr,
            TStringConversion::baseHex)
        << ")" << endl;
      
      ifs.seek(addr);
      ripTilemap(ifs, ofs, *srctbl, false);
    }
  }
  
  {
    ofstream ofs((outprefix + "intro.txt").c_str(), ios_base::binary);
    ifs.seek(0x206);
    for (int i = 0; i < 1; i++) {
      ofs << "// Tilemap " << TStringConversion::intToString(
        ifs.tell(), TStringConversion::baseHex) << endl;
      ripTilemap(ifs, ofs, table, false, false);
    }
  }
  
  {
    ofstream ofs((outprefix + "password.txt").c_str(), ios_base::binary);
    ifs.seek(0x9A0);
    for (int i = 0; i < 4; i++) {
      ofs << "// Tilemap " << TStringConversion::intToString(
        ifs.tell(), TStringConversion::baseHex) << endl;
      ripTilemap(ifs, ofs, table, false, false);
    }
  }
  
  {
    ofstream ofs((outprefix + "chapter1.txt").c_str(), ios_base::binary);
    
    ifs.seek(0x4A63);
    
    for (int i = 0; i < 1; i++) {
      ofs << "// Tilemap " << TStringConversion::intToString(
        ifs.tell(), TStringConversion::baseHex) << endl;
      ripTilemap(ifs, ofs, table, false, false);
    }
    
    for (int i = 0; i < 3; i++) {
      ofs << "// Tilemap " << TStringConversion::intToString(
        ifs.tell(), TStringConversion::baseHex) << endl;
      ripTilemap(ifs, ofs, table, false, true);
    }
  }
  
  {
    ofstream ofs((outprefix + "chapter2.txt").c_str(), ios_base::binary);
    ifs.seek(0x8AB0);
    for (int i = 0; i < 1; i++) {
      ofs << "// Tilemap " << TStringConversion::intToString(
        ifs.tell(), TStringConversion::baseHex) << endl;
      ripTilemap(ifs, ofs, table, false, false);
    }
    
    ifs.seek(0x1EC12);
    for (int i = 0; i < 3; i++) {
      ofs << "// Tilemap " << TStringConversion::intToString(
        ifs.tell(), TStringConversion::baseHex) << endl;
      ripTilemap(ifs, ofs, table, false, true);
    }
  }
  
  {
    ofstream ofs((outprefix + "chapter3.txt").c_str(), ios_base::binary);
    ifs.seek(0xCB37);
    for (int i = 0; i < 1; i++) {
      ofs << "// Tilemap " << TStringConversion::intToString(
        ifs.tell(), TStringConversion::baseHex) << endl;
      ripTilemap(ifs, ofs, table, false, false);
    }
    
    for (int i = 0; i < 3; i++) {
      ofs << "// Tilemap " << TStringConversion::intToString(
        ifs.tell(), TStringConversion::baseHex) << endl;
      ripTilemap(ifs, ofs, table, false, true);
    }
  }
  
  {
    ofstream ofs((outprefix + "chapter4.txt").c_str(), ios_base::binary);
    ifs.seek(0x10A1C);
    for (int i = 0; i < 1; i++) {
      ofs << "// Tilemap " << TStringConversion::intToString(
        ifs.tell(), TStringConversion::baseHex) << endl;
      ripTilemap(ifs, ofs, table, false, false);
    }
    
    for (int i = 0; i < 6; i++) {
      ofs << "// Tilemap " << TStringConversion::intToString(
        ifs.tell(), TStringConversion::baseHex) << endl;
      ripTilemap(ifs, ofs, table, false, true);
    }
  }
  
  {
    ofstream ofs((outprefix + "chapter5.txt").c_str(), ios_base::binary);
    ifs.seek(0x14DDE);
    for (int i = 0; i < 1; i++) {
      ofs << "// Tilemap " << TStringConversion::intToString(
        ifs.tell(), TStringConversion::baseHex) << endl;
      ripTilemap(ifs, ofs, table, false, false);
    }
  }
  
  {
    ofstream ofs((outprefix + "cybergong_inout.txt").c_str(), ios_base::binary);
    ifs.seek(0x43CE);
    for (int i = 0; i < 1; i++) {
      ofs << "// Tilemap " << TStringConversion::intToString(
        ifs.tell(), TStringConversion::baseHex) << endl;
      ripTilemap(ifs, ofs, table, true, true);
    }
  }
  
  {
    ofstream ofs((outprefix + "tamashikaya.txt").c_str(), ios_base::binary);
    ifs.seek(0x473A);
    for (int i = 0; i < 1; i++) {
      ofs << "// Tilemap " << TStringConversion::intToString(
        ifs.tell(), TStringConversion::baseHex) << endl;
      ripTilemap(ifs, ofs, table, true, true);
    }
  }
  
//  {
//    ofstream ofs((outprefix + "kakinabe_station_flowers.txt").c_str(), ios_base::binary);
//    ifs.seek(0x82E4);
//    for (int i = 0; i < 2; i++) {
//      ofs << "// Tilemap " << TStringConversion::intToString(
//        ifs.tell(), TStringConversion::baseHex) << endl;
//      ripTilemap(ifs, ofs, table, true, true);
//    }
//    ifs.seek(0x83BE);
//    for (int i = 0; i < 1; i++) {
//      ofs << "// Tilemap " << TStringConversion::intToString(
//        ifs.tell(), TStringConversion::baseHex) << endl;
//      ripTilemap(ifs, ofs, table, true, true);
//    }
//  }
  
  {
    ofstream ofs((outprefix + "kakinabe_market.txt").c_str(), ios_base::binary);
    ifs.seek(0x85E1);
    for (int i = 0; i < 1; i++) {
      ofs << "// Tilemap " << TStringConversion::intToString(
        ifs.tell(), TStringConversion::baseHex) << endl;
      ripTilemap(ifs, ofs, table, true, true);
    }
  }
  
  {
    ofstream ofs((outprefix + "polynabe_office.txt").c_str(), ios_base::binary);
    ifs.seek(0xC41F);
    for (int i = 0; i < 1; i++) {
      ofs << "// Tilemap " << TStringConversion::intToString(
        ifs.tell(), TStringConversion::baseHex) << endl;
      ripTilemap(ifs, ofs, table, true, true);
    }
  }
  
  {
    ofstream ofs((outprefix + "castle.txt").c_str(), ios_base::binary);
    ifs.seek(0x109DE);
    for (int i = 0; i < 1; i++) {
      ofs << "// Tilemap " << TStringConversion::intToString(
        ifs.tell(), TStringConversion::baseHex) << endl;
      ripTilemap(ifs, ofs, table, true, true);
    }
  }
  
//  {
//    ofstream ofs((outprefix + "pose_attr2.txt").c_str(), ios_base::binary);
//    ifs.seek(0x102BB);
//    for (int i = 0; i < 1; i++) {
//      ofs << "// Tilemap " << TStringConversion::intToString(
//        ifs.tell(), TStringConversion::baseHex) << endl;
//      ripTilemap(ifs, ofs, table, true, true);
//    }
//  }
  
//  {
//    ofstream ofs((outprefix + "pose_attr_wrong.txt").c_str(), ios_base::binary);
//    ifs.seek(0x141B9);
//    for (int i = 0; i < 1; i++) {
//      ofs << "// Tilemap " << TStringConversion::intToString(
//        ifs.tell(), TStringConversion::baseHex) << endl;
//      ripTilemap(ifs, ofs, table, true, true);
//    }
//  }
  
  {
    ofstream ofs((outprefix + "pose_attr_right.txt").c_str(), ios_base::binary);
    ifs.seek(0xC240);
    for (int i = 0; i < 1; i++) {
      ofs << "// Tilemap " << TStringConversion::intToString(
        ifs.tell(), TStringConversion::baseHex) << endl;
      ripTilemap(ifs, ofs, table, true, true);
    }
  } */
  
  
  return 0;
}
