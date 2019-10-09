/*
 *  decimal754.h
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
extern D128 __bid128_mul ( D128, D128, _IDEC_round, _IDEC_flags *);
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
extern int __bid128_signaling_less (D128 x, D128 y, _IDEC_flags *pfpsf);

/* 64-bit functions */
extern D64 __bid64_from_string (char *ps, _IDEC_round rnd_mode, _IDEC_flags *pfpsf);
extern D64 __bid64_from_uint32 (uint32_t);
extern D64 __bid64_from_uint64 (uint64_t);
extern D64 __bid64_from_int32 (int32_t);
extern D64 __bid64_from_int64 (int64_t);
extern D64 __bid64_abs (D64 x);
extern D64 __bid64_mul ( D64, D64, _IDEC_round, _IDEC_flags *);
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
extern int __bid64_signaling_less (D64 x, D64 y, _IDEC_flags *pfpsf);
//extern _IDEC_flags __bid_testFlags (_IDEC_flags flagsmask, _IDEC_flags *pfpsf);
}

namespace decimal754 {

class IDecimal {};

template <class T>
class Decimal : public IDecimal {
protected:
	T val; // TODO: DEFAULT TO ZERO

	std::function<T(const uint32_t)> from_uint32;
	std::function<T(const uint64_t)> from_uint64;
	std::function<T(char*, _IDEC_round, _IDEC_flags*)> from_string;

public:
	struct Exception : public virtual std::runtime_error {
		_IDEC_flags flags;
		Exception(const std::string & message, _IDEC_flags flags) 
			: std::runtime_error(message), flags(flags) {}
		const std::string flags_str() const {
			std::string s = "Flags: | ";
			if ((_IDEC_invalid & this->flags) == _IDEC_invalid) {
				s += "invalid |";
			} else if ((_IDEC_zerodivide & this->flags) == _IDEC_invalid) {
				s += " zerodivide |";
			} else if ((_IDEC_overflow & this->flags) == _IDEC_overflow) {
				s += " overflow |";
			} else if ((_IDEC_underflow & this->flags) == _IDEC_underflow) {
				s += " underflow |";
			} else if ((_IDEC_inexact & this->flags) == _IDEC_inexact) {
				s += " inexact |";
			}

			return s;
		}
	};

	struct Unsupported : public virtual std::runtime_error {
		Unsupported() : std::runtime_error("Unsupported decimal type") {}
	};

	Decimal() {
		if constexpr(std::is_same<T, D128>::value) {
			this->from_uint32 = bid128_from_uint32;
			this->from_uint64 = bid128_from_uint64;
			this->from_string = bid128_from_string;
		} else if constexpr(std::is_same<T, D128>::value) {
			this->from_uint32 = bid64_from_uint32;
			this->from_uint64 = bid64_from_uint64;
			this->from_string = bid64_from_string;
		} else { throw Unsupported(); }

		this->val = this->from_uint64(0);
	}

	Decimal(const std::string & value) : Decimal() {
		auto val = value.empty()? "0" : value;
		char *cstr = new char[val.length() + 1];
		strcpy(cstr, val.c_str());
		_IDEC_flags flags = _IDEC_allflagsclear;
		this->val = this->from_string(cstr, _IDEC_dflround, &flags);
		delete[] cstr;

		if (flags != _IDEC_allflagsclear) {
			throw Exception("Failed to construct decimal from string", flags);
		}
	}
	
	Decimal(const unsigned char value) : Decimal((unsigned int) value) {}
	Decimal(const unsigned short value) : Decimal((unsigned int) value) {}
	Decimal(const unsigned int value) : Decimal() { this->val = this->from_uint32(value); }
	Decimal(const unsigned long value) : Decimal((unsigned long long) value) {}
	Decimal(const unsigned long long value) : Decimal() { this->val = this->from_uint64(value); }
	
	Decimal(const char value) : Decimal((int) value) {}
	Decimal(const short value) : Decimal((int) value) {}
	Decimal(const int value) : Decimal() { this->val = bid128_from_int32(value); }
	Decimal(const long value) : Decimal((long long) value) {}
	Decimal(const long long value) : Decimal() { this->val = bid128_from_int64(value); }
	
	explicit operator unsigned char() const { 
		_IDEC_flags flags = _IDEC_allflagsclear;
		unsigned int ans = bid128_to_uint8_xrnint (this->val, &flags);
		if (flags != _IDEC_allflagsclear) {
			throw Exception("Failed to convert decimal to unsigned char", flags);
		}

		return ans;
	}
	
	explicit operator unsigned short() const { 
		_IDEC_flags flags = _IDEC_allflagsclear;
		unsigned int ans = bid128_to_uint16_xrnint (this->val, &flags);
		if (flags != _IDEC_allflagsclear) {
			throw Exception("Failed to convert decimal to unsigned short", flags);
		}

		return ans;
	}

	explicit operator unsigned int() const { 
		_IDEC_flags flags = _IDEC_allflagsclear;
		unsigned int ans = bid128_to_uint32_xrnint (this->val, &flags);
		if (flags != _IDEC_allflagsclear) {
			throw Exception("Failed to convert decimal to unsigned int", flags);
		}

		return ans;
	}
	
	explicit operator unsigned long() const { 
		_IDEC_flags flags = _IDEC_allflagsclear;
		unsigned long ans = bid128_to_uint64_xrnint (this->val, &flags);
		if (flags != _IDEC_allflagsclear) {
			throw Exception("Failed to convert decimal to unsigned long", flags);
		}

		return ans;
	}
	
