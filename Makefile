CC=g++
COPTS=--std=c++11 -g -fPIC -Wall

GEN=gen
OBJ=obj
LIB=lib

CXXDIR=./vendor/cxxtest-4.3
CXXTESTGEN=$(CXXDIR)/bin/cxxtestgen
CXXOPTS=--error-printer --have-eh

NGYPDIR=./node_modules/node-gyp/bin
NGYP=$(NGYPDIR)/node-gyp.js

MOCHADIR=./node_modules/mocha/bin
MOCHA=$(MOCHADIR)/mocha

PINC=include/petard
PSRC=src/petard
PFILES=type value block_builder function_builder switch_builder code_unit
POBJS=$(addprefix $(OBJ)/,$(addsuffix .o,$(PFILES)))

UINC=include/utils
USRC=src/utils
UFILES=llvm_utils
UOBJS=$(addprefix $(OBJ)/,$(addsuffix .o,$(UFILES)))

LLVM_CONFIG=llvm-config-3.6
LLVMINC=`$(LLVM_CONFIG) --includedir`
LLVMLIBS=`$(LLVM_CONFIG) --libs core native support bitwriter mcjit executionengine` `$(LLVM_CONFIG) --ldflags --system-libs`

TESTMAIN=runner

default: all

all: libpetard petard_tests node_tests

test: petard_tests node_tests

libpetard: dirs $(LIB)/libpetard.a $(LIB)/libpetard.so

$(LIB)/libpetard.so: $(POBJS) $(UOBJS)
	$(CC) -shared -o $@ $^

$(LIB)/libpetard.a: $(POBJS) $(UOBJS)
	ar rs $@ $^

petard_tests: dirs $(TESTMAIN)
	LD_LIBRARY_PATH=$(LIB) ./$(TESTMAIN)

$(TESTMAIN): $(OBJ)/$(TESTMAIN).o
	$(CC) -o $@ $^ -L$(LIB) -lpetard $(LLVMLIBS)

$(OBJ)/$(TESTMAIN).o: $(GEN)/$(TESTMAIN).cpp
	$(CC) -c -o $@ $(COPTS) -I$(CXXDIR) -I$(PINC) -I$(LLVMINC) $<

$(GEN)/$(TESTMAIN).cpp: test/petard/type/void_test.h test/petard/type/int_test.h test/petard/type/float_test.h
	$(CXXTESTGEN) $(CXXOPTS) -o $@ $^

$(OBJ)/%.o: $(PSRC)/%.cpp
	$(CC) -c -o $@ $(COPTS) -I$(PINC) -I$(UINC) -I$(LLVMINC) $<

$(OBJ)/%.o: $(USRC)/%.cpp
	$(CC) -c -o $@ $(COPTS) -I$(UINC) -I$(LLVMINC) $<

dirs: $(OBJ) $(GEN) $(LIB)

$(GEN):
	mkdir -p $(GEN)

$(OBJ):
	mkdir -p $(OBJ)

$(LIB):
	mkdir -p $(LIB)

node_tests: node_build node_test

node_build:
	LLVM_CONFIG=$(LLVM_CONFIG) $(NGYP) configure
	LLVM_CONFIG=$(LLVM_CONFIG) $(NGYP) build
node_test:
	LD_LIBRARY_PATH=$(LIB) $(MOCHA) --reporter spec

clean: node_clean petard_clean

node_clean:
	$(NGYP) clean
petard_clean:
	rm -rf $(OBJ) $(GEN) $(LIB) $(TESTMAIN)
