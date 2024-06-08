EMCC = emcc
REMOTE_SOURCES = web/main.cpp web/webgl.c
PRESENTER_SOURCES = web/presenter.cpp

REMOTE_EMFLAGS = -s --js-library web/library.js --closure 1 -s TEXTDECODER=2    \
          -s MIN_WEBGL_VERSION=1 -s MAX_WEBGL_VERSION=2    \
		  -s ABORTING_MALLOC=0 -s ENVIRONMENT=web                               \
		  -s GL_SUPPORT_AUTOMATIC_ENABLE_EXTENSIONS=0                           \
		  -s GL_EXTENSIONS_IN_PREFIXED_FORMAT=0 -s GL_POOL_TEMP_BUFFERS=0       \
		  -s AGGRESSIVE_VARIABLE_ELIMINATION=1                                  \
		  -s EXPORTED_FUNCTIONS='["_send_presenter_msg","_get_remote_mac", "_main", '_fake_presenter_state_dark', 'UTF8ToString', "_free"]' \
		  -s EXPORTED_RUNTIME_METHODS=['ccall'] \
		  -s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=1 -s FILESYSTEM=0    \
		  -s GL_TRACK_ERRORS=1 -lGL -O3 -s WASM=1 -sMODULARIZE=0

PRESENTER_EMFLAGS = -sMODULARIZE=1 -s ENVIRONMENT=web \
    	-sEXPORTED_FUNCTIONS="['_message_string_to_json', "_message_json_to_string", "_generate_base64_little_state_hash_json"]" -s EXPORTED_RUNTIME_METHODS=['ccall'] \
		-sEXPORT_ES6=1 -sEXPORT_NAME=OpenClicker

remote: clean
	$(EMCC) $(REMOTE_EMFLAGS) $(REMOTE_SOURCES) -I src -I include -o out/remote.html
	cp out/remote.js ../../../presenter/src/wasm
	cp out/remote.wasm ../../../presenter/src/wasm

presenter: clean
	$(EMCC) $(PRESENTER_EMFLAGS) $(PRESENTER_SOURCES) -I src -I include -I web -o out/presenter.js
	cp out/presenter.js ../../../presenter/src/wasm
	cp out/presenter.wasm ../../../presenter/src/wasm

build: remote presenter
	@echo "Build Complete"

clean:
	rm -rf out
	mkdir out
