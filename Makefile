DESTDIR ?=
PREFIX ?= /usr/local/

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
	install -d $(DESTDIR)$(PREFIX)/bin
	install $^ $(DESTDIR)$(PREFIX)/bin
