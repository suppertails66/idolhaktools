IDIR := src
SRCDIR := src
ODIR := obj
LDIR :=

BLACKTDIR = ../blackt
LIBNESDIR = ../libnes

AR=ar
CXX=g++
# Compile only
CDEFINES = -DBLACKT_ENABLE_LIBPNG
#CLIBS = -lpng
CFLAGS = -std=gnu++11 -O2 -Wall -L${LIBNESDIR} -lnes -L${BLACKTDIR} -lblackt -lpng
CINCLUDES = -I${BLACKTDIR}/src -I${LIBNESDIR}/src
CXXFLAGS=$(CFLAGS) $(CDEFINES) $(CINCLUDES) -I$(IDIR)

#SRC := $(wildcard $(SRCDIR)/*/*.cpp)
#OBJ := $(patsubst $(SRCDIR)/%,$(ODIR)/%,$(patsubst %.cpp,%.o,$(SRC)))
#DEP := $(patsubst %.o,%.d,$(OBJ))
#LIB := libnes.a

LIBDEPS := $(LIBNESDIR)/libnes.a $(BLACKTDIR)/libblackt.a

TOOLSRCS := $(wildcard $(SRCDIR)/*.cpp)
#TOOLS = test opbextr opbinsr opbfileextr opbfileinsr dsimgconv
TOOLSINDIR := $(patsubst %.cpp,%,$(TOOLSRCS))
TOOLS := $(notdir $(TOOLSINDIR))

all: blackt libnes $(TOOLS)

blackt:
	cd ${BLACKTDIR} && $(MAKE) && cd $(CURDIR)

libnes:
	cd ${LIBNESDIR} && $(MAKE) && cd $(CURDIR)
	
#$(SRCDIR)/%: blackt libnes
#	$(CXX) $@.cpp $(OBJ) -o $(notdir $@) $(CXXFLAGS)

test: $(SRCDIR)/test.cpp $(LIBDEPS)
	$(CXX) $(SRCDIR)/$@.cpp $(OBJ) -o $@ $(CXXFLAGS)

scriptrip: $(SRCDIR)/scriptrip.cpp $(LIBDEPS)
	$(CXX) $(SRCDIR)/$@.cpp $(OBJ) -o $@ $(CXXFLAGS)

romprep: $(SRCDIR)/romprep.cpp $(LIBDEPS)
	$(CXX) $(SRCDIR)/$@.cpp $(OBJ) -o $@ $(CXXFLAGS)

romfinalize: $(SRCDIR)/romfinalize.cpp $(LIBDEPS)
	$(CXX) $(SRCDIR)/$@.cpp $(OBJ) -o $@ $(CXXFLAGS)

filepatch: $(SRCDIR)/filepatch.cpp $(LIBDEPS)
	$(CXX) $(SRCDIR)/$@.cpp $(OBJ) -o $@ $(CXXFLAGS)

fontbuild: $(SRCDIR)/fontbuild.cpp $(LIBDEPS)
	$(CXX) $(SRCDIR)/$@.cpp $(OBJ) -o $@ $(CXXFLAGS)

scriptconv: $(SRCDIR)/scriptconv.cpp $(LIBDEPS)
	$(CXX) $(SRCDIR)/$@.cpp $(OBJ) -o $@ $(CXXFLAGS)

tilemapper_nes: $(SRCDIR)/tilemapper_nes.cpp $(LIBDEPS)
	$(CXX) $(SRCDIR)/$@.cpp $(OBJ) -o $@ $(CXXFLAGS)

nes_tileundmp: $(SRCDIR)/nes_tileundmp.cpp $(LIBDEPS)
	$(CXX) $(SRCDIR)/$@.cpp $(OBJ) -o $@ $(CXXFLAGS)

nes_tiledmp: $(SRCDIR)/nes_tiledmp.cpp $(LIBDEPS)
	$(CXX) $(SRCDIR)/$@.cpp $(OBJ) -o $@ $(CXXFLAGS)

tblconv: $(SRCDIR)/tblconv.cpp $(LIBDEPS)
	$(CXX) $(SRCDIR)/$@.cpp $(OBJ) -o $@ $(CXXFLAGS)

mapconv: $(SRCDIR)/mapconv.cpp $(LIBDEPS)
	$(CXX) $(SRCDIR)/$@.cpp $(OBJ) -o $@ $(CXXFLAGS)

rawgrpconv: $(SRCDIR)/rawgrpconv.cpp $(LIBDEPS)
	$(CXX) $(SRCDIR)/$@.cpp $(OBJ) -o $@ $(CXXFLAGS)

cmptablebuild: $(SRCDIR)/cmptablebuild.cpp $(LIBDEPS)
	$(CXX) $(SRCDIR)/$@.cpp $(OBJ) -o $@ $(CXXFLAGS)

romprefinalize: $(SRCDIR)/romprefinalize.cpp $(LIBDEPS)
	$(CXX) $(SRCDIR)/$@.cpp $(OBJ) -o $@ $(CXXFLAGS)

.PHONY: blackt libnes cleanme clean

cleanme:
	rm -f $(TOOLS)

clean: cleanme
#	rm -f $(LIB)
#	rm -rf $(ODIR)
	cd ${BLACKTDIR} && $(MAKE) clean && cd $(CURDIR)
	cd ${LIBNESDIR} && $(MAKE) clean && cd $(CURDIR)