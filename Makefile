CSL_BASE = $(PWD)/gsm/csl
CSL_BUILD = $(CSL_BASE)/build
CSL_CMAKE = $(CSL_BASE)/CMakeLists.txt
CSL_LIB = $(CSL_BUILD)/lib/libcsl.so
CSL_BIN = $(CSL_BUILD)/bin
JAVA_HOME ?= /usr/lib/jvm/java-7-openjdk

all: build/bin/profiler

build/bin/profiler: $(CSL_LIB)
	mkdir -p build &&                      \
	cd build &&                            \
	JAVA_HOME=$(JAVA_HOME)/inlcude cmake   \
	        -DCMAKE_BUILD_TYPE=release     \
		-DCSL_INCLUDE_DIR=$(CSL_BASE)  \
		-DCSL_LIBRARY=$(CSL_LIB) .. && \
	$(MAKE) VERBOSE=1

$(CSL_CMAKE):
	git submodule update --init --recursive $(CSL_BASE)

$(CSL_LIB): $(CSL_CMAKE)
	mkdir -p $(CSL_BUILD) &&                                     \
	cd $(CSL_BUILD) &&                                           \
	JAVA_HOME=$(JAVA_HOME) cmake -DCMAKE_BUILD_TYPE=relase .. && \
	$(MAKE)
