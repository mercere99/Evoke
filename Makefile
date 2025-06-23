# Project-specific settings
PROJECT := evoke
EMP_DIR := ../Empirical/include/

# Flags to use regardless of compiler
CFLAGS_all := -Wall -Wextra -Wno-unused-function -std=c++20 -I$(EMP_DIR)/

# Native compiler information
CXX_nat := c++
CFLAGS_nat := -O3 -DNDEBUG $(CFLAGS_all)
CFLAGS_nat_debug := -g $(CFLAGS_all)

# Emscripten compiler information
CXX_web := emcc
EMP_METHODS = EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap', 'UTF8ToString', 'stringToUTF8', 'lengthBytesUTF8']"
EMP_FUNCTIONS = EXPORTED_FUNCTIONS="['_main', "_malloc", "_free", '_empCppCallback']"
JS_LIB = --js-library $(EMP_DIR)/emp/web/library_emp.js
OFLAGS_web_all := -s $(EMP_METHODS) -s TOTAL_MEMORY=67108864 $(JS_LIB) -s $(EMP_FUNCTIONS) -s DISABLE_EXCEPTION_CATCHING=1 -s NO_EXIT_RUNTIME=1
OFLAGS_web := -Oz -DNDEBUG
OFLAGS_web_debug := -g4 -pedantic -Wno-dollar-in-identifier-extension

CFLAGS_web := $(CFLAGS_all) $(OFLAGS_web) $(OFLAGS_web_all)
CFLAGS_web_debug := $(CFLAGS_all) $(OFLAGS_web_debug) $(OFLAGS_web_all)

default: $(PROJECT)
native: $(PROJECT)
web: $(PROJECT).js
all: $(PROJECT) $(PROJECT).js


debug:	CFLAGS_nat := $(CFLAGS_nat_debug)
debug:	$(PROJECT)

debug-web:	CFLAGS_web := $(CFLAGS_web_debug)
debug-web:	$(PROJECT).js

web-debug:	debug-web

$(PROJECT):	source/drivers/command_line.cc
	$(CXX_nat) $(CFLAGS_nat) source/drivers/command_line.cc -o $(PROJECT)
	@echo To build the web version use: make web

$(PROJECT).js: source/drivers/html.cc
	$(CXX_web) $(CFLAGS_web) source/drivers/html.cc -o web/$(PROJECT).js

.PHONY: clean test serve

serve:
	python3 -m http.server

clean:
	rm -f $(PROJECT) web/$(PROJECT).js web/*.js.mem web/*.js.map web/*.wasm web/*.wasm.map *~ source/*.o

test: debug debug-web
	./evoke | grep -q 'Hello, world!' && echo 'matched!' || exit 1
	npm install
	echo "const puppeteer = require('puppeteer'); var express = require('express'); var app = express(); app.use(express.static('web')); app.listen(3000); express.static.mime.types['wasm'] = 'application/wasm'; function sleep(millis) { return new Promise(resolve => setTimeout(resolve, millis)); } async function run() { const browser = await puppeteer.launch(); const page = await browser.newPage(); await page.goto('http://localhost:3000/evoke.html'); await sleep(1000); const html = await page.content(); console.log(html); browser.close(); process.exit(0); } run();" | node | tr -d '\n' | grep -q "Each circle" && echo "matched!" || exit 1

# Debugging information
print-%: ; @echo '$(subst ','\'',$*=$($*))'
