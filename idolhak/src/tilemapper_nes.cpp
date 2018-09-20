#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include "util/TIniFile.h"
#include "util/TStringConversion.h"
#include "util/TFreeSpace.h"
#include "util/TFileManip.h"
#include "util/TArray.h"
#include "util/TByte.h"
#include "util/TGraphic.h"
#include "util/TPngConversion.h"
#include "nes/NesPattern.h"
#include "nes/NesPatternTable.h"
#include "nes/NesPalette.h"
#include "nes/NesTilemap.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <cctype>
#include <cstring>

using namespace std;
using namespace BlackT;
using namespace Nes;

// stupid and inefficient, but good enough for what we're doing
typedef vector<int> Blacklist;

Blacklist blacklist;
vector<int> backgroundedTileIndices;
vector<int> includedTileIndices;
int loadAddr = 0;

int readIntString(const string& src, int* pos) {
  string numstr;
  while (*pos < src.size()) {
    // accept "x" for hex
    if (!isalnum(src[*pos]) && !(src[*pos] == 'x')) break;
    else {
      numstr += src[(*pos)++];
    }
  }
  
  if (*pos < src.size()) ++(*pos);
  
  return TStringConversion::stringToInt(numstr);
}

void readBlacklist(Blacklist& blacklist, const string& src) {
  int pos = 0;
  
  while ((pos < src.size())) {
    int next = readIntString(src, &pos);
    
    // check if this is a range
    if ((pos < src.size()) && (src[(pos - 1)] == '-')) {
      int next2 = readIntString(src, &pos);
      for (unsigned int i = next; i <= next2; i++) {
        blacklist.push_back(i);
      }
    }
    else {
      blacklist.push_back(next);
    }
  }
}

void readWhitelist(Blacklist& blacklist, const string& src) {
  int pos = 0;
  
  while ((pos < src.size())) {
    int next = readIntString(src, &pos);
    
    // check if this is a range
    if ((pos < src.size()) && (src[(pos - 1)] == '-')) {
      int next2 = readIntString(src, &pos);
      for (unsigned int i = next; i <= next2; i++) {
        for (unsigned int j = 0; j < blacklist.size(); j++) {
          if (blacklist[j] == i) {
            blacklist.erase(blacklist.begin() + j);
            break;
          }
        }
      }
    }
    else {
      for (unsigned int j = 0; j < blacklist.size(); j++) {
        if (blacklist[j] == next) {
          blacklist.erase(blacklist.begin() + j);
          break;
        }
      }
    }
  }
}

bool isBlacklisted(const Blacklist& blacklist, int value) {
  for (unsigned int i = 0; i < blacklist.size(); i++) {
    if (blacklist[i] == value) {
      return true;
    }
  }
  
  return false;
}

typedef int NesTileId;

int processTile(const TGraphic& srcG, int x, int y,
                const NesPatternTable& vram,
                NesTileId* dstId,
                vector<NesPattern>& rawTiles) {
  
  NesPattern pattern;
  
/*  int paletteNum = 0;
  // If palette forcing is on, use the specified palette
  if (forcePaletteLine != -1) {
    paletteNum = forcePaletteLine;
    int result = pattern.fromColorGraphicGG(
                             srcG, vram.getPalette(forcePaletteLine),
                             x, y);
    if (result != 0) paletteNum = NesPatternTable::numPaletteLines;
  }
  else {
    // Find a palette that matches this part of the image.
    // If none matches, we can't proceed.
    for ( ; paletteNum < NesPatternTable::numPaletteLines; paletteNum++) {
      int result = pattern.fromColorGraphicGG(
                               srcG, vram.getPalette(paletteNum),
                               x, y);
      if (result == 0) break;
    }
  }
  
  if (paletteNum >= NesPatternTable::numPaletteLines) return -1;
  
  dstId->palette = paletteNum; */
  
  pattern.fromGrayscaleGraphic(srcG, x, y);
  
  // Determine if target graphic matches any existing tile.
  // If so, we don't need to add a new tile.
  bool foundMatch = false;
  
  // Check backgrounded tiles
  for (int i = 0; i < backgroundedTileIndices.size(); i++) {
    int index = backgroundedTileIndices[i];
    const NesPattern& checkPattern
      = vram.tile(index);
    
    if (pattern == checkPattern) {
      *dstId = index;
      foundMatch = true;
      break;
    }
  }
  
  if (foundMatch) goto done;
  
  // Check existing tiles
  for (int i = 0; i < rawTiles.size(); i++) {
    if (pattern == rawTiles[i]) {
      *dstId = i + 0x100;
      
      foundMatch = true;
      break;
    }
  }

done:

  // if we found a match, we're done
  if (foundMatch) {
//    cout << dstId << endl;
    return 0;
  }
  
  // otherwise, add a new tile
  
  rawTiles.push_back(pattern);
  
  *dstId = (rawTiles.size() - 1) + 0x100;
  
  return 0;
}

