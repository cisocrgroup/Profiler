CSL_BASE = $(PWD)/gsm/csl
CSL_BUILD = $(CSL_BASE)/build
CSL_CMAKE = $(CSL_BASE)/CMakeLists.txt
CSL_LIB = $(CSL_BUILD)/lib/libcsl.so
COMPILE_FBDIC = $(CSL_BIN)/compileFBDic
CSL_BIN = $(CSL_BUILD)/bin
PROFILER = build/bin/profiler
JAVA_HOME ?= /usr/lib/jvm/java-7-openjdk
DESTDIR ?=
PREFIX ?= /usr/bin

all: $(PROFILER)

$(PROFILER): $(CSL_LIB)
	mkdir -p build &&                      \
	cd build &&                            \
	JAVA_HOME=$(JAVA_HOME) cmake           \
	        -DCMAKE_BUILD_TYPE=release     \
		-DCSL_INCLUDE_DIR=$(CSL_BASE)  \
		-DCSL_LIBRARY=$(CSL_LIB) .. && \
	$(MAKE)

$(CSL_CMAKE):
	git submodule update --init --recursive $(CSL_BASE)

$(CSL_LIB) $(COMPILE_FBDIC): $(CSL_CMAKE)
	mkdir -p $(CSL_BUILD) &&                                     \
	cd $(CSL_BUILD) &&                                           \
	JAVA_HOME=$(JAVA_HOME) cmake -DCMAKE_BUILD_TYPE=relase .. && \
	$(MAKE)

.PHONY: clean
clean:
	$(RM) -r build
.PHONY: install
install: $(PROFILER) $(COMPILE_FBDIC)
	install -d $(DESTDIR)/$(PREFIX)
	install $^ $(DESTDIR)/$(PREFIX)
