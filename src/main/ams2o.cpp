#include <amscript/amscript2.hpp>
#include <amscript/except.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>



namespace {

	using amscript2::int_t;
	using uint_t = unsigned long long int;

	constexpr unsigned int AMS2O_BIG_ENDIAN   = 1;
	constexpr unsigned int AMS2O_SMALL_ENDIAN = 0x80000000;

	constexpr unsigned int AMS2O_SIZE_8       = 8;
	constexpr unsigned int AMS2O_SIZE_16      = 16;
	constexpr unsigned int AMS2O_SIZE_32      = 32;
	constexpr unsigned int AMS2O_SIZE_64      = 64;

	constexpr unsigned int AMS2O_TRUE         = 1;
	constexpr unsigned int AMS2O_FALSE        = 0;

	constexpr unsigned int AMS2O_BINARY       = 0;
	constexpr unsigned int AMS2O_TEXT         = 1;

	const amscript2::Namespace AMS2O_NS = amscript2::Namespace("ams2o");


	struct Options {
		unsigned int byte_order = BIG_ENDIAN;
		unsigned int word_size = AMS2O_SIZE_8;
		bool text_to_bin = false;

		std::string separator = std::string();

		bool show_warnings = true;

		void warn(std::string message) const {
			if(show_warnings)  std::cout << "WARNING - " << message << std::endl;
		}
	};

	constexpr size_t heuristic_length_text(size_t n_of_values) {
		return 7 * n_of_values;
	}

	constexpr size_t heuristic_length_binary(size_t n_of_values) {
		return 3 * n_of_values;
	}

	std::string concat(const Options& opt, const std::vector<amscript2::Value>& values) {
		std::string r;  r.reserve(heuristic_length_text(values.size()));
		if(! values.empty())
			r += values[0].strValue();
		for(size_t i=1; i < values.size(); ++i)
			r += opt.separator + values[i].strValue();
		return r;
	}

	/* The assumption that a byte (char) contains at least 8 bits is
	 * a bit iffy (no pun intended), but I see no solution around
	 * this dilemma. */
	template<unsigned bits>
	constexpr uint_t word_length = bits / 8;

	template<unsigned bits>
	constexpr uint_t word_limit =
		[](uint_t pow) {
			uint_t r = 1;
			uint_t overflow_guard = r;
			while((--pow)>0) {
				overflow_guard = r;
				r*=2;
				if(r < overflow_guard)
					return overflow_guard;
			}
			return r;
		} (bits);

	template<unsigned bits>
	const std::string word_limit_str = std::to_string(word_limit<bits>);

	template<unsigned bits>
	std::string int_to_binary_str_se(uint_t x) {
		std::string r;  r.resize(word_length<bits>);
		for(uint_t i=0; i < word_length<bits>; ++i)
			r[i] = (x << (i*8)) & 0xFF;
		return r;
	}

	template<unsigned bits>
	std::string int_to_binary_str_be(uint_t x) {
		std::string r;  r.resize(word_length<bits>);
		for(uint_t i=0; i < word_length<bits>; ++i)
			r[i] = (x << ((word_length<bits> - (i+1)) * 8)) & 0xFF;
		return r;
	}

	template<unsigned byte_order, unsigned bits>
	std::string text_to_binary(const Options& opt, const amscript2::Value& value) {
		static_assert(
			(byte_order == AMS2O_SMALL_ENDIAN) || (byte_order == AMS2O_BIG_ENDIAN),
			"template parameter 'byte_order' must be AMS2O_SMALL_ENDIAN or AMS2O_BIG_ENDIAN");
		std::string r;
		if(value.type() != amscript2::Type::INTEGER) {
			opt.warn("Invalid integer value: " + value.strValue());
		} else {
			uint_t ivalue = value.intValue();
			if(ivalue < 0) {
				opt.warn("Binary word should be signed: " + value.strValue());
			}
			else
			if(ivalue > word_limit<bits>) {
				opt.warn(
					  "Integer overflow: "
					+ value.strValue() + " > "
					+ word_limit_str<bits>);
			}
			if constexpr(byte_order == AMS2O_SMALL_ENDIAN)
				r = int_to_binary_str_se<bits>(ivalue);
			else
				r = int_to_binary_str_be<bits>(ivalue);
		}
		return r;
	}

	template<unsigned byte_order, unsigned bits>
	std::string text_to_binary(
			const Options& opt,
			const std::vector<amscript2::Value>& values
	) {
		std::string r;  r.reserve(heuristic_length_binary(values.size() * opt.word_size));
		if(! values.empty())
			r += text_to_binary<byte_order, bits>(opt, values[0]);
		for(size_t i=1; i < values.size(); ++i)
			r += opt.separator + text_to_binary<byte_order, bits>(opt, values[i]);
		return r;
	}