int main(int argc, char* argv[]) {
  
  
  // Input:
  // * output filename for graphics
  //   (tilemaps assumed from input names)
  // * raw graphic(s)
  // * target offset in VRAM of tilemapped data
  // * optional output prefix
  // * palette
  //   (don't think we need this on a per-file basis?)
  
  if (argc < 2) {
    cout << "NES tilemap generator" << endl;
    cout << "Usage: " << argv[0] << " <scriptfile>" << endl;
    
    return 0;
  }
  
  TIniFile script = TIniFile(string(argv[1]));
  
  if (!script.hasSection("Properties")) {
    cerr << "Error: Script has no 'Properties' section" << endl;
    return 1;
  }
  
  string paletteName;
  string destName;
//  int loadAddr = 0;
  int minTiles = 0;
  int maxTiles = -1;
  
  // Mandatory fields
  
//  if (!script.hasKey("Properties", "palette")) {
//    cerr << "Error: Properties.palette is undefined" << endl;
//    return 1;
//  }
//  paletteName = script.valueOfKey("Properties", "palette");
  
  if (!script.hasKey("Properties", "dest")) {
    cerr << "Error: Properties.dest is undefined" << endl;
    return 1;
  }
  destName = script.valueOfKey("Properties", "dest");
  
  // Optional fields
  
  if (script.hasKey("Properties", "palette")) {
    paletteName = script.valueOfKey("Properties", "palette");
  }
  
  if (script.hasKey("Properties", "loadAddr")) {
    loadAddr = TStringConversion::stringToInt(script.valueOfKey(
      "Properties", "loadAddr"));
  }
  
  if (script.hasKey("Properties", "minTiles")) {
    minTiles = TStringConversion::stringToInt(script.valueOfKey(
      "Properties", "minTiles"));
  }
  
  if (script.hasKey("Properties", "maxTiles")) {
    maxTiles = TStringConversion::stringToInt(script.valueOfKey(
      "Properties", "maxTiles"));
  }
  
  if (script.hasKey("Properties", "blacklist")) {
    string blacklistStr = script.valueOfKey("Properties", "blacklist");
    readBlacklist(blacklist, blacklistStr);
  }
  
//  for (int i = 0; i < blacklist.size(); i++) {
//    cout << blacklist[i] << endl;
//  }

  NesPatternTable vram;
  
  // get palette
//  SmsPalette palette;
//  {
//    TIfstream ifs(paletteName.c_str(), ios_base::binary);
//    SmsPalette pal;
//    pal.readGG(ifs);
//    vram.setTilePalette(pal);
//    
//    if (ifs.remaining() > 0) {
//      pal.readGG(ifs);
//      vram.setSpritePalette(pal);
//    }
//  }

  int includeSize = 0;
  
  // If any tiles have been backgrounded, load them to VRAM at the
  // specified positions
  for (SectionKeysMap::const_iterator it = script.cbegin();
       it != script.cend();
       ++it) {
    // iterate over all sections beginning with "Background"
    string cmpstr = "Background";
    string cmpstr2 = "Include";
    bool isBackground = false;
    bool isInclude = false;
    if (it->first.substr(0, cmpstr.size()).compare(cmpstr) == 0) {
      isBackground = true;
    }
    else if (it->first.substr(0, cmpstr2.size()).compare(cmpstr2) == 0) {
      isInclude = true;
    }
    else continue;
    
    string sectionName = it->first;
    
    if (!script.hasKey(sectionName, "source")) {
      cerr << "Error: " << sectionName << ".source is undefined" << endl;
      return 1;
    }
    string sourceStr = script.valueOfKey(sectionName, "source");
    
    if (!script.hasKey(sectionName, "loadaddr")) {
      cerr << "Error: " << sectionName << ".loadaddr is undefined" << endl;
      return 1;
    }
    int grploadaddr = TStringConversion::stringToInt(
      script.valueOfKey(sectionName, "loadaddr"));
    
    TIfstream ifs(sourceStr.c_str(), ios_base::binary);
    while (ifs.remaining() > 0) {
      NesPattern pattern;
      pattern.read(ifs);
      
      if (isBackground) {
        backgroundedTileIndices.push_back(grploadaddr);
      }
      else if (isInclude) {
        includedTileIndices.push_back(grploadaddr);
        // blacklist included tiles by default
        if (!isBlacklisted(blacklist, grploadaddr)) blacklist.push_back(grploadaddr);
        ++includeSize;
      }
      
      vram.tile(grploadaddr) = pattern;
      ++grploadaddr;
    }
    
  }
  
  // read the whitelist (we do this now so the user can override any included
  // tiles as needed, since they were blacklisted by default)
  
  if (script.hasKey("Properties", "whitelist")) {
    string whitelistStr = script.valueOfKey("Properties", "whitelist");
    readWhitelist(blacklist, whitelistStr);
  }
  
  // 1. go through all source images and analyze for matching tiles
  // 2. create per-image tilemap corresponding to raw tile indices
  // 3. map raw tile indices to actual tile positions (accounting for
  //    blacklist, etc.)
  // 4. generate final tilemaps by mapping raw indices to final positions
  
  map<string, NesTilemap> rawTilemaps;
  vector<NesPattern> rawTiles;
  
  for (SectionKeysMap::const_iterator it = script.cbegin();
       it != script.cend();
       ++it) {
    // iterate over all sections beginning with "Tilemap"
    string cmpstr = "Tilemap";
    if (it->first.substr(0, cmpstr.size()).compare(cmpstr) != 0) continue;
    string sectionName = it->first;
    
    string sourceStr;
    
    // mandatory fields
    
    if (!script.hasKey(sectionName, "source")) {
      cerr << "Error: " << sectionName << ".source is undefined" << endl;
      return 1;
    }
    sourceStr = script.valueOfKey(sectionName, "source");
    
    // optional fields
    
    // get source graphic
    TGraphic srcG;
    TPngConversion::RGBAPngToGraphic(sourceStr, srcG);
    
    // infer tilemap dimensions from source size
    int tileW = srcG.w() / NesPattern::w;
    int tileH = srcG.h() / NesPattern::h;
    
    NesTilemap tilemap;
    tilemap.resize(tileW, tileH);
    
//    cout << tileW << " " << tileH << endl;

    for (int j = 0; j < tilemap.h(); j++) {
      for (int i = 0; i < tilemap.w(); i++) {
        NesTileId tileId = tilemap.data(i, j);
      
        int result = processTile(srcG, i * NesPattern::w, j * NesPattern::h,
                                 vram,
                                 &tileId,
                                 rawTiles);
        
        if (result != 0) {
          cerr << "Error in " << sectionName
            << ": failed processing tile (" << i << ", " << j << ")" << endl;
          return 2;
        }
        
        tilemap.setData(i, j, tileId);
      }
    }
    
    rawTilemaps[sectionName] = tilemap;
  }
  
//  cout << rawTiles.size() << endl;

  // Produce the final arrangement of tiles
  
  map<int, NesPattern> outputTiles;
  map<int, int> rawToOutputMap;
  int outputTileNum = 0;
  
//  for (int i = 0; i < blacklist.size(); i++) {
//    cout << hex << blacklist[i] << endl;
//  }
  
  for (int i = 0; i < rawTiles.size(); i++) {
    // Skip blacklisted content
    while (isBlacklisted(blacklist, outputTileNum + loadAddr)) {
//      outputTiles[outputTileNum] = NesPattern();
      ++outputTileNum;
    }
    
    outputTiles[i] = rawTiles[i];
    rawToOutputMap[i] = outputTileNum + loadAddr;
    ++outputTileNum;
  }
  
  if ((outputTileNum + loadAddr) > 0x100) {
    cerr << "Error: too many tiles in input (overran VRAM by "
      << (outputTileNum - loadAddr) << " tiles)" << endl;
    return -4;
  }
  
  // create identity mapping for backgrounded tiles
//  for (int i = 0; i < backgroundedTileIndices.size(); i++) {
//    rawToOutputMap[backgroundedTileIndices[i]] = backgroundedTileIndices[i];
//  }
  
  // Give an error if tile limit exceeded
  if (outputTiles.size() > maxTiles) {
    cerr << "Error: Tile limit exceeded (limit is "
      << maxTiles << "; generated "
      << outputTiles.size() << ")" << endl;
    return -3;
  }
  
  // Write new tiles to VRAM
  for (map<int, NesPattern>::const_iterator it = outputTiles.cbegin();
       it != outputTiles.cend();
       ++it) {
//    cerr << hex << it->first << " " << rawToOutputMap[it->first] << endl;
  
//    int tilenum = rawToOutputMap[it->first];
    
    vram.tile(rawToOutputMap[it->first]) = it->second;
  }
  
  // Write tile data
/*  {
    TOfstream ofs(destName.c_str(), ios_base::binary);
    for (map<int, NesPattern>::const_iterator it = outputTiles.cbegin();
         it != outputTiles.cend();
         ++it) {
      it->second.write(ofs);
    }
    
    // pad with extra tiles to meet minimum length
    int padTiles = minTiles - outputTiles.size();
    NesPattern pattern;
    for (int i = 0; i < padTiles; i++) {
      pattern.write(ofs);
    }
  } */
  {
    TOfstream ofs(destName.c_str(), ios_base::binary);
    
    int outputSize = std::max(includeSize, outputTileNum);
    
    for (unsigned int i = 0; i < outputSize; i++) {
      vram.tile(loadAddr + i).write(ofs);
    }
    
    // pad with extra tiles to meet minimum length
//    int padTiles = minTiles - outputTiles.size();
    int padTiles = minTiles - outputSize;
    NesPattern pattern;
    for (int i = 0; i < padTiles; i++) {
      pattern.write(ofs);
    }
  }
  
  // Update tilemaps and write to destinations
  
  for (SectionKeysMap::const_iterator it = script.cbegin();
       it != script.cend();
       ++it) {
    // iterate over all sections beginning with "Tilemap"
    string cmpstr = "Tilemap";
    if (it->first.substr(0, cmpstr.size()).compare(cmpstr) != 0) continue;
    string sectionName = it->first;
    
    if (!script.hasKey(sectionName, "dest")) {
      cerr << "Error: " << sectionName << ".dest is undefined" << endl;
      return 1;
    }
    string destStr = script.valueOfKey(sectionName, "dest");
    
    NesTilemap& tilemap = rawTilemaps[sectionName];
    
    TOfstream ofs(destStr.c_str(), ios_base::binary);
//    TBufStream ofs(tilemap.tileIds.h() * tilemap.tileIds.w() * 2);
    
    for (int j = 0; j < tilemap.h(); j++) {
      for (int i = 0; i < tilemap.w(); i++) {
        NesTileId id = tilemap.data(i, j);
        
        // non-backgrounded tiles have placeholder indices that are +0x100
        if (id >= 0x100) {
          id = rawToOutputMap[id - 0x100];
//          cerr << hex << id << endl;
        }
        
        tilemap.setData(i, j, id);
        ofs.writeu8(id);
      }
    }
  }
  
  return 0;
}
