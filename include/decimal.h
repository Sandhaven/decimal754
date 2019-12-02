/*
 *  decimal.h
 *  C++ implementation of Intel's Decimal Floating-Point Math Library
 *  https://software.intel.com/en-us/articles/intel-decimal-floating-point-math-library
 *
 *  Based on IEEE 754-2008
 *  https://standards.ieee.org/standard/754-2008.html
 *
 *  Copyright 2019 Sandhaven, LLC
 *  
 *  Permission is hereby granted, free of charge, to any person obtaining 
 *  a copy of this software and associated documentation files (the "Software"), 
 *  to deal in the Software without restriction, including without limitation the 
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or 
 *  sell copies of the Software, and to permit persons to whom the Software is 
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included 
 *  in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR 
 *  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef DECIMAL_H
#define DECIMAL_H

#include <algorithm>
#include <random>
#include <sstream>

#include <bid_conf.h> // Intel's definitions

namespace decimal754 {

// holds 64-bit decimals
typedef uint64_t D64; 

// holds 128-bit decimals
typedef struct __attribute__((aligned(16))) D128 { 
	uint64_t w[2];
	friend inline bool operator!=(const D128 & l, const D128 & r) { return !(l == r); }
	friend inline bool operator==(const D128 & l, const D128 & r) { return (l.w[0] == r.w[0]) && (l.w[1] == r.w[1]); }
} D128; 

typedef unsigned int RoundMode;
typedef unsigned int ErrorFlags;

// Declare the C functions we will be linking from Intel's library (libbid.a)
extern "C" {

/* 128-bit functions */
extern D128 __bid128_from_string (char *ps, RoundMode rnd_mode, ErrorFlags *pfpsf);
extern D128 __bid128_from_uint32 (uint32_t);
extern D128 __bid128_from_uint64 (uint64_t);
extern D128 __bid128_from_int32 (int32_t);
extern D128 __bid128_from_int64 (int64_t);
extern D128 __bid128_round_integral_zero (D128 x, ErrorFlags *pfpsf);
extern D128 __bid128_abs (D128 x);
extern D128 __bid128_negate (D128 x);
extern D128 __bid128_add ( D128, D128, RoundMode, ErrorFlags *);
extern D128 __bid128_sub ( D128, D128, RoundMode, ErrorFlags *);
extern D128 __bid128_mul ( D128, D128, RoundMode, ErrorFlags *);
extern D128 __bid128_div ( D128, D128, RoundMode, ErrorFlags *);
extern void __bid128_to_string ( char *ps, D128 x, ErrorFlags *pfpsf);
extern uint8_t __bid128_to_uint8_xrnint (D128 x, ErrorFlags *pfpsf);
extern uint16_t __bid128_to_uint16_xrnint (D128 x, ErrorFlags *pfpsf);
extern uint32_t __bid128_to_uint32_xrnint (D128 x, ErrorFlags *pfpsf);
extern uint64_t __bid128_to_uint64_xrnint (D128 x, ErrorFlags *pfpsf);
extern int8_t __bid128_to_int8_xrnint (D128 x, ErrorFlags *pfpsf);
extern int16_t __bid128_to_int16_xrnint (D128 x, ErrorFlags *pfpsf);
extern int32_t __bid128_to_int32_xrnint (D128 x, ErrorFlags *pfpsf);
extern int64_t __bid128_to_int64_xrnint (D128 x, ErrorFlags *pfpsf);
extern float __bid128_to_binary32 (D128 x, RoundMode rnd_mode, ErrorFlags *pfpsf);
extern double __bid128_to_binary64 (D128 x, RoundMode rnd_mode, ErrorFlags *pfpsf);
extern D128 __binary32_to_bid128 (float x, _IDEC_round rnd_mode, _IDEC_flags *pfpsf);
extern D128 __binary64_to_bid128 (double x, _IDEC_round rnd_mode, _IDEC_flags *pfpsf);
extern int __bid128_isSigned (D128 x);
extern int __bid128_isNormal (D128 x);
extern int __bid128_isZero (D128 x);
extern int __bid128_quiet_equal (D128 x, D128 y, ErrorFlags *pfpsf);
extern int __bid128_quiet_less (D128 x, D128 y, ErrorFlags *pfpsf); // TODO: SIGNALLING?

