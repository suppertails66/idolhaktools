#include "idolhak/IdolHakScriptReader.h"
#include "util/TBufStream.h"
#include "util/TStringConversion.h"
#include "util/TParse.h"
#include "exception/TGenericException.h"
#include <cctype>
#include <algorithm>
#include <string>
#include <iostream>

using namespace BlackT;

namespace Nes {


const static int scriptBufferCapacity = 0x10000;

IdolHakScriptReader::IdolHakScriptReader(
                  BlackT::TStream& src__,
                  ResultCollection& dst__,
                  const BlackT::TThingyTable& thingy__,
                  const BlackT::TThingyTable& nameThingy__,
                  bool hasName__,
                  bool warnOnBadX__)
  : src(src__),
    dst(dst__),
    thingy(thingy__),
    nameThingy(nameThingy__),
    hasName(hasName__),
    xpos(0),
    warnOnBadX(warnOnBadX__),
    lineNum(0),
    currentScriptBuffer(scriptBufferCapacity),
    nameRead(false) {
  loadThingy(thingy__);
//  spaceOfs.open((outprefix + "msg_space.txt").c_str());
//  indexOfs.open((outprefix + "msg_index.txt").c_str());
}

void IdolHakScriptReader::operator()() {
  while (!src.eof()) {
    std::string line;
    src.getLine(line);
    ++lineNum;
    
    if (line.size() <= 0) continue;
    
    // discard lines containing only ASCII spaces and tabs
//    bool onlySpace = true;
//    for (int i = 0; i < line.size(); i++) {
//      if ((line[i] != ' ')
//          && (line[i] != '\t')) {
//        onlySpace = false;
//        break;
//      }
//    }
//    if (onlySpace) continue;
    
    TBufStream ifs(line.size());
    ifs.write(line.c_str(), line.size());
    ifs.seek(0);
    
    // check for special stuff
    if (ifs.peek() == '#') {
      // directives
      ifs.get();
      processDirective(ifs);
      continue;
    }
    
    while (!ifs.eof()) {
      // check for comments
      if ((ifs.remaining() >= 2)
          && (ifs.peek() == '/')) {
        ifs.get();
        if (ifs.peek() == '/') break;
        else ifs.unget();
      }
      
      outputNextSymbol(ifs);
    }
  }
}
  
void IdolHakScriptReader::loadThingy(const BlackT::TThingyTable& thingy__) {
  thingy = thingy__;
}
  
void IdolHakScriptReader::outputNextSymbol(TStream& ifs) {
  // literal value
  if ((ifs.remaining() >= 5)
      && (ifs.peek() == '<')) {
    int pos = ifs.tell();
    
    ifs.get();
    if (ifs.peek() == '$') {
      ifs.get();
      std::string valuestr = "0x";
      valuestr += ifs.get();
      valuestr += ifs.get();
      
      if (ifs.peek() == '>') {
        ifs.get();
        int value = TStringConversion::stringToInt(valuestr);
        
//        dst.writeu8(value);
        currentScriptBuffer.writeu8(value);

        return;
      }
    }
    
    // not a literal value
    ifs.seek(pos);
  }
  
  

/*  for (int i = 0; i < thingiesBySize.size(); i++) {
    if (checkSymbol(ifs, thingiesBySize[i].value)) {
      int symbolSize;
      if (thingiesBySize[i].key <= 0xFF) symbolSize = 1;
      else if (thingiesBySize[i].key <= 0xFFFF) symbolSize = 2;
      else if (thingiesBySize[i].key <= 0xFFFFFF) symbolSize = 3;
      else symbolSize = 4;
      
//      dst.writeInt(thingiesBySize[i].key, symbolSize,
//        EndiannessTypes::big, SignednessTypes::nosign);
      currentScriptBuffer.writeInt(thingiesBySize[i].key, symbolSize,
        EndiannessTypes::big, SignednessTypes::nosign);
        
      // when terminator reached, flush script to ROM stream
//      if (thingiesBySize[i].key == 0) {
//        flushActiveScript();
//      }
      
      return;
    }
  } */
  
  TThingyTable::MatchResult result;
  if (hasName && !nameRead) {
    result = nameThingy.matchTableEntry(ifs);
    nameRead = true;
  }
  else {
    result = thingy.matchTableEntry(ifs);
  }
  
  if (result.id != -1) {
//    std::cerr << std::dec << lineNum << " " << std::hex << result.id << " " << result.size << std::endl;
  
    int symbolSize;
    if (result.id <= 0xFF) symbolSize = 1;
    else if (result.id <= 0xFFFF) symbolSize = 2;
    else if (result.id <= 0xFFFFFF) symbolSize = 3;
    else symbolSize = 4;
    
    currentScriptBuffer.writeInt(result.id, symbolSize,
      EndiannessTypes::big, SignednessTypes::nosign);
    
//    if ((result.id == 0xFF) || (result.id == 0xFE) || (result.id == 0xFD)
//        || (result.id == 0xFC) || (result.id == 0xFB) || (result.id == 0xF9)) {
    if (result.id == 0xFC) {
      
    }
    else if ((result.id >= 0xF9)) {
      xpos = 0;
    }
//    else if (result.id == 0xF9) {
//      
//    }
    else {
      // compressed
      if ((result.id >= 0xC8) && (result.id <= 0xF8)) {
        xpos += 2;
      }
      // uncompressed
      else {
        ++xpos;
      }
    }
    
    if (warnOnBadX && (xpos >= 17)
//        && ((result.id == 0xFF) || (result.id < 0xF9))
        ) {
      std::cerr << "WARNING: possible overflow on line "
        << lineNum << std::endl;
    }
    
    return;
  }
  
  std::string remainder;
  ifs.getLine(remainder);
  
  // if we reached end of file, this is not an error: we're done
  if (ifs.eof()) return;
  
  throw TGenericException(T_SRCANDLINE,
                          "IdolHakScriptReader::outputNextSymbol()",
                          "Line "
                            + TStringConversion::intToString(lineNum)
                            + ":\n  Couldn't match symbol at: '"
                            + remainder
                            + "'");
}
  
void IdolHakScriptReader::flushActiveScript() {
  int outputSize = currentScriptBuffer.size();
  
  // output message binary
/*  std::string blockStartString = TStringConversion::intToString(blockStart,
                 TStringConversion::baseHex);
  currentScriptBuffer.seek(0);
  std::string filename =
    outprefix
    + std::string("msg_")
    + blockStartString
    + ".bin";
  TOfstream ofs(filename.c_str(), std::ios_base::binary);
  ofs.write(currentScriptBuffer.data().data(), currentScriptBuffer.size()); */
  
  ResultString result;
  currentScriptBuffer.seek(0);
  while (!currentScriptBuffer.eof()) {
    result.str += currentScriptBuffer.get();
  }
  
  dst.push_back(result);
  
  // clear script buffer
  currentScriptBuffer = TBufStream(scriptBufferCapacity);
  nameRead = false;
  xpos = 0;
}
  
bool IdolHakScriptReader::checkSymbol(BlackT::TStream& ifs, std::string& symbol) {
  if (symbol.size() > ifs.remaining()) return false;
  
  int startpos = ifs.tell();
  for (int i = 0; i < symbol.size(); i++) {
    if (symbol[i] != ifs.get()) {
      ifs.seek(startpos);
      return false;
    }
  }
  
  return true;
}
  
void IdolHakScriptReader::processDirective(BlackT::TStream& ifs) {
  TParse::skipSpace(ifs);
  
  std::string name = TParse::matchName(ifs);
  TParse::matchChar(ifs, '(');
  
  for (int i = 0; i < name.size(); i++) {
    name[i] = toupper(name[i]);
  }
  
  if (name.compare("LOADTABLE") == 0) {
    processLoadTable(ifs);
  }
  else if (name.compare("END") == 0) {
    processEndMsg(ifs);
  }
  else if (name.compare("INCBIN") == 0) {
    processIncBin(ifs);
  }
  else {
    throw TGenericException(T_SRCANDLINE,
                            "IdolHakScriptReader::processDirective()",
                            "Line "
                              + TStringConversion::intToString(lineNum)
                              + ":\n  Unknown directive: "
                              + name);
  }
  
  TParse::matchChar(ifs, ')');
}

void IdolHakScriptReader::processLoadTable(BlackT::TStream& ifs) {
  std::string tableName = TParse::matchString(ifs);
  TThingyTable table(tableName);
  loadThingy(table);
}

void IdolHakScriptReader::processEndMsg(BlackT::TStream& ifs) {
  flushActiveScript();
}

void IdolHakScriptReader::processIncBin(BlackT::TStream& ifs) {
  std::string filename = TParse::matchString(ifs);
  TBufStream src(1);
  src.open(filename.c_str());
  currentScriptBuffer.writeFrom(src, src.size());
}


}
