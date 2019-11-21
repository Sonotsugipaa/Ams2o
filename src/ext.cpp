#include "ams2o/util.tpp"



namespace ams2o::ext {

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
		using val = amscript2::Value;
		ext.define(ref_t("big_endian"),   val(AMS2O_BIG_ENDIAN),   true);
		ext.define(ref_t("small_endian"), val(AMS2O_SMALL_ENDIAN), true);
		ext.define(ref_t("int8"),   val(AMS2O_SIZE_8),   true);
		ext.define(ref_t("int16"),  val(AMS2O_SIZE_16),  true);
		ext.define(ref_t("int32"),  val(AMS2O_SIZE_32),  true);
		ext.define(ref_t("int64"),  val(AMS2O_SIZE_64),  true);
		ext.define(ref_t("uint8"),  val(AMS2O_SIZE_8U),  true);
		ext.define(ref_t("uint16"), val(AMS2O_SIZE_16U), true);
		ext.define(ref_t("uint32"), val(AMS2O_SIZE_32U), true);
		ext.define(ref_t("uint64"), val(AMS2O_SIZE_64U), true);
		ext.define(ref_t("true"),   val(AMS2O_TRUE),     true);
		ext.define(ref_t("false"),  val(AMS2O_FALSE),    true);
		ext.define(ref_t("binary"), val(AMS2O_BINARY),   true);
		ext.define(ref_t("text"),   val(AMS2O_TEXT),     true);
		ext.define(ref_t(AMS2O_NS, "ascii"),  ascii,  true);
		ext.define(ref_t(AMS2O_NS, "concat"), concat, true);
		return ext;
	}();

}
