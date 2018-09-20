#ifndef IDOLHAKSCRIPTREADER_H
#define IDOLHAKSCRIPTREADER_H


#include "util/TStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include "util/TThingyTable.h"
#include <string>
#include <vector>
#include <map>
#include <fstream>

namespace Nes {


class IdolHakScriptReader {
public:

  struct ResultString {
    std::string str;
  };
  
  typedef std::vector<ResultString> ResultCollection;

  IdolHakScriptReader(BlackT::TStream& src__,
//                  BlackT::TStream& dst__,
//                  NesRom& dst__,
                  ResultCollection& dst__,
                  const BlackT::TThingyTable& thingy__,
                  const BlackT::TThingyTable& nameThingy__,
                  bool hasName__ = true,
                  bool warnOnBadX__ = false);
  
  void operator()();
protected:
  struct ThingyValueAndKey {
    std::string value;
    int key;
    
    bool operator<(const ThingyValueAndKey& src) const {
      return (value.size() > src.value.size());
    }
  };

  BlackT::TStream& src;
  ResultCollection& dst;
  BlackT::TThingyTable thingy;
  BlackT::TThingyTable nameThingy;
  bool hasName;
  int xpos;
  bool warnOnBadX;
//  std::vector<ThingyValueAndKey> thingiesBySize;
  int lineNum;
  
  BlackT::TBufStream currentScriptBuffer;
  bool nameRead;
  
  void outputNextSymbol(BlackT::TStream& ifs);
  
  bool checkSymbol(BlackT::TStream& ifs, std::string& symbol);
  
  void flushActiveScript();
  
  void processDirective(BlackT::TStream& ifs);
  void processLoadTable(BlackT::TStream& ifs);
  void processEndMsg(BlackT::TStream& ifs);
  void processIncBin(BlackT::TStream& ifs);
  
  void loadThingy(const BlackT::TThingyTable& thingy__);
  
};


}


#endif
