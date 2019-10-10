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

extern "C" {
#include <bid_conf.h>

typedef uint64_t D64;
typedef struct __attribute__((aligned(16))) { unsigned long long w[2]; } D128;

/* rounding modes */
typedef enum _IDEC_roundingmode {
    _IDEC_nearesteven = 0,
    _IDEC_downward    = 1,
    _IDEC_upward      = 2,
    _IDEC_towardzero  = 3,
    _IDEC_nearestaway = 4,
    _IDEC_dflround    = _IDEC_nearesteven
} _IDEC_roundingmode;
typedef unsigned int _IDEC_round;

/* exception flags */
typedef enum _IDEC_flagbits {
    _IDEC_invalid       = 0x01,
    _IDEC_zerodivide    = 0x04,
    _IDEC_overflow      = 0x08,
    _IDEC_underflow     = 0x10,
    _IDEC_inexact       = 0x20,
    _IDEC_allflagsclear = 0x00
} _IDEC_flagbits;
typedef unsigned int _IDEC_flags;  // could be a struct with diagnostic info

/* 128-bit functions */
extern D128 __bid128_from_string (char *ps, _IDEC_round rnd_mode, _IDEC_flags *pfpsf);
extern D128 __bid128_from_uint32 (uint32_t);
extern D128 __bid128_from_uint64 (uint64_t);
extern D128 __bid128_from_int32 (int32_t);
extern D128 __bid128_from_int64 (int64_t);
extern D128 __bid128_abs (D128 x);
extern D128 __bid128_add ( D128, D128, _IDEC_round, _IDEC_flags *);
extern D128 __bid128_sub ( D128, D128, _IDEC_round, _IDEC_flags *);
extern D128 __bid128_mul ( D128, D128, _IDEC_round, _IDEC_flags *);
extern D128 __bid128_div ( D128, D128, _IDEC_round, _IDEC_flags *);
extern void __bid128_to_string ( char *ps, D128 x, _IDEC_flags *pfpsf);
extern uint8_t __bid128_to_uint8_xrnint (D128 x, _IDEC_flags *pfpsf);
extern uint16_t __bid128_to_uint16_xrnint (D128 x, _IDEC_flags *pfpsf);
extern uint32_t __bid128_to_uint32_xrnint (D128 x, _IDEC_flags *pfpsf);
extern uint64_t __bid128_to_uint64_xrnint (D128 x, _IDEC_flags *pfpsf);
extern int8_t __bid128_to_int8_xrnint (D128 x, _IDEC_flags *pfpsf);
extern int16_t __bid128_to_int16_xrnint (D128 x, _IDEC_flags *pfpsf);
extern int32_t __bid128_to_int32_xrnint (D128 x, _IDEC_flags *pfpsf);
extern int64_t __bid128_to_int64_xrnint (D128 x, _IDEC_flags *pfpsf);
extern int __bid128_quiet_equal (D128 x, D128 y, _IDEC_flags *pfpsf);
extern int __bid128_quiet_less (D128 x, D128 y, _IDEC_flags *pfpsf); // TODO: SIGNALLING?

/* 64-bit functions */
extern D64 __bid64_from_string (char *ps, _IDEC_round rnd_mode, _IDEC_flags *pfpsf);
extern D64 __bid64_from_uint32 (uint32_t);
extern D64 __bid64_from_uint64 (uint64_t);
extern D64 __bid64_from_int32 (int32_t);
extern D64 __bid64_from_int64 (int64_t);
extern D64 __bid64_abs (D64 x);
extern D64 __bid64_add ( D64, D64, _IDEC_round, _IDEC_flags *);
extern D64 __bid64_sub ( D64, D64, _IDEC_round, _IDEC_flags *);
extern D64 __bid64_mul ( D64, D64, _IDEC_round, _IDEC_flags *);
extern D64 __bid64_div ( D64, D64, _IDEC_round, _IDEC_flags *);
extern void __bid64_to_string ( char *ps, D64 x, _IDEC_flags *pfpsf);
extern uint8_t __bid64_to_uint8_xrnint (D64 x, _IDEC_flags *pfpsf);
extern uint16_t __bid64_to_uint16_xrnint (D64 x, _IDEC_flags *pfpsf);
extern uint32_t __bid64_to_uint32_xrnint (D64 x, _IDEC_flags *pfpsf);
extern uint64_t __bid64_to_uint64_xrnint (D64 x, _IDEC_flags *pfpsf);
extern int8_t __bid64_to_int8_xrnint (D64 x, _IDEC_flags *pfpsf);
extern int16_t __bid64_to_int16_xrnint (D64 x, _IDEC_flags *pfpsf);
extern int32_t __bid64_to_int32_xrnint (D64 x, _IDEC_flags *pfpsf);
extern int64_t __bid64_to_int64_xrnint (D64 x, _IDEC_flags *pfpsf);
extern int __bid64_quiet_equal (D64 x, D64 y, _IDEC_flags *pfpsf);
extern int __bid64_quiet_less (D64 x, D64 y, _IDEC_flags *pfpsf);
}