/* 64-bit functions */ // TODO: Implement 64-bit C functions
}

class IDecimal {
public:
	// rounding modes
	// NearestEven apparently means half-even
	enum Round: unsigned int{
		NearestEven = 0,	// 3.1415 -> 3.142,		2.71828182845 -> 2.7182818284
		Downward    = 1,	// -3.1415 -> -3.142	3.1415 -> 3.141
		Upward      = 2,	// -3.1415 -> 3.141     3.1415 -> 3.142
		TowardZero  = 3,	// -3.1415 -> -3.141	3.1415 -> 3.141
		NearestAway = 4		// 3.1415 -> 3.142,		2.71828182845 -> 2.7182818285
	};
	
	enum Error: unsigned int {
		None			= 0x00,
		Invalid			= 0x01,
		DivideByZero	= 0x04,
		Overflow		= 0x08,
		Underflow		= 0x10,
		Inexact			= 0x20,
		Undefined		= Error::Invalid | Error::DivideByZero | Error::Overflow | Error::Underflow,
		Any				= 0xFF
	};
	
	struct Exception : public std::runtime_error {
		const ErrorFlags flags;
		const IDecimal * decimal;

		Exception(const std::string & message, ErrorFlags flags = Error::None, const IDecimal * decimal = nullptr) 
			: std::runtime_error(message), flags(flags), decimal(decimal) {}
		Exception(const char * message, ErrorFlags flags = Error::None, const IDecimal * decimal = nullptr) 
			: Exception(std::string(message), flags, decimal) {}
	};
	
	struct InvalidException : public Exception { using Exception::Exception; };
	struct DivideByZeroException : public Exception { using Exception::Exception; };
	struct OverflowException : public Exception { using Exception::Exception; };
	struct UnderflowException : public Exception { using Exception::Exception; };
	struct InexactException : public Exception { using Exception::Exception; };
	struct MismatchedRoundingException : public std::runtime_error { 
		const RoundMode mode1;
		const RoundMode mode2;
		MismatchedRoundingException(const RoundMode mode1, const RoundMode mode2) :
			std::runtime_error("Mismatched rounding modes"), mode1(mode1), mode2(mode2) {}
   	};
	struct NonDecimalException: public std::runtime_error {
		const std::string value; 
		NonDecimalException(const char value) :
			std::runtime_error("Non-decimal value received"), value(std::string(1, value)) {}
		NonDecimalException(const char * value) :
			std::runtime_error("Non-decimal value received"), value(value) {}
		NonDecimalException(const std::string & value) :
			std::runtime_error("Non-decimal value received"), value(value) {}
	};
};

// Base class for decimal types
template <class T>
class DecimalBase : public IDecimal {
protected:
	T _val;
	RoundMode _round_mode = Round::NearestEven;
	ErrorFlags _errors = Error::None;
	ErrorFlags _throw = Error::None;
	
	virtual void _attach_bid_functions() = 0;
	virtual const unsigned short _precision() = 0;
	virtual const unsigned short _emax() = 0;
	virtual const unsigned short _emin() = 0;
	
