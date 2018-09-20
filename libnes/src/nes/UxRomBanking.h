#ifndef UXROMBANKING_H
#define UXROMBANKING_H


namespace Nes {


class UxRomBanking {
public:
  const static int movableBankOffset = 0x8000;
  const static int fixedBankOffset = 0xC000;
  const static int sizeOfPrgBank = 0x4000;
  
  static int prgBankSize();
  
  static int directToBankedAddressMovable(
      int directAddress);
  static int directToBankNumMovable(
      int directAddress);
  static int bankedToDirectAddressMovable(
      int bankNum, int bankedAddress);
  
  static int directToBankedAddressFixed(
      int directAddress);
  static int bankedToDirectAddressFixed(
      int bankNum, int bankedAddress);
protected:
  const static int prgBankSize_ = sizeOfPrgBank;
};


};


#endif
