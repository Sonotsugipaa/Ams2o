#include <amscript/amscript2.hpp>
#include <amscript/except.hpp>

#include "ams2o/util.tpp"

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>  // only used for strcmp in "print_base"
#include <vector>



namespace ams2o {

	std::string file_to_str(std::istream& in) {
		std::string r;
		char buffer[STREAM_BUFFER_SIZE];
		size_t readn;
		do {
			while(in && (0 < (readn = in.readsome(buffer, STREAM_BUFFER_SIZE-1)))) {
				buffer[readn] = '\0';
				r += buffer;
			}
			readn = in.read(&buffer[0], 1)? 1 : 0;
			if(readn > 0) {
				buffer[readn] = '\0';
				r += buffer;
			} else {
				/* this break directive is technically superfluous,
				 * but it skips a conditional jump */
				break;
			}
		} while(readn > 0);
		return r;
	}

	amscript2::Script from_file(const char* path) {
		std::ifstream is = std::ifstream(path);
		amscript2::Script r;
		if(is) {
			std::string src = file_to_str(is);
			r = amscript2::Script(std::move(src));
		} else {
			std::cerr << "ERROR: file not found: " << path << std::endl;
			::exit(EXIT_FAILURE);
		}
		return r;
	}

	amscript2::Script from_stdin() {
		std::string src = file_to_str(std::cin);
		amscript2::Script r = amscript2::Script(std::move(src));
		return r;
	}

}



int main(int argn, char** args) {
	bool debug = false;
	if(argn > 1 && 0 == ::strcmp(args[1], "--debug")) {
		std::cerr << "DEBUGGING ENABLED\n";
		debug = true; --argn; ++args;
	}
	try {
		if(argn <= 1) {
			amscript2::Script full_script = ams2o::from_stdin();
			std::cout << ams2o::run(full_script, debug);
		} else {
			amscript2::Script full_script = ams2o::from_file(args[1]);
			for(size_t i=2; i < (size_t) argn; ++i) {
				full_script << ams2o::from_file(args[i]);
			}
			std::cout << ams2o::run(full_script, debug);
		}
	} catch(ParseException& ex) {
		std::cerr
			<< "An error occurred while interpreting the script.\n"
			<< ex.value << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
