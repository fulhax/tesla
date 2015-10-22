BASE_DIR=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
BUILD_DIR=$(BASE_DIR)/build

UNAME=$(shell uname)
ifeq ($(UNAME), windows32)
	MKDIR=mkdir
	BUILD_TARGET="MinGW Makefiles"
else ifeq ($(UNAME), windows64)
	MKDIR=mkdir
	BUILD_TARGET="MinGW Makefiles"
else
	MKDIR=mkdir -p
	BUILD_TARGET="Unix Makefiles"
endif

BUILD_TYPE=debug
BUILD_INFO=Debug
ifdef RELEASE_DBG_INFO
	BUILD_TYPE=release_dbg_info
	BUILD_INFO=RelWithDebInfo
	export RELEASE_DBG_INFO=1
endif
ifdef RELEASE
	BUILD_TYPE=release
	BUILD_INFO=Release
	export RELEASE=1
endif
PLATFORM_LIST=default
ifdef PLATFORM
	PLATFORM_LIST=$(PLATFORM)
endif

#CMAKE_WIN32_TOOLCHAIN=${BASE_DIR}/toolchain-mingw32.cmake

.PHONY: all build distclean clean
.SILENT:

all: package

build: $(BUILD_DIR)/$(UNAME)/$(BUILD_TYPE)/Makefile
	@make -C $(BUILD_DIR)/$(UNAME)/$(BUILD_TYPE)

package: build
	mkdir -p bin
	cp -v ${BUILD_DIR}/$(UNAME)/$(BUILD_TYPE)/game bin/

${BUILD_DIR}/$(UNAME)/$(BUILD_TYPE)/Makefile: src/CMakeLists.txt
	-@$(MKDIR) "$(BUILD_DIR)/$(UNAME)/$(BUILD_TYPE)"
	@cd "$(BUILD_DIR)/$(UNAME)/$(BUILD_TYPE)" && \
		cmake -G $(BUILD_TARGET) \
		-DCMAKE_BUILD_TYPE=$(BUILD_INFO) \
		"$(BASE_DIR)/src" \
		 -DCMAKE_EXPORT_COMPILE_COMMANDS=1

clean:
	@make -C "$(BUILD_DIR)/$(UNAME)/$(BUILD_TYPE)" clean

distclean:
	@rm -rf $(BUILD_DIR)

