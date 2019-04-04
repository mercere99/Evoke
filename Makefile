# Flags to use regardless of compiler
CFLAGS_all := -Wall -Wno-unused-function -std=c++17 -I../Empirical/source/ -I./

# Native compiler information
CXX_native := g++
#CFLAGS_native := -g $(CFLAGS_all)
CFLAGS_native_debug := -g  $(CFLAGS_all) -pedantic -DEMP_TRACK_MEM  -Wnon-virtual-dtor -Wcast-align -Woverloaded-virtual -Wconversion -Weffc++
#CFLAGS_native_debug := -g -DEMP_ABORT_PTR_DELETE=3227 $(CFLAGS_all) -pedantic -DEMP_TRACK_MEM  -Wnon-virtual-dtor -Wcast-align -Woverloaded-virtual -Wconversion -Weffc++
CFLAGS_native := -O3 -DNDEBUG $(CFLAGS_all)

# Emscripten compiler information
CXX_web := emcc
#OFLAGS_web := -g4 -pedantic -Wno-dollar-in-identifier-extension -s TOTAL_MEMORY=67108864 # -s DEMANGLE_SUPPORT=1 # -s SAFE_HEAP=1
OFLAGS_web := -DNDEBUG -s TOTAL_MEMORY=67108864
#OFLAGS_web := -Oz -DNDEBUG -s TOTAL_MEMORY=67108864 -s ASSERTIONS=1
#OFLAGS_web := -Os -DNDEBUG -s TOTAL_MEMORY=67108864


# CFLAGS_web := $(CFLAGS_all) $(OFLAGS_web) --js-library ../Empirical/web/library_emp.js -s EXPORTED_FUNCTIONS="['_main', '_empCppCallback']" -s DISABLE_EXCEPTION_CATCHING=1 -s COMPILER_ASSERTIONS=1 -s NO_EXIT_RUNTIME=1 --embed-file configs
CFLAGS_web := $(CFLAGS_all) $(OFLAGS_web) --js-library ../Empirical/source/web/library_emp.js -s EXPORTED_FUNCTIONS="['_main', '_empCppCallback']" -s DISABLE_EXCEPTION_CATCHING=1 -s NO_EXIT_RUNTIME=1 --embed-file configs

default: evoke
web: evoke.js
all: evoke evoke.js

debug:	source/drivers/command_line.cc
	$(CXX_native) $(CFLAGS_native_debug) source/drivers/command_line.cc -o evoke
	@echo Debug version of command_line evoke complete.

evoke:	source/drivers/command_line.cc
	$(CXX_native) $(CFLAGS_native) source/drivers/command_line.cc -o evoke
	@echo To build the web version use: make web

evoke.js: source/drivers/html.cc
	$(CXX_web) $(CFLAGS_web) source/drivers/html.cc -o evoke.js

clean:
	rm -f evoke evoke.js *.js.map *~ source/*.o source/*/*.o

# Debugging information
#print-%: ; @echo $*=$($*)
print-%: ; @echo '$(subst ','\'',$*=$($*))'