	std::string binary(const Options& opt, const std::vector<amscript2::Value>& values) {
		std::string r;
		/* too much boilerplate code for me to even care about
		 * ctrl+V rules anymore */
		switch(opt.byte_order) {
			case AMS2O_SMALL_ENDIAN:
				switch(opt.word_size) {
					case AMS2O_SIZE_8:
						r = text_to_binary<AMS2O_SMALL_ENDIAN, 8>(opt, values);
					break;
					case AMS2O_SIZE_16:
						r = text_to_binary<AMS2O_SMALL_ENDIAN, 16>(opt, values);
					break;
					case AMS2O_SIZE_32:
						r = text_to_binary<AMS2O_SMALL_ENDIAN, 32>(opt, values);
					break;
					case AMS2O_SIZE_64:
						r = text_to_binary<AMS2O_SMALL_ENDIAN, 64>(opt, values);
					break;
					default:
						opt.warn("Invalid word_size option: " + std::to_string(opt.word_size));
					break;
				}
			break;
			case AMS2O_BIG_ENDIAN:
				switch(opt.word_size) {
					case AMS2O_SIZE_8:
						r = text_to_binary<AMS2O_BIG_ENDIAN, 8>(opt, values);
					break;
					case AMS2O_SIZE_16:
						r = text_to_binary<AMS2O_BIG_ENDIAN, 16>(opt, values);
					break;
					case AMS2O_SIZE_32:
						r = text_to_binary<AMS2O_BIG_ENDIAN, 32>(opt, values);
					break;
					case AMS2O_SIZE_64:
						r = text_to_binary<AMS2O_BIG_ENDIAN, 64>(opt, values);
					break;
					default:
						opt.warn("Invalid word_size option: " + std::to_string(opt.word_size));
					break;
				}
			break;
			default:
				opt.warn("Invalid byte_order option: " + std::to_string(opt.byte_order));
			break;
		}
		return r;
	}

	void parse_opt_byte_order(Options& opt, amscript2::Script& scr) {
		using namespace amscript2;
		std::vector<Value> values = scr.invoke(ref_t(AMS2O_NS, "byte_order"), {});
		if(! values.empty()) {
			if(values[0].type() == amscript2::Type::INTEGER) {
				int_t arg = values[0].intValue();
				if(arg == AMS2O_BIG_ENDIAN)  opt.byte_order = AMS2O_BIG_ENDIAN;
				else
				if(arg == AMS2O_SMALL_ENDIAN)  opt.byte_order = AMS2O_SMALL_ENDIAN;
				else
				opt.warn("Invalid byte order \""+values[0].strValue()+"\"");
			}
		}
	}

	void parse_opt_word_size(Options& opt, amscript2::Script& scr) {
		using namespace amscript2;
		std::vector<Value> values = scr.invoke(ref_t(AMS2O_NS, "word_size"), {});
		if(! values.empty()) {
			if(values[0].type() == amscript2::Type::INTEGER) {
				int_t arg = values[0].intValue();
				if(arg == AMS2O_SIZE_8)  opt.word_size = AMS2O_SIZE_8;
				else
				if(arg == AMS2O_SIZE_16)  opt.word_size = AMS2O_SIZE_16;
				else
				if(arg == AMS2O_SIZE_32)  opt.word_size = AMS2O_SIZE_32;
				else
				if(arg == AMS2O_SIZE_64)  opt.word_size = AMS2O_SIZE_64;
				else
				opt.warn("Invalid word size \""+values[0].strValue()+"\"");
			}
		}
	}

	void parse_opt_mode(Options& opt, amscript2::Script& scr) {
		using namespace amscript2;
		std::vector<Value> values = scr.invoke(ref_t(AMS2O_NS, "mode"), {});
		if(! values.empty()) {
			if(values[0].type() == amscript2::Type::INTEGER) {
				int_t arg = values[0].intValue();
				if(arg == AMS2O_BINARY)  opt.text_to_bin = true;
				else
				if(arg == AMS2O_TEXT)  opt.text_to_bin = false;
				else
				opt.warn("invalid output mode \""+values[0].strValue()+"\"");
			}
		}
	}