	// functions used for conducting operations.
	// since different functions are needed 
	// depending on the data type of T,
	// we have to attach these when the object is constructed
	std::function<void(char *, D128, ErrorFlags*)> to_string;
	std::function<uint8_t(D128, ErrorFlags*)> to_uint8_xrnint;
	std::function<uint16_t(D128, ErrorFlags*)> to_uint16_xrnint;
	std::function<uint32_t(D128, ErrorFlags*)> to_uint32_xrnint;
	std::function<uint64_t(D128, ErrorFlags*)> to_uint64_xrnint;
	std::function<int8_t(D128, ErrorFlags*)> to_int8_xrnint;
	std::function<int16_t(D128, ErrorFlags*)> to_int16_xrnint;
	std::function<int32_t(D128, ErrorFlags*)> to_int32_xrnint;
	std::function<int64_t(D128, ErrorFlags*)> to_int64_xrnint;
	std::function<float(D128, RoundMode, ErrorFlags*)> to_binary32;
	std::function<double(D128, RoundMode, ErrorFlags*)> to_binary64;
	std::function<int(D128)> _is_signed;
	std::function<int(D128)> _is_normal;
	std::function<int(D128)> _is_zero;
	std::function<int(D128, D128, ErrorFlags*)> quiet_equal;
	std::function<int(D128, D128, ErrorFlags *)> quiet_less;
	std::function<D128(D128, ErrorFlags*)> round_integral_zero;
	std::function<D128(D128)> abs;
	std::function<D128(D128)> negate;
	std::function<D128(D128, D128, RoundMode, ErrorFlags *)> _add;
	std::function<D128(D128, D128, RoundMode, ErrorFlags *)> _sub;
	std::function<D128(D128, D128, RoundMode, ErrorFlags *)> _mul;
	std::function<D128(D128, D128, RoundMode, ErrorFlags *)> _div;
	
	// throws an exception if an invalid operation occurred	
	static void check_flags(const ErrorFlags flags, const ErrorFlags throw_on_err = Error::Undefined) { 
		if ((flags & Error::Invalid) == Error::Invalid) {
			throw IDecimal::InvalidException("Invalid decimal operation", flags, nullptr);
		} else if ((flags & throw_on_err & Error::DivideByZero) == Error::DivideByZero) {
			throw IDecimal::DivideByZeroException("Attempt to divide by zero", flags, nullptr);
		} else if ((flags & throw_on_err & Error::Overflow) == Error::Overflow) {
			throw IDecimal::OverflowException("Overflow", flags, nullptr);
		} else if ((flags & throw_on_err & Error::Underflow) == Error::Underflow) {
			throw IDecimal::UnderflowException("Underflow", flags, nullptr);
		} else if ((flags & throw_on_err & Error::Inexact) == Error::Inexact) {
			throw IDecimal::InexactException("Inexact", flags, nullptr);
		}
	}

	// saves the flags so they can be read later,
	// and checks them for invalid operations
	void save_flags(const ErrorFlags flags, const ErrorFlags throw_on_err = Error::Undefined) {
		this->_errors |= flags;
		this->check_flags(flags, throw_on_err);
	}

	// helper method to invoke an operation
	// this method does not save any flags
	// (it does not modify the object)	
	static void invoke(std::function<void(ErrorFlags*)> func, const ErrorFlags throw_on_err = Error::Undefined) {
		ErrorFlags flags = Error::None;
		func(&flags);
		check_flags(flags, throw_on_err);
	}

	// helper method to invoke an operation
	// this method does not save any flags
	// (it does not modify the object)	
	// this method returns the result of the operation
	template <class TResult>
	static TResult invoke(std::function<TResult(ErrorFlags*)> func, const ErrorFlags throw_on_err = Error::Undefined) {
		ErrorFlags flags = Error::None;
		TResult result = func(&flags);
		check_flags(flags, throw_on_err);
		return result;
	}
	
	// encapsulates construction / destruction of a char*
	struct cstr {
		char* val;
		cstr(std::string & s) {
			this->val = new char[s.length() + 1];
			strcpy(this->val, s.c_str());
		}
		cstr(const char * c) {
			this->val = new char[strlen(c) + 1];
			strcpy(this->val, c);
		}
		~cstr() {
			delete[] this->val;
		}
	};
		
