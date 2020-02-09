DEBUG=-DNDEBUG
CPPFLAGS=-std=c++17 $(DEBUG) -O3 -Wall -Wpedantic -I./include -L./lib
INCLUDE_AMS2=-I./Amscript2/include
LINK_AMS2=-L./Amscript2/lib
CPP_SRCS=$(wildcard src/*.cpp)
ALL_OBJS=$(patsubst src/%.cpp, build/%.o, $(CPP_SRCS))


# objects should not be removed automatically
.PRECIOUS: build/%.o build install


build: bin/ams

install: /usr/local/bin/ams /usr/local/lib/libamscript2.so


# create a generic folder through a target dependency
%/:
	mkdir -p "$@"

# links all C++ source files from ./src/main
bin/%: src/main/%.cpp
	# ----- C++ executable ----- #
	$(MAKE) --no-print-directory bin/
	g++ $(CPPFLAGS) $(INCLUDE_AMS2) $(LINK_AMS2) -o"$@" $^
	strip "$@"

# compiles a C++ source file from ./src
build/%.o: src/%.cpp
	# ----- C++ object ----- #
	$(MAKE) --no-print-directory build/
	g++ $(CPPFLAGS) $(INCLUDE_AMS2) $< -c -o"$@"

.PHONY: /usr/local/lib/libamscript2.so
/usr/local/lib/libamscript2.so: Amscript2/lib/libamscript2.so
	# ----- Install Amscript2 shared library ----- #
	sudo cp -f "$<" "$@"
	sudo ldconfig

PHONY: /usr/local/bin/ams
/usr/local/bin/ams: bin/ams /usr/local/lib/libamscript2.so
	# ----- Install "ams" shell command ----- #
	sudo cp -f "$<" "$@"

Amscript2/%:
	git submodule update --init Amscript2
	$(MAKE) --directory=Amscript2 $(patsubst "Amscript2/%",%,"$@")

bin/ams: src/main/ams.cpp build/ams2o.o build/ext.o Amscript2/lib/libamscript2.so
	$(MAKE) --no-print-directory bin/
<<<<<<< HEAD
	g++ $(CPPFLAGS) $(INCLUDE_AMS2) $(LINK_AMS2) -o"$@" \
		build/ams2o.o build/ext.o $< -lamscript2
	strip "$@"
=======
	g++ $(CPPFLAGS) $(INCLUDE_AMS2) $(LINK_AMS2) -o"$@" \
		build/ams2o.o build/ext.o $< -lamscript2
	strip "$@"
>>>>>>> master


.PHONY: setup clean reset
setup: reset bin/ build/ src/main/ include/ lib/

clean:
	rm -rf build

reset:
	rm -rf bin lib build
	$(MAKE) --directory="Amscript2/" reset
