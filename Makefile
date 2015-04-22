CSL_BASE = gsm/csl
CSL_BUILD = $(CSL_BASE)/build
CSL_CMAKE = $(CSL_BASE)/CMakeLists.txt
CSL_LIB = $(CSL_BASE)/build/lib/libcsl.so
JAVA_HOME ?= /usr/lib/jvm/java-7-openjdk

all: $(CSL_LIB)

$(CSL_CMAKE):
	git submodule update --init --recursive $(CSL_BASE)

$(CSL_LIB): $(CSL_CMAKE)
	mkdir -p $(CSL_BUILD) &&                                     \
	cd $(CSL_BUILD) &&                                           \
	JAVA_HOME=$(JAVA_HOME) cmake -DCMAKE_BUILD_TYPE=relase .. && \
	$(MAKE)