	static const std::string random_str(const short precision, const short emin, const short emax) {
		// Algorithm derived from IEEE754-2008, Page 8
		// Signed zero and non-zero floating-point numbers of the form (−1)^s*b^q*c, where
		// 	s is 0 or 1.
		// 	q is any integer emin <= q + p − 1 <= emax.
		// 	c is a number represented by a digit string of the form
		//	d0 d1 d2 ... dp−1 where di is an integer digit 0 <= di < b (c is therefore an integer with 0 <= c < b^p).

		// setup a random number generator,
		// make it static so we don't have to rebuild it with every call
 		static std::random_device rd;
    	static std::mt19937_64 gen(rd());

		// use a bernoulli probability distribution to decide whether the number will be negative
		static auto sign_dist = std::bernoulli_distribution(0.5);

		// use a uniform integer distribution to generate the digits of the significand
		// create two 64-bit integers, and use modulus to extract the first p / 2 bits of each
		static auto significand_dist = std::uniform_int_distribution<uint64_t>(0, std::numeric_limits<uint64_t>::max());
		static const long mask = pow(10, precision >> 1);
		
		// use a uniform integer distribution to generate the exponent in the range [emin - p + 1, emax - p + 1]
		static auto exponent_dist = std::uniform_int_distribution<short>(emin, emax);
	
		// generate the random parameters	
		bool negative = sign_dist(gen);
		uint64_t significand_low = significand_dist(gen) % mask;
		uint64_t significand_high = significand_dist(gen) % mask;
		short exponent = exponent_dist(gen);

		// create a string with the following format:
		// "sDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDE+DDDD" + \0
		auto n = precision + 8;
		char result[n];
		sprintf(result, "%c%llu%lluE%+d", (negative? '-' : '+'), significand_high, significand_low, exponent);
		return result;
	}

	// All derived constructors must eventually invoke this base constructor	
	DecimalBase(const T value, 
				const RoundMode round_mode = Round::NearestEven, 
				const ErrorFlags throw_on_err = Error::Undefined)  
		: _val(value), _round_mode(round_mode), _throw(throw_on_err) {
			if ((throw_on_err & Error::Invalid) != Error::Invalid) {
				throw Exception("Error::Invalid is required for all operations.");
			}
		}
	
public:	 	
	void throw_on(const Error & error) { this->_throw |= error; }
	void throw_off(const Error & error) { this->_throw ^= error; }

	const unsigned int round_mode() const { return this->_round_mode; }
	const unsigned int errors() const { return this->_errors; }
	const unsigned int throw_on_err() const { return this->_throw; }

	const bool divide_by_zero() const { return ((this->_errors & Error::DivideByZero) == Error::DivideByZero); }
	const bool overflow() const { return ((this->_errors & Error::Overflow)== Error::Overflow); }
	const bool underflow() const { return ((this->_errors & Error::Underflow) == Error::Underflow); }
	const bool inexact() const { return ((this->_errors & Error::Inexact) == Error::Inexact); }

	// combines the given error flags to produce a comma-separated list
	static const std::string error_str(const unsigned int flags) {
		std::stringstream ss;
		std::vector<std::string> v;

		if ((flags & Error::Invalid) == Error::Invalid) {
			v.push_back("invalid");
		} else if ((flags & Error::DivideByZero) == DivideByZero) {
			v.push_back("zerodivide");
		} else if ((flags & Error::Overflow) == Error::Overflow) {
			v.push_back("overflow");
		} else if ((flags & Error::Underflow) == Error::Underflow) {
			v.push_back("underflow");
		} else if ((flags & Error::Inexact) == Error::Inexact) {
			v.push_back("inexact");
		}

		if (v.size() > 0) {
			for(size_t i = 0; i < v.size(); ++i) {
				if (i != 0)
					ss << ", ";
				ss << v[i];
			}
		} else {
			ss << "None";
		}
			
		return ss.str();
	}
	
	// converts the decimal to a string
	const std::string str() {
		assert(this->_precision() > 0);
		auto len = this->_precision() + 8; // 6 for the exponent, 1 for the sign, 1 for \0
		char buf[len];
		this->invoke([&](ErrorFlags * flags) {
			this->to_string (buf, this->_val, flags);
		}, this->_throw);	
			
		buf[len - 1] = '\0'; // in case to_string fails to add one
		return buf;
	}
	
