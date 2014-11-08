# Flags to use regardless of compiler
CFLAGS_all := -Wall -std=c++11

# Native compiler information
CXX_nat := g++-4.8
CFLAGS_nat := -g $(CFLAGS_all)

# Emscripten compiler information
CXX_web := emcc
OFLAGS_web := -g4 -Werror -DEMP_DEBUG -I/Users/charles/dev/GitHub/Empirical/
CFLAGS_web := $(CFLAGS_all) $(OFLAGS_web) --js-library ../Empirical/kinetic/library_kinetic.js  -s EXPORTED_FUNCTIONS="['_empMain', '_empJSDoCallback', '_empLoadString']"

default: evoke
all: evoke evoke.js

# What are the source files we are using?
SRC	:= drivers/command_line.cc
OBJ	:= $(SRC:.cc=.o)

.cc.o:
	$(CXX_nat) $(CFLAGS_nat) -c $< -o $*.o

evoke:	$(OBJ)
	$(CXX_nat) $(CFLAGS_nat) -o evoke $(OBJ)

evoke.js: $(SRC)
	$(CXX_web) $(CFLAGS_web) -o evoke.js $(SRC)

GT_SRC	:= tests/grid_test.cc
grid_test: $(GT_SRC)
	$(CXX_web) $(CFLAGS_web) -o tests/grid_test.js $(GT_SRC)

clean:
	rm -f evoke evoke.js *~ $(OBJ)