namespace decimal754 {
	
struct DecimalException : public std::runtime_error {
	_IDEC_flags flags;

	DecimalException(const std::string & message, _IDEC_flags flags) : std::runtime_error(message), flags(flags) {}
	DecimalException(const char * message, _IDEC_flags flags) : std::runtime_error(message), flags(flags) {}
};

struct UnsupportedDecimal : public virtual std::runtime_error {
	UnsupportedDecimal() : std::runtime_error("Unsupported decimal type") {}
};

class IDecimal {};

template <class T>
class Decimal : public IDecimal {
protected:
	T val;

	enum Error: unsigned int {
		None			= 0x00,
		Invalid			= 0x01,
		DivideByZero	= 0x04,
		Overflow		= 0x08,
		Underflow		= 0x10,
		Inexact			= 0x20
	};
	
	unsigned int errors = Error::None;
	
	void check_flags(const _IDEC_flags flags) {
		this->errors |= flags;
		if ((flags & Error::Invalid) == Error::Invalid) {
			throw DecimalException("Invalid decimal operation", flags);
		}
	}

	std::function<T(const uint32_t)> from_uint32;
	std::function<T(const uint64_t)> from_uint64;
	std::function<T(char*, _IDEC_round, _IDEC_flags*)> from_string;
	std::function<T(int32_t)> from_int32;
	std::function<T(int64_t)> from_int64;
	std::function<void(char*, T, _IDEC_flags*)> to_string;
	std::function<uint8_t(T, _IDEC_flags*)> to_uint8_xrnint;
	std::function<uint16_t(T, _IDEC_flags*)> to_uint16_xrnint;
	std::function<uint32_t(T, _IDEC_flags*)> to_uint32_xrnint;
	std::function<uint64_t(T, _IDEC_flags*)> to_uint64_xrnint;
	std::function<int8_t(T, _IDEC_flags*)> to_int8_xrnint;
	std::function<int16_t(T, _IDEC_flags*)> to_int16_xrnint;
	std::function<int32_t(T, _IDEC_flags*)> to_int32_xrnint;
	std::function<int64_t(T, _IDEC_flags*)> to_int64_xrnint;
	std::function<int(T, T, _IDEC_flags*)> quiet_equal;
	std::function<int(T, T, _IDEC_flags*)> quiet_less;
	std::function<T(T)> abs;
	std::function<T(T, T, _IDEC_round, _IDEC_flags*)> add;
	std::function<T(T, T, _IDEC_round, _IDEC_flags*)> sub;
	std::function<T(T, T, _IDEC_round, _IDEC_flags*)> mul;
	std::function<T(T, T, _IDEC_round, _IDEC_flags*)> div;
	