	// TODO: Inexact strings are ugly
	// TODO: scientific notation
	const std::string sci() {
		auto buf = this->str().c_str();
		auto len = strlen(buf);
		assert(len > 3);
		char buf2[len];
		
		if (strcmp(&buf[1], "NaN") == 0 ||
			strcmp(&buf[1], "SNaN") == 0 ||
			strcmp(&buf[1], "Inf") == 0
		) {
			// return special numbers directly
			strcpy(buf2, buf);
		} else {
			// find the first non-zero digit of the significand
			auto start = 1;
			while (start < len && buf[start] == '0') { ++start; }

			// find the end of the significand (beginning of the exponent)
			auto end = len - 1; // the end of the significand
			while (buf[end+1] != 'E') { --end; }
			assert(end < len - 1);	// the exponent must exist
			assert(end > 0);		// there must also be a sign and significand
			assert(end >= start);	// the end cannot be before the start
		
			// compute the exponent	
			auto exp = atoi(&buf[end+2]); // the value of the exponent
			
			// shift the decimal point and adjust the exponent to match
			int dp = end - start;
			exp += dp;
		
			// rewrite the decimal in scientific notation	
			buf2[0] = buf[0]; // copy the sign
			buf2[len-1] = '\0';

			auto dest = (buf[0] == '-'? &buf2[1] : buf2);

			if (dp <= 0) {
				sprintf(dest, "%cE%+d", buf[start], exp);
			} else {
				sprintf(dest, "%c.%.*sE%+d", buf[start], dp, buf + start + 1, exp);
			}
		}

		return buf2;
	}
	
	const bool is_negative() const {
		return (this->_is_signed(this->_val) > 0);
	}
	
	const bool is_normal() const {
		return (this->_is_normal(this->_val) > 0);
	}
	
	const bool is_zero() const {
		return (this->_is_zero(this->_val) > 0);
	}
	
	// == operators to convert to native types == //
	explicit operator unsigned char() const { 
		return this->invoke<unsigned char>([&](ErrorFlags * flags) {
			return this->to_uint8_xrnint (this->_val, flags);
		}, this->_throw);
	}
	
	explicit operator unsigned short() const { 
		return this->invoke<unsigned short>([&](ErrorFlags * flags) {
			return this->to_uint16_xrnint (this->_val, flags);
		}, this->_throw);
	}

	explicit operator unsigned int() const { 
		return this->invoke<unsigned int>([&](ErrorFlags * flags) {
			return this->to_uint32_xrnint (this->_val, flags);
		}, this->_throw);
	}
	
	explicit operator unsigned long() const { 
		return this->invoke<unsigned long>([&](ErrorFlags * flags) {
			return this->to_uint64_xrnint (this->_val, flags);
		}, this->_throw);
	}
	
	explicit operator unsigned long long() const { 
		return this->invoke<unsigned long long>([&](ErrorFlags * flags) {
			return this->to_uint64_xrnint (this->_val, flags);
		}, this->_throw);
	}
	
	explicit operator char() const { 
		return this->invoke<char>([&](ErrorFlags * flags) {
			return this->to_int8_xrnint(this->_val, flags);
		}, this->_throw);
	}
	
	explicit operator short() const { 
		return this->invoke<short>([&](ErrorFlags * flags) {
			return this->to_int16_xrnint(this->_val, flags);
		}, this->_throw);
	}

	explicit operator int() const { 
		return this->invoke<int>([&](ErrorFlags * flags) {
			return this->to_int32_xrnint(this->_val, flags);
		}, this->_throw);
	}
	
	explicit operator long() const { 
		return this->invoke<long>([&](ErrorFlags * flags) {
			return this->to_int64_xrnint(this->_val, flags);
		}, this->_throw);
	}
	
	explicit operator long long() const { 
		return this->invoke<long long>([&](ErrorFlags * flags) {
			return this->to_int64_xrnint(this->_val, flags);
		}, this->_throw);
	}
	
