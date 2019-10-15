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

#include <random>
#include <sstream>

#include <bid_conf.h> // Intel's definitions

namespace decimal754 {

typedef uint64_t D64; // holds 64-bit decimals
typedef struct __attribute__((aligned(16))) { uint64_t w[2]; } D128; // holds 128-bit decimals
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
extern int __bid128_isSigned (D128 x);
extern int __bid128_isNormal (D128 x);
extern int __bid128_isZero (D128 x);
extern int __bid128_quiet_equal (D128 x, D128 y, ErrorFlags *pfpsf);
extern int __bid128_quiet_less (D128 x, D128 y, ErrorFlags *pfpsf); // TODO: SIGNALLING?

/* 64-bit functions */
extern D64 __bid64_from_string (char *ps, RoundMode rnd_mode, ErrorFlags *pfpsf);
extern D64 __bid64_from_uint32 (uint32_t);
extern D64 __bid64_from_uint64 (uint64_t);
extern D64 __bid64_from_int32 (int32_t);
extern D64 __bid64_from_int64 (int64_t);
extern D64 __bid64_round_integral_zero (D128 x, ErrorFlags *pfpsf);
extern D64 __bid64_abs (D64 x);
extern D64 __bid64_negate (D64 x);
extern D64 __bid64_add ( D64, D64, RoundMode, ErrorFlags *);
extern D64 __bid64_sub ( D64, D64, RoundMode, ErrorFlags *);
extern D64 __bid64_mul ( D64, D64, RoundMode, ErrorFlags *);
extern D64 __bid64_div ( D64, D64, RoundMode, ErrorFlags *);
extern void __bid64_to_string ( char *ps, D64 x, ErrorFlags *pfpsf);
extern uint8_t __bid64_to_uint8_xrnint (D64 x, ErrorFlags *pfpsf);
extern uint16_t __bid64_to_uint16_xrnint (D64 x, ErrorFlags *pfpsf);
extern uint32_t __bid64_to_uint32_xrnint (D64 x, ErrorFlags *pfpsf);
extern uint64_t __bid64_to_uint64_xrnint (D64 x, ErrorFlags *pfpsf);
extern int8_t __bid64_to_int8_xrnint (D64 x, ErrorFlags *pfpsf);
extern int16_t __bid64_to_int16_xrnint (D64 x, ErrorFlags *pfpsf);
extern int32_t __bid64_to_int32_xrnint (D64 x, ErrorFlags *pfpsf);
extern int64_t __bid64_to_int64_xrnint (D64 x, ErrorFlags *pfpsf);
extern int __bid64_isSigned (D64 x);
extern int __bid64_isNormal (D128 x);
extern int __bid64_isZero (D64 x);
extern int __bid64_quiet_equal (D64 x, D64 y, ErrorFlags *pfpsf);
extern int __bid64_quiet_less (D64 x, D64 y, ErrorFlags *pfpsf);
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
		Any				= 0xFF
	};
	
	struct Exception : public std::runtime_error {
		const ErrorFlags flags;
		const IDecimal & decimal;

		Exception(const std::string & message, ErrorFlags flags, const IDecimal & decimal) 
			: std::runtime_error(message), flags(flags), decimal(decimal) {}
		Exception(const char * message, ErrorFlags flags, const IDecimal & decimal) 
			: std::runtime_error(message), flags(flags), decimal(decimal) {}
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
	T val; // either D64 or D128

	RoundMode _round_mode = Round::NearestEven;
	ErrorFlags _errors = Error::None;
	ErrorFlags _throw = Error::None;
	
	// throws an exception if an invalid operation occurred	
	void check_flags(const ErrorFlags flags) const { 
		if ((flags & Error::Invalid) == Error::Invalid) {
			throw DecimalBase::InvalidException("Invalid decimal operation", flags, *this);
		} else if ((flags & Error::DivideByZero) == Error::DivideByZero && 
			(this->_throw & Error::DivideByZero) == Error::DivideByZero) {
			throw DecimalBase::DivideByZeroException("Attempt to divide by zero", flags, *this);
		} else if ((flags & Error::Overflow) == Error::Overflow && 
			(this->_throw & Error::Overflow) == Error::Overflow) {
			throw DecimalBase::OverflowException("Overflow", flags, *this);
		} else if ((flags & Error::Underflow) == Error::Underflow && 
			(this->_throw & Error::Underflow) == Error::Underflow) {
			throw DecimalBase::UnderflowException("Underflow", flags, *this);
		} else if ((flags & Error::Inexact) == Error::Inexact && 
			(this->_throw & Error::Inexact) == Error::Inexact) {
			throw DecimalBase::InexactException("Inexact", flags, *this);
		}
	}

	// saves the flags so they can be read later,
	// and checks them for invalid operations
	void save_flags(const ErrorFlags flags) {
		this->_errors |= flags;
		this->check_flags(flags);
	}

	// throws an exception if an operation involves 
	// multiple values, but their respective rounding modes
	// are not the same
	void check_rounding(const RoundMode against) const {
		if (this->_round_mode != against) {
			throw MismatchedRoundingException(this->_round_mode, against);
		}
	}

	// functions used for conducting operations.
	// since different functions are needed 
	// depending on the data type of T,
	// we have to attach these when the object is constructed
	std::function<T(const uint32_t)> from_uint32;
	std::function<T(const uint64_t)> from_uint64;
	std::function<T(char*, RoundMode, ErrorFlags*)> from_string;
	std::function<T(int32_t)> from_int32;
	std::function<T(int64_t)> from_int64;
	std::function<void(char*, T, ErrorFlags*)> to_string;
	std::function<uint8_t(T, ErrorFlags*)> to_uint8_xrnint;
	std::function<uint16_t(T, ErrorFlags*)> to_uint16_xrnint;
	std::function<uint32_t(T, ErrorFlags*)> to_uint32_xrnint;
	std::function<uint64_t(T, ErrorFlags*)> to_uint64_xrnint;
	std::function<int8_t(T, ErrorFlags*)> to_int8_xrnint;
	std::function<int16_t(T, ErrorFlags*)> to_int16_xrnint;
	std::function<int32_t(T, ErrorFlags*)> to_int32_xrnint;
	std::function<int64_t(T, ErrorFlags*)> to_int64_xrnint;
	std::function<int(T)> _is_signed;
	std::function<int(T)> _is_normal;
	std::function<int(T)> _is_zero;
	std::function<int(T, T, ErrorFlags*)> quiet_equal;
	std::function<int(T, T, ErrorFlags*)> quiet_less;
	std::function<T(T, ErrorFlags *)> round_integral_zero;
	std::function<T(T)> abs;
	std::function<T(T)> negate;
	std::function<T(T, T, RoundMode, ErrorFlags*)> _add;
	std::function<T(T, T, RoundMode, ErrorFlags*)> _sub;
	std::function<T(T, T, RoundMode, ErrorFlags*)> _mul;
	std::function<T(T, T, RoundMode, ErrorFlags*)> _div;
	
	virtual void attach_bid_functions() = 0;
	
	// helper method to invoke an operation
	// this method does not save any flags
	// (it does not modify the object)	
	void invoke(std::function<void(ErrorFlags*)> func) const {
		ErrorFlags flags = Error::None;
		func(&flags);
		check_flags(flags);
	}
	
	// helper method to invoke an operation
	// this method does not save any flags
	// (it does not modify the object)	
	void invoke(std::function<void(ErrorFlags*)> func) {
		ErrorFlags flags = Error::None;
		func(&flags);
		save_flags(flags);
	}

	// helper method to invoke an operation
	// this method does not save any flags
	// (it does not modify the object)	
	// this method returns the result of the operation
	template <class TResult>
	TResult invoke(std::function<TResult(ErrorFlags*)> func) const {
		ErrorFlags flags = Error::None;
		TResult result = func(&flags);
		check_flags(flags);
		return result;
	}
	
	// helper method to invoke an operation
	// this method saves any flags that the operation sets
	// this method returns the result of the operation
	template <class TResult>
	TResult invoke(std::function<TResult(ErrorFlags*)> func) {
		ErrorFlags flags = Error::None;
		TResult result = func(&flags);
		this->save_flags(flags);
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
	
	// used by derived classes to set the rounding mode
	DecimalBase(const RoundMode & round_mode = Round::NearestEven) : _round_mode(round_mode) {}
	
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
	
public:	 	
	void throw_on(const Error & error) { this->_throw |= error; }
	void throw_off(const Error & error) { this->_throw ^= error; }

	const unsigned int errors() const { return this->_errors; }
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
	// TODO: Inexact strings are ugly
	const std::string str() const {
		char buffer[42];
		buffer[41] = '\0';
		
		this->invoke([&](ErrorFlags * flags) {
			this->to_string (buffer, this->val, flags);
		});

		std::string s = buffer;
		auto len = s.length();
		auto start = 0;
		auto count = len;
		
		if (s[0] == '+') {
			start += 1;
		} 

		if (len > 2 && s.substr(len-3, 3) == "E+0") {
			count = len - 3 - start;
		}
			
		return s.substr(start, count);
	}
	
	const bool is_negative() const {
		return (this->_is_signed(this->val) > 0);
	}
	
	const bool is_normal() const {
		return (this->_is_normal(this->val) > 0);
	}
	
	const bool is_zero() const {
		return (this->_is_zero(this->val) > 0);
	}
	
	// == operators to convert to native types == //
	explicit operator unsigned char() const { 
		return this->invoke<unsigned char>([&](ErrorFlags * flags) {
			return this->to_uint8_xrnint (this->val, flags);
		});
	}
	
	explicit operator unsigned short() const { 
		return this->invoke<unsigned short>([&](ErrorFlags * flags) {
			return this->to_uint16_xrnint (this->val, flags);
		});
	}

	explicit operator unsigned int() const { 
		return this->invoke<unsigned int>([&](ErrorFlags * flags) {
			return this->to_uint32_xrnint (this->val, flags);
		});
	}
	
	explicit operator unsigned long() const { 
		return this->invoke<unsigned long>([&](ErrorFlags * flags) {
			return this->to_uint64_xrnint (this->val, flags);
		});
	}
	
	explicit operator unsigned long long() const { 
		return this->invoke<unsigned long long>([&](ErrorFlags * flags) {
			return this->to_uint64_xrnint (this->val, flags);
		});
	}
	
	explicit operator char() const { 
		return this->invoke<char>([&](ErrorFlags * flags) {
			return this->to_int8_xrnint (this->val, flags);
		});
	}
	
	explicit operator short() const { 
		return this->invoke<short>([&](ErrorFlags * flags) {
			return this->to_int16_xrnint (this->val, flags);
		});
	}

	explicit operator int() const { 
		return this->invoke<int>([&](ErrorFlags * flags) {
			return this->to_int32_xrnint (this->val, flags);
		});
	}
	
	explicit operator long() const { 
		return this->invoke<long>([&](ErrorFlags * flags) {
			return this->to_int64_xrnint (this->val, flags);
		});
	}
	
	explicit operator long long() const { 
		return this->invoke<long long>([&](ErrorFlags * flags) {
			return this->to_int64_xrnint (this->val, flags);
		});
	}

	void truncate() {
		this->val =  this->invoke<T>([&](ErrorFlags * flags) {
			return this->round_integral_zero (this->val, flags);
		});
	}
		
	// == arithmetic operators == //	
	void inline add(const DecimalBase & other, const RoundMode round_mode = Round::NearestEven) {
		this->val = this->invoke<D128>([&](ErrorFlags * flags) {
			return this->_add(this->val, other.val, round_mode, flags);
		});
	}

	void inline operator+=(const DecimalBase & other) {
		check_rounding(other._round_mode);
		this->add(other, this->_round_mode);
	}
	
	void inline subtract(const DecimalBase & other, const RoundMode round_mode = Round::NearestEven) {
		this->val = this->invoke<D128>([&](ErrorFlags * flags) {
			return this->_sub(this->val, other.val, round_mode, flags);
		});
	}
	
	void inline operator-=(const DecimalBase & other) {
		check_rounding(other._round_mode);
		this->val = this->invoke<D128>([&](ErrorFlags * flags) {
			return this->_sub(this->val, other.val, this->_round_mode, flags);
		});
	}
	
	void inline multiply(const DecimalBase & other, const RoundMode round_mode = Round::NearestEven) {
		this->val = this->invoke<D128>([&](ErrorFlags * flags) {
			return this->_mul(this->val, other.val, round_mode, flags);
		});
	}
	
	void inline operator*=(const DecimalBase & other) {
		check_rounding(other._round_mode);
		this->val = this->invoke<D128>([&](ErrorFlags * flags) {
			return this->_mul(this->val, other.val, this->_round_mode, flags);
		});
	}
	
	void inline divide(const DecimalBase & other, const RoundMode round_mode = Round::NearestEven) {
		this->val = this->invoke<D128>([&](ErrorFlags * flags) {
			return this->_div(this->val, other.val, round_mode, flags);
		});
	}
	
	void inline operator/=(const DecimalBase & other) {
		check_rounding(other._round_mode);
		this->val = this->invoke<D128>([&](ErrorFlags * flags) {
			return this->_div(this->val, other.val, this->_round_mode, flags);
		});
	}

	// == comparison operators == //	// TODO
	friend inline bool operator!=(const DecimalBase & l, const DecimalBase & r) { return !(l == r); }
	friend inline bool operator==(const DecimalBase & l, const DecimalBase & r) { 
		ErrorFlags flags = Error::None;
		auto result = l.quiet_equal (l.val, r.val, &flags);
		return result > 0;
	

		//return (l.invoke<int>([&](ErrorFlags * flags) {
			//return l.quiet_equal(l.val, r.val, flags);
		//}) > 0);
	}

	friend inline bool operator>(const DecimalBase & l, const DecimalBase & r) { return r < l || l == r; }
	friend inline bool operator<=(const DecimalBase & l, const DecimalBase & r) { return l < r || l == r; }
	friend inline bool operator>=(const DecimalBase & l, const DecimalBase & r) { return l > r || l == r; }
	friend inline bool operator<(const DecimalBase & l, const DecimalBase & r) {
		ErrorFlags flags = Error::None;
		auto result = l.quiet_less (l.val, r.val, &flags);
		return result > 0;
	}
		
	friend inline std::ostream& operator<<(std::ostream& stream, const DecimalBase & decimal) {
		stream << decimal.str();
		return stream;
	}	
};


class LongDecimal : public DecimalBase<D128> {
public:
	static const short precision = 34;
	static const short emax = 6111;
	static const short emin = -6176;

	LongDecimal() : LongDecimal(static_cast<int>(0)) {}
	
	LongDecimal(const unsigned char value, const RoundMode round_mode = Round::NearestEven) 
		: LongDecimal((unsigned int) value, round_mode) {}

	LongDecimal(const unsigned short value, const RoundMode round_mode = Round::NearestEven) 
		: LongDecimal((unsigned int) value, round_mode) {}

	LongDecimal(const unsigned int value, const RoundMode round_mode = Round::NearestEven) 
		: DecimalBase(round_mode) { 
			this->attach_bid_functions();
			this->val = this->from_uint32(value); 
	}

	LongDecimal(const unsigned long value, const RoundMode round_mode = Round::NearestEven) 
		: LongDecimal((unsigned long long) value, round_mode) {}

	LongDecimal(const unsigned long long value, const RoundMode round_mode = Round::NearestEven) 
		: DecimalBase(round_mode) { 
			this->attach_bid_functions();
			this->val = this->from_uint64(value); 
	}
	
	LongDecimal(const char value, const RoundMode round_mode = Round::NearestEven) 
		: LongDecimal((int) value, round_mode) {}

	LongDecimal(const short value, const RoundMode round_mode = Round::NearestEven) : LongDecimal((int) value, round_mode) {}

	LongDecimal(const int value, const RoundMode round_mode = Round::NearestEven) : DecimalBase(round_mode) {
		this->attach_bid_functions();
		this->val = this->from_int32(value); 
	}

	LongDecimal(const long value, const RoundMode round_mode = Round::NearestEven) : LongDecimal((long long) value, round_mode) {}

	LongDecimal(const long long value, const RoundMode round_mode = Round::NearestEven) : LongDecimal(round_mode) { 
		this->attach_bid_functions();
		this->val = this->from_int64(value); 
	}			
	
	LongDecimal(const std::string & value, const RoundMode round_mode = Round::NearestEven) 
		: DecimalBase(round_mode) {
		this->attach_bid_functions();
		auto val = value.empty()? "0" : value;
		auto c = cstr( (value.empty()? "0" : value.c_str()) );
		this->val = this->invoke<D128>([&](ErrorFlags * flags) {
			return this->from_string(c.val, this->_round_mode, flags);
		});
	}
	
	LongDecimal(const char * value, const RoundMode round_mode = Round::NearestEven) 
		: DecimalBase(round_mode) {
		this->attach_bid_functions();
		auto c = cstr( (strlen(value) == 0? "0" : value) );
		this->val = this->invoke<D128>([&](ErrorFlags * flags) {
			return this->from_string(c.val, this->_round_mode, flags);
		});
	}

	void attach_bid_functions() override {
		this->from_string = bid128_from_string;
		this->from_uint32 = bid128_from_uint32;
		this->from_uint64 = bid128_from_uint64;
		this->from_int32 = bid128_from_int32;
		this->from_int64 = bid128_from_int64;
		this->to_string = bid128_to_string;
		this->to_uint8_xrnint = bid128_to_uint8_xrnint;
		this->to_uint16_xrnint = bid128_to_uint16_xrnint;
		this->to_uint32_xrnint = bid128_to_uint32_xrnint;
		this->to_uint64_xrnint = bid128_to_uint64_xrnint;
		this->to_int8_xrnint = bid128_to_int8_xrnint;
		this->to_int16_xrnint = bid128_to_int16_xrnint;
		this->to_int32_xrnint = bid128_to_int32_xrnint;
		this->to_int64_xrnint = bid128_to_int64_xrnint;
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

		this->val = this->from_uint64(0);
	}
	
	friend inline LongDecimal truncate(const LongDecimal & v) {
		LongDecimal v2 = v;
		v2.truncate();
		return v2;
	};
		
	friend inline LongDecimal abs(const LongDecimal & v) {
		LongDecimal v2 = v;
		v2.val = v.abs(v2.val);
		return v2;
	}	

	friend inline LongDecimal operator-(const LongDecimal & v) {
		LongDecimal v2 = v;
		v2.val = v.negate(v2.val);
		return v2;
	}	

	friend inline LongDecimal add(const LongDecimal & l, const LongDecimal & r, const RoundMode round_mode = Round::NearestEven) {
		LongDecimal tmp = l;
		tmp.add(r, round_mode);
		return tmp;
	}
	
	friend inline LongDecimal operator+(const LongDecimal & l, const LongDecimal & r) {
		l.check_rounding(r._round_mode);
		LongDecimal tmp = l;
		tmp += r;
		return tmp;
	}	
	
	friend inline LongDecimal subtract(const LongDecimal & l, const LongDecimal & r, const RoundMode round_mode = Round::NearestEven) {
		LongDecimal tmp = l;
		tmp.subtract(r, round_mode);
		return tmp;
	}
	
	friend inline LongDecimal operator-(const LongDecimal & l, const LongDecimal & r) {
		LongDecimal tmp = l;
		tmp -= r;
		return tmp;
	}	
	
	friend inline LongDecimal multiply(const LongDecimal & l, const LongDecimal & r, const RoundMode round_mode = Round::NearestEven) {
		LongDecimal tmp = l;
		tmp.multiply(r, round_mode);
		return tmp;
	}
	
	friend inline LongDecimal operator*(const LongDecimal & l, const LongDecimal & r) {
		LongDecimal tmp = l;
		tmp *= r;
		return tmp;
	}	
	
	friend inline LongDecimal divide(const LongDecimal & l, const LongDecimal & r, const RoundMode round_mode = Round::NearestEven) {
		LongDecimal tmp = l;
		tmp.divide(r, round_mode);
		return tmp;
	}
	
	friend inline LongDecimal operator/(const LongDecimal & l, const LongDecimal & r) {
		LongDecimal tmp = l;
		tmp /= r;
		return tmp;
	}	
		
	// Generates a random decimal	
	// Does not generate Inf, -Inf, NaN, or subnormal numbers
	static const LongDecimal random() { 	
		auto result = random_str(LongDecimal::precision, LongDecimal::emin, LongDecimal::emax);
		return LongDecimal(result);
	};	
};

class Decimal : public DecimalBase<D64> {
public:		
	static const short precision = 16;
	static const short emax = 369;
	static const short emin = -398;

	Decimal() : DecimalBase() {}
	
	Decimal(const std::string & value) : Decimal() {}
	Decimal(const unsigned char value) : Decimal() {}
	Decimal(const unsigned short value) : Decimal() {}
	Decimal(const unsigned int value) : Decimal() {}
	Decimal(const unsigned long value) : Decimal() {}
	Decimal(const unsigned long long value) : Decimal() {}
	Decimal(const char value) : Decimal() {}
	Decimal(const short value) : Decimal() {}
	Decimal(const int value) : Decimal() {}
	Decimal(const long value) : Decimal() {}
	Decimal(const long long value) : Decimal() {}
	
	void attach_bid_functions() override { 
		// TODO:
	}
	
	// Generates a random decimal	
	// Does not generate Inf, -Inf, NaN, or subnormal numbers
	static const Decimal random() { 	
		auto result = random_str(Decimal::precision, Decimal::emin, Decimal::emax);
		return Decimal(result);
	};	
};

namespace decimal {
	static Decimal Zero(0);
	static Decimal One(1);
	static Decimal Max(std::string("9999999999999999999999999999999999") + "E+6111"); // TODO - WRONG EMAX
	static Decimal Min(std::string("-9999999999999999999999999999999999") + "E+6111");
	static Decimal SmallestPositive(std::string("9999999999999999999999999999999999") + "E-6176");
	static Decimal SmallestNegative(std::string("-9999999999999999999999999999999999") + "E-6176");
	static Decimal Inf("Inf");
}

namespace longDecimal {
	static LongDecimal Zero(0);
	static LongDecimal One(1);
	static LongDecimal Max(std::string("9999999999999999999999999999999999") + "E+6111");
	static LongDecimal Min(std::string("-9999999999999999999999999999999999") + "E+6111");
	static LongDecimal SmallestPositive(std::string("9999999999999999999999999999999999") + "E-6176");
	static LongDecimal SmallestNegative(std::string("-9999999999999999999999999999999999") + "E-6176");
	static LongDecimal Inf("Inf");
}

} // namespace decimal754

#endif // DECIMAL_H
