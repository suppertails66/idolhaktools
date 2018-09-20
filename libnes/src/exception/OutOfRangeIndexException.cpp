#include "exception/OutOfRangeIndexException.h"
#include <string>

namespace Nes {


OutOfRangeIndexException::OutOfRangeIndexException(
      const char* nameOfSourceFile__,
      int lineNum__,
      const std::string& source__,
      int invalidIndex__)
  : BlackT::TException(nameOfSourceFile__,
                   lineNum__,
                   source__),
    invalidIndex_(invalidIndex__) { };

const char* OutOfRangeIndexException::what() const throw() {
  return "Tried to access invalid index";
}

int OutOfRangeIndexException::invalidIndex() const {
  return invalidIndex_;
}



};  
