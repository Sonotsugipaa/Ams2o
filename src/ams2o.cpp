#include "ams2o/util.tpp"

#include "amscript/amscript2.hpp"

#include <iostream>



namespace ams2o {

	const amscript2::Namespace AMS2O_NS = amscript2::Namespace("ams2o");


	struct Options {
		unsigned int byte_order = BIG_ENDIAN;
		unsigned int word_size = AMS2O_SIZE_8;
		bool text_to_bin = false;

		std::string separator = std::string();

		bool show_warnings = true;

		void warn(std::string message, std::ostream& warn_output = std::cerr) const {
			if(show_warnings)  std::cerr << "WARNING - " << message << std::endl;
		}
	};

	std::string concat(const Options& opt, const std::vector<amscript2::Value>& values) {
		std::string r;  r.reserve(heuristic_length_text(values.size()));
		if(! values.empty())
			r += values[0].strValue();
		for(size_t i=1; i < values.size(); ++i)
			r += opt.separator + values[i].strValue();
		return r;
	}

	template<unsigned bits>
	const std::string word_limit_str = std::to_string(word_limit<bits>);

	template<unsigned bits>
	std::string int_to_binary_str_se(uint_t x) {
		std::string r;  r.resize(word_length<bits>);
		for(uint_t i=0; i < word_length<bits>; ++i)
			r[i] = (x >> ((word_length<bits> - (i+1)) * 8)) & 0xFF;
			//r[i] = (x << (i*8)) & 0xFF;
		return r;
	}

	template<unsigned bits>
	std::string int_to_binary_str_be(uint_t x) {
		std::string r;  r.resize(word_length<bits>);
		for(uint_t i=0; i < word_length<bits>; ++i)
			//r[i] = (x << ((word_length<bits> - (i+1)) * 8)) & 0xFF;
			r[i] = (x >> (i*8)) & 0xFF;
		return r;
	}

	template<unsigned byte_order, unsigned bits, bool warn_neg>
	std::string text_to_binary(const Options& opt, const amscript2::Value& value) {
		static_assert(
			(byte_order == AMS2O_SMALL_ENDIAN) || (byte_order == AMS2O_BIG_ENDIAN),
			"template parameter 'byte_order' must be AMS2O_SMALL_ENDIAN or AMS2O_BIG_ENDIAN");
		std::string r;
		if(value.type() != amscript2::Type::INTEGER) {
			opt.warn("Invalid integer value: " + value.strValue());
		} else {
			uint_t ivalue = value.intValue();
			if constexpr(warn_neg) {
				if(value.intValue() < 0) {
					opt.warn("Binary word should be signed: " + value.strValue());
				}
				else
				if(ivalue > word_limit<bits>) {
					opt.warn(
						  "Integer overflow: "
						+ value.strValue() + " > "
						+ word_limit_str<bits>);
				}
			}
			if constexpr(byte_order == AMS2O_SMALL_ENDIAN)
				r = int_to_binary_str_se<bits>(ivalue);
			else
				r = int_to_binary_str_be<bits>(ivalue);
		}
		return r;
	}

	template<unsigned byte_order, unsigned bits, bool warn_neg>
	std::string text_to_binary(
			const Options& opt,
			const std::vector<amscript2::Value>& values
	) {
		std::string r;  r.reserve(heuristic_length_binary(values.size() * opt.word_size));
		if(! values.empty())
			r += text_to_binary<byte_order, bits, warn_neg>(opt, values[0]);
		for(size_t i=1; i < values.size(); ++i)
			r += opt.separator + text_to_binary<byte_order, bits, warn_neg>(opt, values[i]);
		return r;
	}

	std::string binary(const Options& opt, const std::vector<amscript2::Value>& values) {
		std::string r;
		constexpr auto SE = AMS2O_SMALL_ENDIAN;
		constexpr auto BE = AMS2O_BIG_ENDIAN;
		/* too much boilerplate code for me to even care about
		 * ctrl+V rules anymore */
		switch(opt.byte_order) {
			case AMS2O_SMALL_ENDIAN:
				switch(opt.word_size) {
					case AMS2O_SIZE_8:   r = text_to_binary<SE,  8,  true>(opt, values);  break;
					case AMS2O_SIZE_16:  r = text_to_binary<SE, 16,  true>(opt, values);  break;
					case AMS2O_SIZE_32:  r = text_to_binary<SE, 32,  true>(opt, values);  break;
					case AMS2O_SIZE_64:  r = text_to_binary<SE, 64,  true>(opt, values);  break;
					case AMS2O_SIZE_8U:  r = text_to_binary<SE,  8, false>(opt, values);  break;
					case AMS2O_SIZE_16U: r = text_to_binary<SE, 16, false>(opt, values);  break;
					case AMS2O_SIZE_32U: r = text_to_binary<SE, 32, false>(opt, values);  break;
					case AMS2O_SIZE_64U: r = text_to_binary<SE, 64, false>(opt, values);  break;
					default:
						opt.warn("Invalid word_size option: " + std::to_string(opt.word_size));
					break;
				}
			break;
			case AMS2O_BIG_ENDIAN:
				switch(opt.word_size) {
					case AMS2O_SIZE_8:   r = text_to_binary<BE,  8,  true>(opt, values);  break;
					case AMS2O_SIZE_16:  r = text_to_binary<BE, 16,  true>(opt, values);  break;
					case AMS2O_SIZE_32:  r = text_to_binary<BE, 32,  true>(opt, values);  break;
					case AMS2O_SIZE_64:  r = text_to_binary<BE, 64,  true>(opt, values);  break;
					case AMS2O_SIZE_8U:  r = text_to_binary<BE,  8, false>(opt, values);  break;
					case AMS2O_SIZE_16U: r = text_to_binary<BE, 16, false>(opt, values);  break;
					case AMS2O_SIZE_32U: r = text_to_binary<BE, 32, false>(opt, values);  break;
					case AMS2O_SIZE_64U: r = text_to_binary<BE, 64, false>(opt, values);  break;
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
				     if(arg == AMS2O_SIZE_8)   opt.word_size = AMS2O_SIZE_8;
				else if(arg == AMS2O_SIZE_16)  opt.word_size = AMS2O_SIZE_16;
				else if(arg == AMS2O_SIZE_32)  opt.word_size = AMS2O_SIZE_32;
				else if(arg == AMS2O_SIZE_64)  opt.word_size = AMS2O_SIZE_64;
				else if(arg == AMS2O_SIZE_8U)  opt.word_size = AMS2O_SIZE_8U;
				else if(arg == AMS2O_SIZE_16U) opt.word_size = AMS2O_SIZE_16U;
				else if(arg == AMS2O_SIZE_32U) opt.word_size = AMS2O_SIZE_32U;
				else if(arg == AMS2O_SIZE_64U) opt.word_size = AMS2O_SIZE_64U;
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

	std::string run(amscript2::Script script) {
		ams2o::Options options;

		script.import(ext::extension);

		std::vector<amscript2::Value> result = script.run();
		parse_options(options, script);

		if(options.text_to_bin)
			return binary(options, result);
		else
			return concat(options, result) + '\n';
	}

}
