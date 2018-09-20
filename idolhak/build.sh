
echo "*******************************************************************************"
echo "Setting up environment..."
echo "*******************************************************************************"

set -o errexit

BASE_PWD=$PWD
PATH=".:./asm/bin/:$PATH"
INROM="idolhak.nes"
OUTROM="idolhak_en.nes"
WLADX="./wla-dx/binaries/wla-6502"
WLALINK="./wla-dx/binaries/wlalink"

cp "$INROM" "$OUTROM"

mkdir -p out

echo "*******************************************************************************"
echo "Building tools..."
echo "*******************************************************************************"

make blackt
make libnes
make

if [ ! -f $WLADX ]; then
  
  echo "********************************************************************************"
  echo "Building WLA-DX..."
  echo "********************************************************************************"
  
  cd wla-dx
    cmake -G "Unix Makefiles" .
    make
  cd $BASE_PWD
  
fi

echo "*******************************************************************************"
echo "Doing initial ROM prep..."
echo "*******************************************************************************"

mkdir -p out
romprep "$OUTROM" "$OUTROM" "out/idolhak_chr.bin"

echo "*******************************************************************************"
echo "Patching graphics..."
echo "*******************************************************************************"

mkdir -p out/grp
nes_tileundmp rsrc/font/font_0x0000.png 256 out/grp/font_0x0000.bin
nes_tileundmp rsrc/font/font_0x1000.png 256 out/grp/font_0x1000.bin
filepatch out/idolhak_chr.bin 0x0000 out/grp/font_0x0000.bin out/idolhak_chr.bin
filepatch out/idolhak_chr.bin 0x1000 out/grp/font_0x1000.bin out/idolhak_chr.bin

echo "*******************************************************************************"
echo "Building tilemaps..."
echo "*******************************************************************************"

mkdir -p out/maps_raw
tilemapper_nes tilemappers/title.txt

mkdir -p out/maps_conv
mapconv out/maps_conv/

filepatch out/idolhak_chr.bin 0x1000 out/grp/title_grp.bin out/idolhak_chr.bin

echo "*******************************************************************************"
echo "Patching other graphics..."
echo "*******************************************************************************"

for file in rsrc/misc/*.txt; do
  bname=$(basename $file .txt)
  rawgrpconv rsrc/misc/$bname.png rsrc/misc/$bname.txt out/idolhak_chr.bin out/idolhak_chr.bin
done

echo "*******************************************************************************"
echo "Building script..."
echo "*******************************************************************************"

mkdir -p out/script
mkdir -p out/script/credits
mkdir -p out/script/maps
scriptconv script/ table/idolhak_en.tbl table/idolhak_chara.tbl out/script/

filepatch "$OUTROM" 0x43CE out/script/cybergong_inout.bin "$OUTROM"
filepatch "$OUTROM" 0x473A out/script/tamashikaya.bin "$OUTROM"
filepatch "$OUTROM" 0x85E1 out/script/kakinabe_market.bin "$OUTROM"
filepatch "$OUTROM" 0xC41F out/script/polynabe_office.bin "$OUTROM"
filepatch "$OUTROM" 0x109DE out/script/castle.bin "$OUTROM"
filepatch "$OUTROM" 0x14585 out/script/castle.bin "$OUTROM"

echo "*******************************************************************************"
echo "Building compression table..."
echo "*******************************************************************************"

mkdir -p out/cmptbl
cmptablebuild table/idolhak_en.tbl out/cmptbl/cmptbl.bin

echo "********************************************************************************"
echo "Applying ASM patches..."
echo "********************************************************************************"

mkdir -p "out/asm"
cp "$OUTROM" "asm/idolhak.nes"

cd asm
  # apply hacks
  ../$WLADX -I ".." -o "boot.o" "boot.s"
  ../$WLALINK -v linkfile idolhak_build.nes
cd $BASE_PWD

mv -f "asm/idolhak_build.nes" "$OUTROM"
rm "asm/idolhak.nes"
rm asm/*.o

echo "*******************************************************************************"
echo "Finalizing ROM..."
echo "*******************************************************************************"

romfinalize "$OUTROM" "out/idolhak_chr.bin" "$OUTROM"

echo "*******************************************************************************"
echo "Success!"
echo "Output file:" $OUTROM
echo "*******************************************************************************"
