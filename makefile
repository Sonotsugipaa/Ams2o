DEBUG=-g
CPPFLAGS=-std=c++17 $(DEBUG) -O3 -Wall -Wpedantic -I./include -L./lib
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
	make bin/
	g++ $(CPPFLAGS) -o"$@" $^

# compiles a C++ source file from ./src
build/%.o: src/%.cpp
	# ----- C++ object ----- #
	make build/
	g++ $(CPPFLAGS) $< -c -o"$@"

lib/lib%.a: # insert your build/* targets here
	# ----- Static library ----- #
	rm -f $@
	ar -rs $@ $^

.PHONY: setup clean reset
setup: reset bin/ build/ src/main/ include/ lib/

clean:
	rm -rf build

reset:
	rm -rf bin lib build