	template <class TResult>
	TResult invoke(std::function<TResult(_IDEC_flags*)> func) {
		_IDEC_flags flags = _IDEC_allflagsclear;
		TResult result = func(&flags);
		check_flags(flags);
		return result;
	}
	
public:	 
	const std::string error_str() const {
		std::stringstream ss;
		std::vector<std::string> v;

		if ((this->errors & Error::Invalid) == Error::Invalid) {
			v.push_back("invalid");
		} else if ((this->errors & Error::DivideByZero) == DivideByZero) {
			v.push_back("zerodivide");
		} else if ((this->errors & Error::Overflow) == Error::Overflow) {
			v.push_back("overflow");
		} else if ((this->errors & Error::Underflow) == Error::Underflow) {
			v.push_back("underflow");
		} else if ((this->errors & Error::Inexact) == Error::Inexact) {
			v.push_back("inexact");
		}

		if (v.size() > 0) {
			ss << "| ";
			for(size_t i = 0; i < v.size(); ++i) {
				ss << v[i];
				if (i < v.size() - 1) {
					ss << " | ";
				}
			}
			ss << " |";
		}
			
		return ss.str();
	}

	explicit operator unsigned char() const { 
		return this->invoke([&](_IDEC_flags * flags) {
			return this->to_uint8_xrnint (this->val, flags);
		});
	}
	
	explicit operator unsigned short() const { 
		return this->invoke([&](_IDEC_flags * flags) {
			return this->to_uint16_xrnint (this->val, flags);
		});
	}

	explicit operator unsigned int() const { 
		return this->invoke([&](_IDEC_flags * flags) {
			return this->to_uint32_xrnint (this->val, &flags);
		});
	}
	
	explicit operator unsigned long() const { 
		return this->invoke([&](_IDEC_flags * flags) {
			return this->to_uint64_xrnint (this->val, &flags);
		});
	}
	
	explicit operator unsigned long long() const { 
		return this->invoke([&](_IDEC_flags * flags) {
			return this->to_uint64_xrnint (this->val, &flags);
		});
	}
	
	explicit operator char() const { 
		return this->invoke([&](_IDEC_flags * flags) {
			return this->to_int8_xrnint (this->val, &flags);
		});
	}
	
	explicit operator short() const { 
		_IDEC_flags flags = _IDEC_allflagsclear;
		unsigned int ans = this->to_int16_xrnint (this->val, &flags);
		if (flags != _IDEC_allflagsclear) {
			throw DecimalException("Failed to convert decimal to short", flags);
		}

		return ans;
	}

	explicit operator int() const { 
		_IDEC_flags flags = _IDEC_allflagsclear;
		unsigned int ans = this->to_int32_xrnint (this->val, &flags);
		if (flags != _IDEC_allflagsclear) {
			throw DecimalException("Failed to convert decimal to int", flags);
		}

		return ans;
	}
	
	explicit operator long() const { 
		_IDEC_flags flags = _IDEC_allflagsclear;
		unsigned long ans = this->to_int64_xrnint (this->val, &flags);
		if (flags != _IDEC_allflagsclear) {
			throw DecimalException("Failed to convert decimal to long", flags);
		}

		return ans;
	}
	
	explicit operator long long() const { 
		// TODO - CONSOLIDATE THESE
		_IDEC_flags flags = _IDEC_allflagsclear;
		unsigned long ans = this->to_int64_xrnint (this->val, &flags);
		if (flags != _IDEC_allflagsclear) {
			throw DecimalException("Failed to convert decimal to long long", flags);
		}

		return ans;
	}

