DESTDIR ?=
PREFIX ?= /usr/local/
BINDIR ?= $(PREFIX)/bin

PROFILER=build/bin/profiler
COMPILE=build/bin/compileFBDic
TRAIN=build/bin/trainFrequencyList

all: $(PROFILER) $(COMPILE) $(TRAIN)

$(TRAIN) $(PROFILER) $(COMPILE): bin/Makefile
	make -C build

bin/Makefile:
	mkdir -p build && cd build && cmake ..

.PHONY: clean
clean:
	$(RM) -r build

.PHONY: install
install: $(PROFILER) $(COMPILE) $(TRAIN)
	install -d $(DESTDIR)$(BINDIR)
	install $^ $(DESTDIR)$(BINDIR)
