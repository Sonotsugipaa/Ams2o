#ifndef AMS2O_AMS2O_HPP
#define AMS2O_AMS2O_HPP

#include <amscript/amscript2.hpp>

#include <cstdio>
#include <string>
#include <vector>



namespace ams2o {

	class Options;


	using amscript2::int_t;
	using uint_t = unsigned long long int;

	constexpr int_t AMS2O_BIG_ENDIAN   = 1;
	constexpr int_t AMS2O_SMALL_ENDIAN = 0x80000000;

	constexpr int_t AMS2O_SIZE_8       = 0x08;
	constexpr int_t AMS2O_SIZE_16      = 0x10;
	constexpr int_t AMS2O_SIZE_32      = 0x20;
	constexpr int_t AMS2O_SIZE_64      = 0x40;
	constexpr int_t AMS2O_SIZE_8U      = 0x0800;
	constexpr int_t AMS2O_SIZE_16U     = 0x1000;
	constexpr int_t AMS2O_SIZE_32U     = 0x2000;
	constexpr int_t AMS2O_SIZE_64U     = 0x4000;

	constexpr int_t AMS2O_TRUE         = 1;
	constexpr int_t AMS2O_FALSE        = 0;

	constexpr int_t AMS2O_BINARY       = 0;
	constexpr int_t AMS2O_TEXT         = 1;

	constexpr size_t STREAM_BUFFER_SIZE = 1024;


	extern const amscript2::Namespace AMS2O_NS;


	std::string run(amscript2::Script script, bool debug);


	namespace ext {

		extern const amscript2::Script extension;

		/* Translates numbers to their matching ASCII values (e.g. 65 => 'A'),
		 * and ASCII strings to sequences of integer numbers (e.g. "AB" => {65 66} ). */
		std::vector<amscript2::Value> ascii(std::vector<amscript2::Value>, amscript2::ScopeView&);

		/* Concatenates all values into a single string. */
		std::vector<amscript2::Value> concat(std::vector<amscript2::Value>, amscript2::ScopeView&);

	}

}

#endif