	explicit operator unsigned long long() const { 
		// TODO - CONSOLIDATE THESE
		_IDEC_flags flags = _IDEC_allflagsclear;
		unsigned long ans = bid128_to_uint64_xrnint (this->val, &flags);
		if (flags != _IDEC_allflagsclear) {
			throw Exception("Failed to convert decimal to unsigned long long", flags);
		}

		return ans;
	}
	
	explicit operator char() const { 
		_IDEC_flags flags = _IDEC_allflagsclear;
		unsigned int ans = bid128_to_int8_xrnint (this->val, &flags);
		if (flags != _IDEC_allflagsclear) {
			throw Exception("Failed to convert decimal to char", flags);
		}

		return ans;
	}
	
	explicit operator short() const { 
		_IDEC_flags flags = _IDEC_allflagsclear;
		unsigned int ans = bid128_to_int16_xrnint (this->val, &flags);
		if (flags != _IDEC_allflagsclear) {
			throw Exception("Failed to convert decimal to short", flags);
		}

		return ans;
	}

	explicit operator int() const { 
		_IDEC_flags flags = _IDEC_allflagsclear;
		unsigned int ans = bid128_to_int32_xrnint (this->val, &flags);
		if (flags != _IDEC_allflagsclear) {
			throw Exception("Failed to convert decimal to int", flags);
		}

		return ans;
	}
	
	explicit operator long() const { 
		_IDEC_flags flags = _IDEC_allflagsclear;
		unsigned long ans = bid128_to_int64_xrnint (this->val, &flags);
		if (flags != _IDEC_allflagsclear) {
			throw Exception("Failed to convert decimal to long", flags);
		}

		return ans;
	}
	
	explicit operator long long() const { 
		// TODO - CONSOLIDATE THESE
		_IDEC_flags flags = _IDEC_allflagsclear;
		unsigned long ans = bid128_to_int64_xrnint (this->val, &flags);
		if (flags != _IDEC_allflagsclear) {
			throw Exception("Failed to convert decimal to long long", flags);
		}

		return ans;
	}

	const std::string str() const {
		_IDEC_flags flags = _IDEC_allflagsclear;
		char buffer[64];
		buffer[63] = '\0';
		bid128_to_string (buffer, this->val, &flags);
		std::string s = buffer;
		if (flags != _IDEC_allflagsclear) {
			throw Exception("Failed to convert decimal to int64_t while converting to str: " + s, flags);
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
	
	/*	
		_IDEC_flags flags = _IDEC_allflagsclear;
		int64_t ans = bid128_to_int64_xrnint (this->val, &flags);
		auto s = std::to_string(ans);
		if (flags != _IDEC_allflagsclear) {
			throw Exception("Failed to convert decimal to int64_t while converting to str: " + s, flags);
		}
			
		return s;
		*/
	}
	
	const bool negative() const {
		return *this < Decimal(0);
		//assert(this->value.length() > 0);
		//return this->value[0] == '-';
	}

	/*
	const size_t point() const {
		auto pos = this->value.find('.');
		if (pos != std::string::npos) {
			return pos;
		} else {
			return this->value.length();
		}
	}
	*/

	friend Decimal abs(const Decimal & v) {
		Decimal v2 = v;
		v2.val = bid128_abs(v.val);
		return v2;
		/*
		if (v.negative()) {
			return Decimal(v.value.substr(1));
		} else {
			return v;
		}
		*/
	}
	
	friend bool operator==(const Decimal & l, const Decimal & r) { 
		_IDEC_flags flags = _IDEC_allflagsclear;
		auto result = __bid128_quiet_equal (l.val, r.val, &flags);
		if (flags != _IDEC_allflagsclear) {
			throw Exception("Failed to compare decimals for equality", flags);
		}

		return result > 0;
	}
	friend bool operator!=(const Decimal & l, const Decimal & r) { return !(l == r); }
	friend bool operator<(const Decimal & l, const Decimal & r) {
		_IDEC_flags flags = _IDEC_allflagsclear;
		auto result = __bid128_signaling_less (l.val, r.val, &flags);
		if (flags != _IDEC_allflagsclear) {
			throw Exception("Failed to order decimals", flags);
		}

		return result > 0;

		/*
		return l.val < r.val;
		if (l.negative() != r.negative()) {
			return l.negative();
		} else if (l.negative()) {
			return abs(r) < abs(l);
		} else if (l.point() != r.point()) { // TODO: EXPENSIVE
			return l.point() < r.point();
		} else {
			auto llen = l.value.length();
			auto rlen = r.value.length();
			auto index = 0;
			while (index < llen && index < rlen) {
				if (l.value[index] < r.value[index]) {
					return true;
				}
				++index;
			}

			return (llen < rlen);
		}
		*/
	}
	friend bool operator>(const Decimal & l, const Decimal & r) { return r < l || l == r; }
	friend bool operator<=(const Decimal & l, const Decimal & r) { return l < r || l == r; }
	friend bool operator>=(const Decimal & l, const Decimal & r) { return l > r || l == r; }
};

class Decimal128 : public Decimal<D128> {
public:
	using Decimal<D128>::Decimal;
};

class Decimal64 : public Decimal<uint64_t> {
public:	
	using Decimal<D64>::Decimal;
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
		return Decimal128(result);

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
	
} // namespace decimal754

#endif // DECIMAL_H
