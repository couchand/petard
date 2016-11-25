CC=g++
COPTS=--std=c++11

CXXDIR=./vendor/cxxtest-4.3
CXXTESTGEN=$(CXXDIR)/bin/cxxtestgen
CXXOPTS=--error-printer --have-eh

PINC=include/petard
PSRC=src/petard

LLVM_CONFIG=llvm-config-3.6
LLVMINC=`$(LLVM_CONFIG) --includedir`
LLVMLIBS=`$(LLVM_CONFIG) --libs core native support bitwriter mcjit` `$(LLVM_CONFIG) --ldflags --system-libs`

GEN=gen
OBJ=obj

TESTMAIN=runner

default: petard_tests

all: petard_tests node_gyp

petard_tests: dirs $(TESTMAIN)
	./$(TESTMAIN)

$(TESTMAIN): $(OBJ)/$(TESTMAIN).o $(OBJ)/type.o
	$(CC) -o $@ $^ $(LLVMLIBS)

$(OBJ)/$(TESTMAIN).o: $(GEN)/$(TESTMAIN).cpp
	$(CC) -c -o $@ $(COPTS) -I$(CXXDIR) -I$(PINC) -I$(LLVMINC) $<

$(GEN)/$(TESTMAIN).cpp: test/petard/type/void_test.h test/petard/type/int_test.h test/petard/type/float_test.h
	$(CXXTESTGEN) $(CXXOPTS) -o $@ $^

$(OBJ)/%.o: $(PSRC)/%.cpp
	$(CC) -c -o $@ $(COPTS) -I$(PINC) -I$(LLVMINC) $<

dirs: $(OBJ) $(GEN)

$(GEN):
	mkdir $(GEN)

$(OBJ):
	mkdir $(OBJ)

node:
	npm run clean
	npm run build
	npm run test

clean:
	rm -rf $(OBJ) $(GEN) $(TESTMAIN)
