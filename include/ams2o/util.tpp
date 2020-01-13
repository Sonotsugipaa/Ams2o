#ifndef AMS2O_UTIL_HPP
#define AMS2O_UTIL_HPP

#include "ams2o/ams2o.hpp"



namespace ams2o {

	constexpr size_t heuristic_length_text(size_t n_of_values) {
		return 7 * n_of_values; }

	constexpr size_t heuristic_length_binary(size_t n_of_values) {
		return 3 * n_of_values; }


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

}

#endif
