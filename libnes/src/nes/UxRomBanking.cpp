#include "nes/UxRomBanking.h"

namespace Nes {


int UxRomBanking::prgBankSize() {
  return prgBankSize_;
}
  
int UxRomBanking::directToBankedAddressMovable(
    int directAddress) {
  return (directAddress % prgBankSize_) + movableBankOffset;
}

int UxRomBanking::directToBankNumMovable(
    int directAddress) {
  return (directAddress / prgBankSize_);
}

int UxRomBanking::bankedToDirectAddressMovable(
    int bankNum, int bankedAddress) {
  return (bankedAddress - movableBankOffset)
      + (bankNum * prgBankSize_);
}

int UxRomBanking::directToBankedAddressFixed(
    int directAddress) {
  return (directAddress % prgBankSize_) + fixedBankOffset;
}

int UxRomBanking::bankedToDirectAddressFixed(
    int bankNum, int bankedAddress) {
  return (bankedAddress - fixedBankOffset)
      + (bankNum * prgBankSize_);
}


};