	explicit operator float() const { 
		return this->invoke<float>([&](ErrorFlags * flags) {
			return this->to_binary32(this->_val, this->_round_mode, flags);
		}, this->_throw);
	}

	explicit operator double() const { 
		return this->invoke<float>([&](ErrorFlags * flags) {
			return this->to_binary32(this->_val, this->_round_mode, flags);
		}, this->_throw);
	}

	// == comparison operators == //
	friend inline bool operator!=(const DecimalBase<T> & l, const DecimalBase<T> & r) { return !(l == r); }
	friend inline bool operator==(const DecimalBase<T> & l, const DecimalBase<T> & r) { 
		return (DecimalBase::invoke<int>([&](ErrorFlags * flags) {
			return l.quiet_equal(l._val, r._val, flags);
		}, l._throw | r._throw) > 0);
	}

	friend inline bool operator>(const DecimalBase<T> & l, const DecimalBase<T> & r) { return r < l || l == r; }
	friend inline bool operator<=(const DecimalBase<T> & l, const DecimalBase<T> & r) { return l < r || l == r; }
	friend inline bool operator>=(const DecimalBase<T> & l, const DecimalBase<T> & r) { return l > r || l == r; }
	friend inline bool operator<(const DecimalBase<T> & l, const DecimalBase<T> & r) {
		return (DecimalBase::invoke<int>([&](ErrorFlags * flags) {
			return l.quiet_less(l._val, r._val, flags); // TODO: QUIET?
		}, l._throw | r._throw) > 0);
	}
		
	friend inline std::ostream& operator<<(std::ostream& stream, DecimalBase<T> & decimal) {
		stream << decimal.str();
		return stream;
	}	
};

class LongDecimal final : public DecimalBase<D128> {
private:
	void _attach_bid_functions() override {
		this->to_string = bid128_to_string;
		this->to_uint8_xrnint = bid128_to_uint8_xrnint;
		this->to_uint16_xrnint = bid128_to_uint16_xrnint;
		this->to_uint32_xrnint = bid128_to_uint32_xrnint;
		this->to_uint64_xrnint = bid128_to_uint64_xrnint;
		this->to_int8_xrnint = bid128_to_int8_xrnint;
		this->to_int16_xrnint = bid128_to_int16_xrnint;
		this->to_int32_xrnint = bid128_to_int32_xrnint;
		this->to_int64_xrnint = bid128_to_int64_xrnint;
		this->to_binary32 = bid128_to_binary32;
		this->to_binary64 = bid128_to_binary64;
		this->_is_signed = bid128_isSigned;
		this->_is_normal = bid128_isNormal;
		this->_is_zero = bid128_isZero;
		this->quiet_equal = bid128_quiet_equal;
		this->quiet_less = bid128_quiet_less;
		this->round_integral_zero = bid128_round_integral_zero;
		this->abs = bid128_abs;
		this->negate = bid128_negate;
		this->_add = bid128_add;
		this->_sub = bid128_sub;
		this->_mul = bid128_mul;
		this->_div = bid128_div;
	}

	const unsigned short _precision() override { return LongDecimal::precision ; }
	const unsigned short _emax() override { return LongDecimal::emax ; }
	const unsigned short _emin() override { return LongDecimal::emin ; }

	static const D128 from_string(const std::string & value, const RoundMode round_mode, const ErrorFlags throw_on_err) {
		auto val = value.empty()? "0" : value;
		auto c = cstr( (value.empty()? "0" : value.c_str()) );
		return DecimalBase::invoke<D128>([&](ErrorFlags * flags) {
			return bid128_from_string(c.val, round_mode, flags);
		}, throw_on_err);
	}
	
	static const D128 from_cstring(const char * value, const RoundMode round_mode, const ErrorFlags throw_on_err) {
		auto val = (strcmp(value, "") == 0)? "0" : value;
		auto c = cstr(val);
		return DecimalBase::invoke<D128>([&](ErrorFlags * flags) {
			return bid128_from_string(c.val, round_mode, flags);
		}, throw_on_err);
	}
	