	void parse_opt_warnings(Options& opt, amscript2::Script& scr) {
		using namespace amscript2;
		std::vector<Value> values = scr.invoke(ref_t(AMS2O_NS, "show_warnings"), {});
		if(! values.empty()) {
			if(values[0].type() == amscript2::Type::INTEGER) {
				int_t arg = values[0].intValue();
				if(arg == AMS2O_TRUE)  opt.show_warnings = AMS2O_TRUE;
				else
				if(arg == AMS2O_FALSE)  opt.show_warnings = AMS2O_FALSE;
				else
				opt.warn("invalid show_warnings value \""+values[0].strValue()+"\"");
			}
		}
	}

	void parse_separator(Options& opt, amscript2::Script& scr) {
		using namespace amscript2;
		std::vector<Value> values = scr.invoke(ref_t(AMS2O_NS, "separator"), {});
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
		parse_opt_warnings(opt, scr);
		parse_separator(opt, scr);
	}

	std::string build_def_src_ln(std::string ref, int_t val) {
		return AMS2O_NS.strValue() + ref + ": " + std::to_string(val) + '\n';
	}

	const std::string script_extension_src =
		  build_def_src_ln("big_endian", AMS2O_BIG_ENDIAN)
		+ build_def_src_ln("small_endian", AMS2O_SMALL_ENDIAN)
		+ build_def_src_ln("int8", AMS2O_SIZE_8)
		+ build_def_src_ln("int16", AMS2O_SIZE_16)
		+ build_def_src_ln("int32", AMS2O_SIZE_32)
		+ build_def_src_ln("int64", AMS2O_SIZE_64)
		+ build_def_src_ln("true", AMS2O_TRUE)
		+ build_def_src_ln("false", AMS2O_FALSE)
		+ build_def_src_ln("binary", AMS2O_BINARY)
		+ build_def_src_ln("text", AMS2O_TEXT);

	constexpr size_t STREAM_BUFFER_SIZE = 1024;

	std::string file_to_str(std::istream& in) {
		std::string r;
		char buffer[STREAM_BUFFER_SIZE];
		size_t readn;
		while(in && (0 < (readn = in.readsome(buffer, STREAM_BUFFER_SIZE-1)))) {
			buffer[readn] = '\0';
			r += buffer;
		}
		return r;
	}

	amscript2::Script from_file(const char* path) {
		std::ifstream is = std::ifstream(path);
		amscript2::Script r;
		if(is) {
			std::string src = script_extension_src + file_to_str(is);
			r = amscript2::Script(std::move(src));
		} else {
			std::cerr << "ERROR: file not found: " << path << std::endl;
			::exit(EXIT_FAILURE);
		}
		return r;
	}

	amscript2::Script from_stdin() {
		amscript2::Script r;
		std::string src = script_extension_src + file_to_str(std::cin);
		r = amscript2::Script(std::move(src));
		return r;
	}

}



namespace ext {

	using amscript2::Value;
	using amscript2::Type;
	using amscript2::ref_t;


	std::vector<Value> ascii(std::vector<Value> values, amscript2::ScopeView&) {
		std::string str;
		std::vector<Value> r;
		for(const auto& v : values) {
			switch(v.type()) {
				case Type::STRING:
					for(char c : v.strValue())
						r.push_back(Value(Type::INTEGER, std::to_string((unsigned) c)));
				break;
				case Type::INTEGER:
				case Type::FRACTIONAL:
					str = static_cast<char>(v.intValue() & 0xFF);
					r.push_back(Value(Type::STRING, str));
				break;
				default:  break;
			}
		}
		return r;
	}

	std::vector<Value> concat(std::vector<Value> values, amscript2::ScopeView&) {
		std::string r;  r.reserve(values.size() * 4);
		if(! values.empty())
			r += values[0].strValue();
		for(size_t i=1; i < values.size(); ++i)
			r += values[i].strValue();
		return {r};
	}

	const amscript2::Script extension = []() {
		amscript2::Script ext;
		ext.define(ref_t(AMS2O_NS, "ascii"), ascii);
		ext.define(ref_t(AMS2O_NS, "concat"), concat);
		return ext;
	}();

}




int main(int argn, char** args) {
	Options options;

	amscript2::Script script;

	try {
		if(argn > 1)
			script = amscript2::Script(from_file(args[1]));
		else
			script = amscript2::Script(from_stdin());

		script.import(ext::extension);

		std::vector<amscript2::Value> result = script.run();
		parse_options(options, script);

		if(options.text_to_bin)
			std::cout << binary(options, result);
		else
			std::cout << concat(options, result) << std::endl;
	} catch(ParseException& ex) {
		std::cerr
			<< "An error occurred while interpreting the script.\n"
			<< ex.value << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
