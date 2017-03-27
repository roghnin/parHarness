IDIR =./
CC=g++
JEMALLOC_PATH = jemalloc

# -DLEVEL1_DCACHE_LINESIZE detects the cache line size and passes it in as a compiler flag

CFLAGS=-I$(IDIR) -I ./include -m32 -Wno-write-strings -fpermissive -pthread -std=c++0x -DLEVEL1_DCACHE_LINESIZE=`getconf LEVEL1_DCACHE_LINESIZE`

# Additional options for different builds:

# gcov build
# -fprofile-arcs -ftest-coverage -O0 
# test build
#-DNDEBUG -O3  
#debug build
#-g -pg -rdynamic 
# line by line debug coverage (access via command line: gprof -l)
#-O0 -pg -g 

CFLAGS+= -O0 -g

ODIR=./obj
LDIR =./

LIBS=-lpthread 
LIBS_JE = -L$(JEMALLOC_PATH)/lib -Wl,-rpath,$(JEMALLOC_PATH)/lib -lpthread -ljemalloc

_DEPS = HarnessUtils.hpp ParallelLaunch.hpp RContainer.hpp TestConfig.hpp DefaultHarnessTests.hpp SGLQueue.hpp SGLMap.hpp LFHashMap.hpp HazardTracker.hpp RMap.hpp ConcurrentPrimitives.hpp
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = ParallelLaunch.o TestConfig.o DefaultHarnessTests.o SGLQueue.o SGLMap.o LFHashMap.o HarnessUtils.o Recorder.o HazardTracker.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.cpp $(DEPS)
	@mkdir -p $(@D)
	$(CC) -c -o $@ $< $(CFLAGS)

all: harness library

harness: $(ODIR)/Harness.o  $(OBJ)
	g++ -o $@ $^ $(CFLAGS) $(LIBS)

harness_je: $(ODIR)/Harness.o  $(OBJ)
	g++ -o $@ $^ $(CFLAGS) $(LIBS_JE)

library: $(OBJ)
	ar rcs libharness.a $(OBJ)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ harness libharness.a

