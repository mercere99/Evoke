# Flags to use regardless of compiler
CFLAGS_all := -Wall -std=c++11 -I../Empirical/

# Native compiler information
CXX_nat := g++-4.8
CFLAGS_nat := -g $(CFLAGS_all)

# Emscripten compiler information
CXX_web := emcc
OFLAGS_web := -g4 -Werror -s TOTAL_MEMORY=67108864 # -s SAFE_HEAP=1
# OFLAGS_web := -Oz -Werror -DNDEBUG -s TOTAL_MEMORY=67108864
CFLAGS_web := $(CFLAGS_all) $(OFLAGS_web) --js-library ../Empirical/kinetic/library_kinetic.js  -s EXPORTED_FUNCTIONS="['_evokeMain', '_empJSDoCallback']"

default: evoke
web: evoke.js
all: evoke evoke.js

evoke:	drivers/command_line.cc
	$(CXX_nat) $(CFLAGS_nat) drivers/command_line.cc -o evoke

evoke.js: drivers/html.cc
	$(CXX_web) $(CFLAGS_web) drivers/html.cc -o evoke.js

GT_SRC	:= tests/grid_test.cc
grid_test: $(GT_SRC)
	$(CXX_web) $(CFLAGS_web) -o tests/grid_test.js $(GT_SRC)

clean:
	rm -f evoke evoke.js *.js.map *~ *.o */*.o
