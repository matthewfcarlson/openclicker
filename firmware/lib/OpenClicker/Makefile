EMCC = emcc
SOURCES = web/main.cpp web/webgl.c

EMFLAGS = -s --js-library web/library.js --closure 1 -s TEXTDECODER=2    \
          -s MINIMAL_RUNTIME=2 -s MIN_WEBGL_VERSION=1 -s MAX_WEBGL_VERSION=2    \
		  -s ABORTING_MALLOC=0 -s ENVIRONMENT=web                               \
		  -s GL_SUPPORT_AUTOMATIC_ENABLE_EXTENSIONS=0                           \
		  -s GL_EXTENSIONS_IN_PREFIXED_FORMAT=0 -s GL_POOL_TEMP_BUFFERS=0       \
		  -s AGGRESSIVE_VARIABLE_ELIMINATION=1                                  \
		  -s EXPORTED_FUNCTIONS='["_send_presenter_msg","_get_remote_mac", "_main"]' \
		  -s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=1 -s FILESYSTEM=0    \
		  -s GL_TRACK_ERRORS=1 -lGL -O0

build: clean
	mkdir out
	cp web/index.html out/index.html
	$(EMCC) $(EMFLAGS) $(SOURCES) -I src -I include -o out/remote.html
	cp out/remote.wasm ../../../presenter/src/wasm
	cp out/remote.js ../../../presenter/src/wasm

clean:
	rm -rf out