	static const D128 from_float(const float value, const RoundMode round_mode, const ErrorFlags throw_on_err) {
		return DecimalBase::invoke<D128>([&](ErrorFlags * flags) {
			return binary32_to_bid128(value, round_mode, flags);
		}, throw_on_err);
	}

	static const D128 from_double(const double value, const RoundMode round_mode, const ErrorFlags throw_on_err) {
		return DecimalBase::invoke<D128>([&](ErrorFlags * flags) {
			return binary64_to_bid128(value, round_mode, flags);
		}, throw_on_err);
	}
	
	friend inline const LongDecimal binary_op(const LongDecimal & l, const LongDecimal & r, 
											const std::function<D128(D128, D128, RoundMode, ErrorFlags*)> func) {
		if (l._round_mode != r._round_mode) {
			throw MismatchedRoundingException(l._round_mode, r._round_mode);
		}

		ErrorFlags flags = Error::None;
		auto val = func(l._val, r._val, l._round_mode, &flags);
		check_flags(flags, l._throw | r._throw);
		auto v = LongDecimal(val);
		v._errors = flags;
		return v;
	}
    
    LongDecimal(const D128 value,
                const RoundMode round_mode = Round::NearestEven,
                const ErrorFlags throw_on_err = Error::Undefined)
        : DecimalBase<D128>(value, round_mode, throw_on_err) {
        this->_attach_bid_functions();
    }
    
public:
	static const short precision = 34;
	static const short emax = 6111;
	static const short emin = -6176;
	
	LongDecimal() : LongDecimal(static_cast<const int>(0)) {}

	LongDecimal(const unsigned int value, 
				const RoundMode round_mode = Round::NearestEven, 
				const ErrorFlags throw_on_err = Error::Undefined) 
		: LongDecimal(bid128_from_uint32(value), round_mode, throw_on_err) {}
	
	LongDecimal(const unsigned long long value, 
				const RoundMode round_mode = Round::NearestEven, 
				const ErrorFlags throw_on_err = Error::Undefined) 
		: LongDecimal(bid128_from_uint64(value), round_mode, throw_on_err) {}

	LongDecimal(const int value, 
				const RoundMode round_mode = Round::NearestEven, 
				const ErrorFlags throw_on_err = Error::Undefined) 
		: LongDecimal(bid128_from_int32(value), round_mode, throw_on_err) {}

	LongDecimal(const long long value, 
				const RoundMode round_mode = Round::NearestEven, 
				const ErrorFlags throw_on_err = Error::Undefined) 
		: LongDecimal(bid128_from_int64(value), round_mode, throw_on_err) {}			

	LongDecimal(const std::string & value, 
				const RoundMode round_mode = Round::NearestEven, 
				const ErrorFlags throw_on_err = Error::Undefined) 
		: LongDecimal(LongDecimal::from_string(value, round_mode, throw_on_err), round_mode, throw_on_err) {}

	LongDecimal(const char * value, 
				const RoundMode round_mode = Round::NearestEven, 
				const ErrorFlags throw_on_err = Error::Undefined)
		: LongDecimal(LongDecimal::from_cstring(value, round_mode, throw_on_err), round_mode, throw_on_err) {}

	LongDecimal(const unsigned char value, 
				const RoundMode round_mode = Round::NearestEven, 
				const ErrorFlags throw_on_err = Error::Undefined) 
		: LongDecimal(static_cast<unsigned int>(value), round_mode, throw_on_err) {}

	LongDecimal(const unsigned short value, 
				const RoundMode round_mode = Round::NearestEven, 
				const ErrorFlags throw_on_err = Error::Undefined) 
		: LongDecimal(static_cast<unsigned int>(value), round_mode, throw_on_err) {}

