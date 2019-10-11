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
extern int __bid128_quiet_equal (D128 x, D128 y, ErrorFlags *pfpsf);
extern int __bid128_quiet_less (D128 x, D128 y, ErrorFlags *pfpsf); // TODO: SIGNALLING?

/* 64-bit functions */
extern D64 __bid64_from_string (char *ps, RoundMode rnd_mode, ErrorFlags *pfpsf);
extern D64 __bid64_from_uint32 (uint32_t);
extern D64 __bid64_from_uint64 (uint64_t);
extern D64 __bid64_from_int32 (int32_t);
extern D64 __bid64_from_int64 (int64_t);
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
extern int __bid64_quiet_equal (D64 x, D64 y, ErrorFlags *pfpsf);
extern int __bid64_quiet_less (D64 x, D64 y, ErrorFlags *pfpsf);
}

// Base class for decimal types
template <class T>
class Decimal {
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

protected:
	T val; // either D64 or D128

	RoundMode _round_mode = Round::NearestEven;
	ErrorFlags _errors = Error::None;
	ErrorFlags _throw = Error::None;
	
	// throws an exception if an invalid operation occurred	
	void check_flags(const ErrorFlags flags) const { 
		if ((flags & Error::Invalid) == Error::Invalid) {
			throw Decimal::InvalidException("Invalid decimal operation", flags, *this);
		} else if ((flags & Error::DivideByZero) == Error::DivideByZero && 
			(this->_throw & Error::DivideByZero) == Error::DivideByZero) {
			throw Decimal::DivideByZeroException("Attempt to divide by zero", flags, *this);
		} else if ((flags & Error::Overflow) == Error::Overflow && 
			(this->_throw & Error::Overflow) == Error::Overflow) {
			throw Decimal::OverflowException("Overflow", flags, *this);
		} else if ((flags & Error::Underflow) == Error::Underflow && 
			(this->_throw & Error::Underflow) == Error::Underflow) {
			throw Decimal::UnderflowException("Underflow", flags, *this);
		} else if ((flags & Error::Inexact) == Error::Inexact && 
			(this->_throw & Error::Inexact) == Error::Inexact) {
			throw Decimal::InexactException("Inexact", flags, *this);
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
	void check_rounding(const RoundMode against) {
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
	std::function<int(T)> is_signed;
	std::function<int(T, T, ErrorFlags*)> quiet_equal;
	std::function<int(T, T, ErrorFlags*)> quiet_less;
	std::function<T(T)> abs;
	std::function<T(T)> negate;
	std::function<T(T, T, RoundMode, ErrorFlags*)> add;
	std::function<T(T, T, RoundMode, ErrorFlags*)> sub;
	std::function<T(T, T, RoundMode, ErrorFlags*)> mul;
	std::function<T(T, T, RoundMode, ErrorFlags*)> div;
	
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
		~cstr() {
			delete[] this->val;
		}
	};

	// used by derived classes to set the rounding mode
	Decimal(const RoundMode & round_mode = Round::NearestEven) : _round_mode(round_mode) {}
	
public:	 
	struct Exception : public std::runtime_error {
		const ErrorFlags flags;
		const Decimal decimal;

		Exception(const std::string & message, ErrorFlags flags, const Decimal & decimal) 
			: std::runtime_error(message), flags(flags), decimal(decimal) {}
		Exception(const char * message, ErrorFlags flags, const Decimal & decimal) 
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
	
	void throw_on(const Error & error) { this->_throw |= error; }
	void throw_off(const Error & error) { this->_throw ^= error; }

	const unsigned int errors() const { return this->_errors; }
	const bool divide_by_zero() const { return ((this->_errors & Error::DivideByZero) == Error::DivideByZero); }
	const bool overflow() const { return ((this->_errors & Error::Overflow)== Error::Overflow); }
	const bool underflow() const { return ((this->_errors & Error::Underflow) == Error::Underflow); }
	const bool inexact() const { return ((this->_errors & Error::Inexact) == Error::Inexact); }

	// TODO: ADDITIONAL PROPERTIES
	// SUBNORMAL
	// PRECISION
	// EMAX / EMIN

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

	// true if this decimal < 0	
	const bool is_negative() const {
		return (this->is_signed(this->val) > 0);
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

	// == arithmetic operators == //	
	void inline operator+=(const Decimal<T> & other) {
		check_rounding(other._round_mode);
		this->val = this->invoke<D128>([&](ErrorFlags * flags) {
			return this->add(this->val, other.val, this->_round_mode, flags);
		});
	}
	
	void inline operator-=(const Decimal<T> & other) {
		check_rounding(other._round_mode);
		this->val = this->invoke<D128>([&](ErrorFlags * flags) {
			return this->sub(this->val, other.val, this->_round_mode, flags);
		});
	}
	
	void inline operator*=(const Decimal<T> & other) {
		check_rounding(other._round_mode);
		this->val = this->invoke<D128>([&](ErrorFlags * flags) {
			return this->mul(this->val, other.val, this->_round_mode, flags);
		});
	}
	
	void inline operator/=(const Decimal<T> & other) {
		check_rounding(other._round_mode);
		this->val = this->invoke<D128>([&](ErrorFlags * flags) {
			return this->div(this->val, other.val, this->_round_mode, flags);
		});
	}

	// == comparison operators == //	
	friend inline bool operator!=(const Decimal & l, const Decimal & r) { return !(l == r); }
	friend inline bool operator==(const Decimal & l, const Decimal & r) { 
		ErrorFlags flags = Error::None;
		auto result = l.quiet_equal (l.val, r.val, &flags);
		return result > 0;
	

		//return (l.invoke<int>([&](ErrorFlags * flags) {
			//return l.quiet_equal(l.val, r.val, flags);
		//}) > 0);
	}

	friend inline bool operator>(const Decimal & l, const Decimal & r) { return r < l || l == r; }
	friend inline bool operator<=(const Decimal & l, const Decimal & r) { return l < r || l == r; }
	friend inline bool operator>=(const Decimal & l, const Decimal & r) { return l > r || l == r; }
	friend inline bool operator<(const Decimal & l, const Decimal & r) {
		ErrorFlags flags = Error::None;
		auto result = l.quiet_less (l.val, r.val, &flags);
		return result > 0;
	}
		
	friend inline std::ostream& operator<<(std::ostream& stream, const Decimal & decimal) {
		stream << decimal.str();
		return stream;
	}
};


class Decimal128 : public Decimal<D128> {
public:
	Decimal128() : Decimal128(static_cast<int>(0)) {}
	
	Decimal128(const unsigned char value, const RoundMode round_mode = Round::NearestEven) 
		: Decimal128((unsigned int) value, round_mode) {}
	Decimal128(const unsigned short value, const RoundMode round_mode = Round::NearestEven) 
		: Decimal128((unsigned int) value, round_mode) {}
	Decimal128(const unsigned int value, const RoundMode round_mode = Round::NearestEven) 
		: Decimal(round_mode) { 
			this->attach_bid_functions();
			this->val = this->from_uint32(value); 
	}
	Decimal128(const unsigned long value, const RoundMode round_mode = Round::NearestEven) 
		: Decimal128((unsigned long long) value, round_mode) {}
	Decimal128(const unsigned long long value, const RoundMode round_mode = Round::NearestEven) 
		: Decimal(round_mode) { 
			this->attach_bid_functions();
			this->val = this->from_uint64(value); 
	}
	
	Decimal128(const char value, const RoundMode round_mode = Round::NearestEven) : Decimal128((int) value, round_mode) {}
	Decimal128(const short value, const RoundMode round_mode = Round::NearestEven) : Decimal128((int) value, round_mode) {}
	Decimal128(const int value, const RoundMode round_mode = Round::NearestEven) : Decimal(round_mode) {
		this->attach_bid_functions();
		this->val = this->from_int32(value); 
	}
	Decimal128(const long value, const RoundMode round_mode = Round::NearestEven) : Decimal128((long long) value, round_mode) {}
	Decimal128(const long long value, const RoundMode round_mode = Round::NearestEven) : Decimal128(round_mode) { 
		this->attach_bid_functions();
		this->val = this->from_int64(value); 
	}			
	
	Decimal128(const std::string & value, const RoundMode round_mode = Round::NearestEven) 
		: Decimal(round_mode) {
		this->attach_bid_functions();
		auto val = value.empty()? "0" : value;
		cstr c(val);
		this->val = this->invoke<D128>([&](ErrorFlags * flags) {
			return this->from_string(c.val, this->_round_mode, flags);
		});
	}

	void attach_bid_functions() {
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
		this->is_signed = bid128_isSigned;
		this->quiet_equal = bid128_quiet_equal;
		this->quiet_less = bid128_quiet_less;
		this->abs = bid128_abs;
		this->negate = bid128_negate;
		this->add = bid128_add;
		this->sub = bid128_sub;
		this->mul = bid128_mul;
		this->div = bid128_div;

		this->val = this->from_uint64(0);
	}
	
	friend inline Decimal128 abs(const Decimal128 & v) {
		Decimal128 v2 = v;
		v2.val = v.abs(v2.val);
		return v2;
	}	
	
	friend inline Decimal128 operator-(const Decimal128 & v) {
		Decimal128 v2 = v;
		v2.val = v.negate(v2.val);
		return v2;
	}	
	
	friend inline Decimal128 operator+(const Decimal128 & l, const Decimal128 & r) {
		Decimal128 tmp = l;
		tmp += r;
		return tmp;
	}	
	
	friend inline Decimal128 operator-(const Decimal128 & l, const Decimal128 & r) {
		Decimal128 tmp = l;
		tmp -= r;
		return tmp;
	}	
	
	friend inline Decimal128 operator*(const Decimal128 & l, const Decimal128 & r) {
		Decimal128 tmp = l;
		tmp *= r;
		return tmp;
	}	
	
	friend inline Decimal128 operator/(const Decimal128 & l, const Decimal128 & r) {
		Decimal128 tmp = l;
		tmp /= r;
		return tmp;
	}	
};

namespace decimal128 {
static Decimal128 Zero(0);
static Decimal128 One(1);
static Decimal128 Max(std::string("9999999999999999999999999999999999") + "E+6111");
static Decimal128 Min(std::string("-9999999999999999999999999999999999") + "E+6111");
static Decimal128 SmallestPositive(std::string("9999999999999999999999999999999999") + "E-6176");
static Decimal128 SmallestNegative(std::string("-9999999999999999999999999999999999") + "E-6176");
static Decimal128 Inf("Inf");
}


class Decimal64 : public Decimal<D64> {
public:		
	Decimal64() : Decimal() {
		this->from_string = bid64_from_string;
		this->from_uint32 = bid64_from_uint32;
		this->from_uint64 = bid64_from_uint64;
		this->from_int32 = bid64_from_int32;
		this->from_int64 = bid64_from_int64;
		this->to_string = bid64_to_string;
		this->to_uint8_xrnint = bid64_to_uint8_xrnint;
		this->to_uint16_xrnint = bid64_to_uint16_xrnint;
		this->to_uint32_xrnint = bid64_to_uint32_xrnint;
		this->to_uint64_xrnint = bid64_to_uint64_xrnint;
		this->to_int8_xrnint = bid64_to_int8_xrnint;
		this->to_int16_xrnint = bid64_to_int16_xrnint;
		this->to_int32_xrnint = bid64_to_int32_xrnint;
		this->to_int64_xrnint = bid64_to_int64_xrnint;
		this->is_signed = bid64_isSigned;
		this->quiet_equal = bid64_quiet_equal;
		this->quiet_less = bid64_quiet_less;
	}
	
	Decimal64(const std::string & value) : Decimal64() {}
	Decimal64(const unsigned char value) : Decimal64() {}
	Decimal64(const unsigned short value) : Decimal64() {}
	Decimal64(const unsigned int value) : Decimal64() {}
	Decimal64(const unsigned long value) : Decimal64() {}
	Decimal64(const unsigned long long value) : Decimal64() {}
	Decimal64(const char value) : Decimal64() {}
	Decimal64(const short value) : Decimal64() {}
	Decimal64(const int value) : Decimal64() {}
	Decimal64(const long value) : Decimal64() {}
	Decimal64(const long long value) : Decimal64() {}
};

class DecimalFactory {
public:
	/*
	IDecimal random() {
		// Algorithm derived from IEEE754-2008, Page 8
		// http://www.dsc.ufcg.edu.br/~cnum/modulos/Modulo2/IEEE754_2008.pdf
		// Signed zero and non-zero floating-point numbers of the form (−1)^s*b^q*c, where
		// 	s is 0 or 1.
		// 	q is any integer emin <= q + p − 1 <= emax.
		// 	c is a number represented by a digit string of the form
		//	d0 d1 d2 ... dp−1 where di is an integer digit 0 <= di < b (c is therefore an integer with 0 <= c < b^p).
		//
		//	For Decimal128:
		//	p = 34
		//	emax = 6144
		//	emin = -6143
		//
		//	For Decimal64:
		//	p = 16
		//	emax = 384
		//	emin = -383

		// setup a random number generator,
		// make it static so we don't have to rebuild it with every call
		static std::random_device rd;
		static std::mt19937_64 gen(rd());

		// use a bernoulli probability distribution to decide whether the number will be negative
		static auto sign_dist = std::bernoulli_distribution(0.5);
		bool negative = sign_dist(gen);

		// use a uniform integer distribution to generate the digits of the significand
		// create two 64-bit integers, and use modulus to extract the first p / 2 bits of each
		static auto significand_dist = std::uniform_int_distribution<uint64_t>(0, std::numeric_limits<uint64_t>::max());
		uint64_t significand_low = significand_dist(gen) % 100000000000000000;
		uint64_t significand_high = significand_dist(gen) % 100000000000000000;

		// use a uniform integer distribution to generate the exponent in the range [emin - p + 1, emax - p + 1]
		static auto exponent_dist = std::uniform_int_distribution<short>(-6176, 6111);
		int exponent = exponent_dist(gen);

		// create a string with the following format:
		// "sDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDE+DDDD" + \0
		auto n = 1 + 34 + 2 + 4 + 1;
		char result[n];
		sprintf(result, "%c%llu%lluE%+d", (negative? '-' : '+'), significand_high, significand_low, exponent);

		std::cout << "Random number: " << result << std::endl;

		// use the string to create a decimal
		return Decimal128(); // TODO:
		//return Decimal128(result);
		The maximum number of decimal digits in the significand of numerical
    values represented in these three formats are:
      P = 7 decimal digits for the 32-bit decimal floating-point format
      P = 16 decimal digits for the 64-bit decimal floating-point format
      P = 34 decimal digits for the 128-bit decimal floating-point format
    The ranges for normal decimal floating-point numbers are (in magnitude):
      1.000000 * 10^(-95) <= x <= 9.999999 * 10^96 for 32-bit format
      1.0...0 * 10^(-383) <= x <= 9.9...9 * 10^384 for 64-bit format
            (15 decimal digits in the fractional part of the significand)
      1.0...0 * 10^(-6143) <= x <= 9.9...9 * 10^6144 for 128-bit format
            (33 decimal digits in the fractional part of the significand)
    The ranges for subnormal decimal floating-point numbers are (in magnitude):
      0.000001 * 10^(-95) <= x <= 0.999999 * 10^(-95) for 32-bit format
      0.0...01 * 10^(-383) <= x <= 0.9...9 * 10^(-383) for 64-bit format
            (15 decimal digits in the fractional part of the significand)
      0.0...01 * 10^(-6144) <= x <= 0.9...9 * 10^(-6144) for 128-bit format
            (33 decimal digits in the fractional part of the significand)
	}
	*/
};	
} // namespace decimal754

#endif // DECIMAL_H
