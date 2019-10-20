DEBUG=-g
CPPFLAGS=-std=c++17 $(DEBUG) -O3 -Wall -Wpedantic -I./include -L./lib
INCLUDE_AMS2=-I./Amscript2/include
CPP_SRCS=$(wildcard src/*.cpp)
ALL_OBJS=$(patsubst src/%.cpp, build/%.o, $(CPP_SRCS))


# objects should not be removed automatically
.PRECIOUS: build/%.o

# create a generic folder through a target dependency
%/:
	mkdir -p "$@"

# links all C++ source files from ./src/main
bin/%: src/main/%.cpp
	# ----- C++ executable ----- #
	make --no-print-directory bin/
	g++ $(CPPFLAGS) -o"$@" $^

# compiles a C++ source file from ./src
build/%.o: src/%.cpp
	# ----- C++ object ----- #
	make --no-print-directory build/
	g++ $(CPPFLAGS) $< -c -o"$@"


lib/libamscript2.a:
	# ----- External static library ----- #
	make --no-print-directory lib/
	make --directory="Amscript2/" lib/libamscript2.a
	mv Amscript2/lib/libamscript2.a lib/libamscript2.a

bin/ams2o: src/main/ams2o.cpp lib/libamscript2.a
	make --no-print-directory bin/
	g++ $(CPPFLAGS) $(INCLUDE_AMS2) -o"$@" $< -lamscript2


.PHONY: setup clean reset
setup: reset bin/ build/ src/main/ include/ lib/

clean:
	rm -rf build

reset:
	rm -rf bin lib build
	make --directory="Amscript2/" reset