	LongDecimal(const unsigned long value, 
				const RoundMode round_mode = Round::NearestEven, 
				const ErrorFlags throw_on_err = Error::Undefined) 
		: LongDecimal(static_cast<unsigned long long>(value), round_mode, throw_on_err) {}
	
	LongDecimal(const char value, 
				const RoundMode round_mode = Round::NearestEven, 
				const ErrorFlags throw_on_err = Error::Undefined) 
		: LongDecimal(static_cast<int>(value), round_mode, throw_on_err) {}

	LongDecimal(const short value, 
				const RoundMode round_mode = Round::NearestEven, 
				const ErrorFlags throw_on_err = Error::Undefined) 
		: LongDecimal(static_cast<int>(value), round_mode, throw_on_err) {}

	LongDecimal(const long value, 
				const RoundMode round_mode = Round::NearestEven, 
				const ErrorFlags throw_on_err = Error::Undefined) 
		: LongDecimal(static_cast<long long>(value), round_mode, throw_on_err) {}

	LongDecimal(const float value, 
				const RoundMode round_mode = Round::NearestEven, 
				const ErrorFlags throw_on_err = Error::Undefined) 
		: LongDecimal(LongDecimal::from_float(value, round_mode, throw_on_err), round_mode, throw_on_err) {}
	
	LongDecimal(const double value, 
				const RoundMode round_mode = Round::NearestEven, 
				const ErrorFlags throw_on_err = Error::Undefined) 
		: LongDecimal(LongDecimal::from_double(value, round_mode, throw_on_err), round_mode, throw_on_err) {}
	
	LongDecimal(const LongDecimal & other)
        : LongDecimal(other._val, other._round_mode, other._throw) {}
    	
	friend inline LongDecimal truncate(const LongDecimal & v) {
		auto t = DecimalBase::invoke<D128>([&](ErrorFlags * flags) {
			return v.round_integral_zero (v._val, flags);
		}, v._throw);
		return LongDecimal(t);
	}
		
	friend inline LongDecimal abs(const LongDecimal & v) {
		auto val2 = DecimalBase::invoke<D128>([&](ErrorFlags * flags) {
			return v.abs(v._val);
		}, v._throw);
		return LongDecimal(val2);
	}	

	friend inline LongDecimal operator-(const LongDecimal & v) {
		auto val2 = DecimalBase::invoke<D128>([&](ErrorFlags * flags) {
			return v.negate(v._val);
		}, v._throw);
		return LongDecimal(val2);
	}	
	
	// == arithmetic operators == //	
	friend inline const LongDecimal operator+(const LongDecimal & l, const LongDecimal & r) {
		return binary_op(l, r, l._add);
	}	
	
	friend inline const LongDecimal operator-(const LongDecimal & l, const LongDecimal & r) {
		return binary_op(l, r, l._sub);
	}	
	
	friend inline const LongDecimal operator*(const LongDecimal & l, const LongDecimal & r) {
		return binary_op(l, r, l._mul);
	}	
	
	friend inline const LongDecimal operator/(const LongDecimal & l, const LongDecimal & r) {
		return binary_op(l, r, l._div);
	}	
	
	// Generates a random decimal	
	// Does not generate Inf, -Inf, NaN, or subnormal numbers
	static const LongDecimal random() { 	
		auto result = random_str(LongDecimal::precision, LongDecimal::emin, LongDecimal::emax);
		return LongDecimal(result);
	};	
};

namespace longDecimal {
	static LongDecimal Zero(0);
	static LongDecimal One(1);
	static LongDecimal Max(std::string("9999999999999999999999999999999999") + "E+6111");
	static LongDecimal Min(std::string("-9999999999999999999999999999999999") + "E+6111");
	static LongDecimal SmallestPositive(std::string("9999999999999999999999999999999999") + "E-6176");
	static LongDecimal SmallestNegative(std::string("-9999999999999999999999999999999999") + "E-6176");
	static LongDecimal Inf("Inf");
	static LongDecimal NaN("+NaN");
}

} // namespace decimal754

#endif // DECIMAL_H