	const std::string str() const {
		_IDEC_flags flags = _IDEC_allflagsclear;
		char buffer[64];
		buffer[63] = '\0';
		this->to_string (buffer, this->val, &flags);
		std::string s = buffer;
		if (flags != _IDEC_allflagsclear) {
			throw DecimalException("Failed to convert decimal to int64_t while converting to str: " + s, flags);
		}

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
	
	const bool negative() const {
		return *this < Decimal(0); // TODO: use library
	}
	
	friend Decimal abs(const Decimal & v) {
		Decimal v2 = v;
		v2.val = T::abs(v.val);
		return v2;
	}	
	
	void operator+=(const Decimal<T> & other) {
		_IDEC_flags flags = _IDEC_allflagsclear;
		auto result = this->add(this->val, other.val, _IDEC_dflround, &flags);
		if (flags != _IDEC_allflagsclear) {
			throw DecimalException("Failed to add decimals", flags);
		}

		this->val = result;
	}
	
	void operator-=(const Decimal<T> & other) {
		_IDEC_flags flags = _IDEC_allflagsclear;
		auto result = this->sub(this->val, other.val, _IDEC_dflround, &flags);
		if (flags != _IDEC_allflagsclear) {
			throw DecimalException("Failed to subtract decimals", flags);
		}

		this->val = result;
	}
	
	void operator*=(const Decimal<T> & other) {
		_IDEC_flags flags = _IDEC_allflagsclear;
		auto result = this->mul(this->val, other.val, _IDEC_dflround, &flags);
		check_flags(flags);
		this->val = result;
	}
	
	void operator/=(const Decimal<T> & other) {
		_IDEC_flags flags = _IDEC_allflagsclear;
		auto result = this->div(this->val, other.val, _IDEC_dflround, &flags);
		check_flags(flags);
		this->val = result;
	}
};

class Decimal128 : public Decimal<D128> {
public:
	Decimal128() {
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
		this->quiet_equal = bid128_quiet_equal;
		this->quiet_less = bid128_quiet_less;
		this->abs = bid128_abs;
		this->add = bid128_add;
		this->sub = bid128_sub;
		this->mul = bid128_mul;
		this->div = bid128_div;

		this->val = this->from_uint64(0);
	}

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
	
	Decimal128(const std::string & value) : Decimal128() {
		auto val = value.empty()? "0" : value;
		cstr c(val);
		this->val = this->invoke<D128>([&](_IDEC_flags * flags) {
			return this->from_string(c.val, _IDEC_dflround, flags);
		});
	}
	
	Decimal128(const unsigned char value) : Decimal128((unsigned int) value) {}
	Decimal128(const unsigned short value) : Decimal128((unsigned int) value) {}
	Decimal128(const unsigned int value) : Decimal128() { this->val = this->from_uint32(value); }
	Decimal128(const unsigned long value) : Decimal128((unsigned long long) value) {}
	Decimal128(const unsigned long long value) : Decimal128() { this->val = this->from_uint64(value); }
	
	Decimal128(const char value) : Decimal128((int) value) {}
	Decimal128(const short value) : Decimal128((int) value) {}
	Decimal128(const int value) : Decimal128() { this->val = this->from_int32(value); }
	Decimal128(const long value) : Decimal128((long long) value) {}
	Decimal128(const long long value) : Decimal128() { this->val = this->from_int64(value); }
	
	friend bool operator!=(const Decimal128 & l, const Decimal128 & r) { return !(l == r); }
	friend bool operator==(const Decimal128 & l, const Decimal128 & r) { 
		_IDEC_flags flags = _IDEC_allflagsclear;
		auto result = l.quiet_equal (l.val, r.val, &flags);
		return result > 0;
	}

	friend bool operator>(const Decimal128 & l, const Decimal128 & r) { return r < l || l == r; }
	friend bool operator<=(const Decimal128 & l, const Decimal128 & r) { return l < r || l == r; }
	friend bool operator>=(const Decimal128 & l, const Decimal128 & r) { return l > r || l == r; }
	friend bool operator<(const Decimal128 & l, const Decimal128 & r) {
		_IDEC_flags flags = _IDEC_allflagsclear;
		auto result = l.quiet_less (l.val, r.val, &flags);
		return result > 0;
	}
	
	friend Decimal128 operator+(const Decimal128 & l, const Decimal128 & r) {
		Decimal128 tmp = l;
		tmp += r;
		return tmp;
	}	
	
	friend Decimal128 operator-(const Decimal128 & l, const Decimal128 & r) {
		Decimal128 tmp = l;
		tmp -= r;
		return tmp;
	}	
	
	friend Decimal128 operator*(const Decimal128 & l, const Decimal128 & r) {
		Decimal128 tmp = l;
		tmp *= r;
		return tmp;
	}	
	
	friend Decimal128 operator/(const Decimal128 & l, const Decimal128 & r) {
		Decimal128 tmp = l;
		tmp /= r;
		return tmp;
	}	

	friend std::ostream& operator<<(std::ostream& stream, const Decimal128 & decimal) {
		stream << decimal.str();
		return stream;
	}
};

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

   /* 
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
			*/
	}
};
	
} // namespace markets

#endif // DECIMAL_H
