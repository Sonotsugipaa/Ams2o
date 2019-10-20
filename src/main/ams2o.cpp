#include <amscript/amscript2.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>



namespace {

	using amscript2::int_t;

	static constexpr unsigned int OPT_BIG_ENDIAN   = 1;
	static constexpr unsigned int OPT_SMALL_ENDIAN = 0x80000000;

	static constexpr unsigned int OPT_SIZE_8       = 8;
	static constexpr unsigned int OPT_SIZE_16      = 16;
	static constexpr unsigned int OPT_SIZE_32      = 32;
	static constexpr unsigned int OPT_SIZE_64      = 64;

	static constexpr unsigned int OPT_TRUE         = 1;
	static constexpr unsigned int OPT_FALSE        = 0;

	static constexpr unsigned int OPT_BINARY       = 0;
	static constexpr unsigned int OPT_TEXT         = 1;


	struct Options {
		unsigned int byte_order = BIG_ENDIAN;
		unsigned int word_size = OPT_SIZE_8;
		bool text_to_bin = 0;

		std::string separator = std::string();

		bool show_warnings = true;
	};


	void warn(const Options& opt, std::string message) {
		if(opt.show_warnings)  std::cout << "WARNING: " << message << std::endl;
	}

	size_t heuristic_length(const std::vector<amscript2::Value>& values) {
		return 7 * values.size();
	}

	std::string concat(const Options& opt, const std::vector<amscript2::Value>& values) {
		std::string r;  r.reserve(heuristic_length(values));
		if(! values.empty())
			r += values[0].strValue();
		for(size_t i=1; i < values.size(); ++i)
			r += opt.separator + values[i].strValue();
		return r;
	}

	void parse_opt_byte_order(Options& opt, amscript2::Script& scr) {
		using namespace amscript2;
		std::vector<Value> values = scr.invoke(ref_t("ams2o::byte_order"), {});
		if(! values.empty()) {
			if(values[0].type() == amscript2::Type::INTEGER) {
				int_t arg = values[0].intValue();
				if(arg == OPT_BIG_ENDIAN)  opt.byte_order = OPT_BIG_ENDIAN;
				else
				if(arg == OPT_SMALL_ENDIAN)  opt.byte_order = OPT_SMALL_ENDIAN;
				else
				warn(opt, "invalid byte order \""+values[0].strValue()+"\"");
			}
		}
	}

	void parse_opt_word_size(Options& opt, amscript2::Script& scr) {
		using namespace amscript2;
		std::vector<Value> values = scr.invoke(ref_t("ams2o::word_size"), {});
		if(! values.empty()) {
			if(values[0].type() == amscript2::Type::INTEGER) {
				int_t arg = values[0].intValue();
				if(arg == OPT_SIZE_8)  opt.word_size = OPT_SIZE_8;
				else
				if(arg == OPT_SIZE_16)  opt.word_size = OPT_SIZE_16;
				else
				if(arg == OPT_SIZE_32)  opt.word_size = OPT_SIZE_32;
				else
				if(arg == OPT_SIZE_64)  opt.word_size = OPT_SIZE_64;
				else
				warn(opt, "invalid word size \""+values[0].strValue()+"\"");
			}
		}
	}

	void parse_opt_mode(Options& opt, amscript2::Script& scr) {
		using namespace amscript2;
		std::vector<Value> values = scr.invoke(ref_t("ams2o::mode"), {});
		if(! values.empty()) {
			if(values[0].type() == amscript2::Type::INTEGER) {
				int_t arg = values[0].intValue();
				if(arg == OPT_BINARY)  opt.text_to_bin = OPT_BINARY;
				else
				if(arg == OPT_TEXT)  opt.text_to_bin = OPT_TEXT;
				else
				warn(opt, "invalid output mode \""+values[0].strValue()+"\"");
			}
		}
	}

	void parse_separator(Options& opt, amscript2::Script& scr) {
		using namespace amscript2;
		std::vector<Value> values = scr.invoke(ref_t("ams2o::separator"), {});
		if(! values.empty()) {
			if(values[0]) {
				opt.separator = values[0].strValue();
			}
		}
	}

	void parse_options(Options& opt, amscript2::Script& scr) {
		parse_opt_byte_order(opt, scr);
		parse_opt_word_size(opt, scr);
		parse_opt_mode(opt, scr);
		parse_separator(opt, scr);
	}

	amscript2::Script script_extension() {
		using namespace amscript2;
		amscript2::Script ext;
		ext.define(ref_t("ams2o::big_endian"), Value(static_cast<int_t>(OPT_BIG_ENDIAN)));
		ext.define(ref_t("ams2o::small_endian"), Value(static_cast<int_t>(OPT_SMALL_ENDIAN)));
		ext.define(ref_t("ams2o::int8"), Value(static_cast<int_t>(OPT_SIZE_8)));
		ext.define(ref_t("ams2o::int16"), Value(static_cast<int_t>(OPT_SIZE_16)));
		ext.define(ref_t("ams2o::int32"), Value(static_cast<int_t>(OPT_SIZE_32)));
		ext.define(ref_t("ams2o::int64"), Value(static_cast<int_t>(OPT_SIZE_64)));
		ext.define(ref_t("ams2o::true"), Value(static_cast<int_t>(OPT_TRUE)));
		ext.define(ref_t("ams2o::false"), Value(static_cast<int_t>(OPT_FALSE)));
		return ext;
	}

	amscript2::Script from_file(const char* path) {
		std::ifstream is = std::ifstream(path);
		amscript2::Script r;
		if(is) {
			r = amscript2::Script(is);
		} else {
			std::cerr << "ERROR: file not found: " << path << std::endl;
		}
		return r;
	}

}



int main(int argn, char** args) {
	Options options;

	amscript2::Script script = script_extension();

	if(argn > 1)
		script.import(from_file(args[1]));
	else
		script.import(std::cin);

	parse_options(options, script);

	std::vector<amscript2::Value> result = script.run();
	std::cout << concat(options, result) << std::endl;

	return EXIT_SUCCESS;
}
