/*
 *  tests.cpp
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

#include <iomanip>
#include <iostream>
#include <random>
#include "decimal.h"
#include "catch.hpp"

using namespace decimal754;
using namespace std;

LongDecimal ntwo("-2");
LongDecimal none("-1");
LongDecimal zero("0");
LongDecimal one("1");
LongDecimal two("2");
LongDecimal three("3");
LongDecimal four("4");
LongDecimal five("5");
LongDecimal six("6");
LongDecimal ten("10");

typedef LongDecimal d;
std::random_device rd;

const int LOOP_SIZE = 100;

template <class T>
static const vector<T> random_ld() {
	vector<T> v;
	for (int i=0; i < LOOP_SIZE; ++i) {
		v.push_back(T::random());
	}

	return v;
}

static const vector<LongDecimal> A() {
	static vector<LongDecimal> a;
	if (a.empty()) {
		a = random_ld<LongDecimal>();
	}

	return a;
}

static const vector<LongDecimal> B() {
	static vector<LongDecimal> b;
	if (b.empty()) {
		b = random_ld<LongDecimal>();
	}

	return b;
}

TEST_CASE( "Constants", "[constants]" ) {
	SECTION("Values") {
		REQUIRE( longDecimal::Zero == d(0) );
		REQUIRE( longDecimal::One == d(1) );
		REQUIRE( longDecimal::Max == d(std::string("9999999999999999999999999999999999") + "E+6111") );
		REQUIRE( longDecimal::Min == d(std::string("-9999999999999999999999999999999999") + "E+6111") );
		REQUIRE( longDecimal::SmallestPositive == d(std::string("9999999999999999999999999999999999") + "E-6176") );
		REQUIRE( longDecimal::SmallestNegative == d(std::string("-9999999999999999999999999999999999") + "E-6176") );
		REQUIRE( longDecimal::Inf == d("Inf") );
	}

	SECTION("Uniqueness") {
		vector<LongDecimal> v = { 
			longDecimal::Zero, 
			longDecimal::One, 
			longDecimal::Max, 
			longDecimal::Min, 
			longDecimal::SmallestPositive, 
			longDecimal::SmallestNegative, 
			longDecimal::Inf 
		};

		for (auto i = v.begin(); i != v.end(); i++) {
			for (auto j = v.begin(); j != v.end(); j++) {
				if (i != j) {
					REQUIRE( *i != *j );
				}
			}
		}
	}
}

// TODO: Constructor rounding mode

TEST_CASE( "Null constructor", "[null constructor]" ) {
	SECTION("Null Constructor") {
		REQUIRE_NOTHROW(d());
		auto a = d();
		REQUIRE( a.errors() == IDecimal::Error::None );
		REQUIRE( !a.is_negative() );
		REQUIRE( !a.is_normal() );
		REQUIRE( a.is_zero() );
		REQUIRE( static_cast<int>(a) == 0);
		REQUIRE( a.str() == "0");
	}
}

TEST_CASE( "Constructors: chars", "[constructors][char]" ) {
	
	SECTION("Constructor: unsigned char") {
		auto min = numeric_limits<unsigned char>::min();
		auto max = numeric_limits<unsigned char>::max();

		REQUIRE_NOTHROW(d(static_cast<unsigned char>('5')));
		REQUIRE_NOTHROW(d(static_cast<unsigned char>('a'))); // non-digits are specifically allowed

		for (int i=min; i < max; ++i) {
			auto a = d(static_cast<unsigned char>(i));
			REQUIRE( a.errors() == IDecimal::Error::None );
			REQUIRE( static_cast<int>(a) == i);
			REQUIRE( a.str() == to_string(i));

			if (i == 0) {
				REQUIRE( a.is_zero() );
				REQUIRE( !a.is_normal() );
			} else {
				REQUIRE( !a.is_zero() );
				REQUIRE( a.is_normal() );
			}

			REQUIRE( !a.is_negative() );
		}
	}
	
	SECTION("Constructor: char") {
		auto min = numeric_limits<char>::min();
		auto max = numeric_limits<char>::max();

		REQUIRE_NOTHROW(d(static_cast<char>('5')));
		REQUIRE_NOTHROW(d(static_cast<char>('a'))); // non-digits are specifically allowed

		for (int i=min; i < max; ++i) {
			auto a = d(static_cast<char>(i));
			REQUIRE( a.errors() == IDecimal::Error::None );
			REQUIRE( static_cast<char>(a) == i);
			REQUIRE( a.str() == to_string(i));

			if (i == 0) {
				REQUIRE( a.is_zero() );
				REQUIRE( !a.is_normal() );
			} else {
				REQUIRE( !a.is_zero() );
				REQUIRE( a.is_normal() );
			}

			if (i >= 0) {
				REQUIRE( !a.is_negative() );
			} else {
				REQUIRE( a.is_negative() );
			}
		}
	}
}

TEMPLATE_TEST_CASE( "Integers", "[constructors][integers]", 
	unsigned short, unsigned int, unsigned long, unsigned long long,
	short, int, long, long long) {
    	
	std::mt19937_64 gen(rd());
	auto min = numeric_limits<TestType>::min();
	auto max = numeric_limits<TestType>::max();

	REQUIRE_NOTHROW(d(static_cast<TestType>('5')));

	for (int i=0; i < LOOP_SIZE; ++i) {
		auto a = uniform_int_distribution<TestType>(min, max)(gen);
		auto A = d(a);
		REQUIRE( A.errors() == IDecimal::Error::None );
		REQUIRE( static_cast<TestType>(A) == a);
		REQUIRE( A.str() == to_string(a));

		if (a == 0) {
			REQUIRE( A.is_zero() );
			REQUIRE( !A.is_normal() );
		} else {
			REQUIRE( !A.is_zero() );
			REQUIRE( A.is_normal() );
		}
		
		if (a >= 0) {
			REQUIRE( !A.is_negative() );
		} else {
			REQUIRE( A.is_negative() );
		}
	}
}

TEST_CASE( "Constructors: strings", "[constructors][strings]" ) {
	std::mt19937_64 gen(rd());
	auto min = numeric_limits<uint64_t>::min();
	auto max = numeric_limits<uint64_t>::max();

	SECTION("std::string") {
		REQUIRE_NOTHROW(d(std::string("5")));

		for (int i=0; i < LOOP_SIZE; ++i) {
			auto l = uniform_int_distribution<uint64_t>(min, max)(gen);
			std::string a = to_string(l);
			auto A = d(a);
			REQUIRE( A.errors() == IDecimal::Error::None );
			REQUIRE( A.str() == a);

			if (a == "0") {
				REQUIRE( A.is_zero() );
				REQUIRE( !A.is_normal() );
			} else {
				REQUIRE( !A.is_zero() );
				REQUIRE( A.is_normal() );
			}
			
			if (l >= 0) {
				REQUIRE( !A.is_negative() );
			} else {
				REQUIRE( A.is_negative() );
			}
		}
	}	

	SECTION("const char *") {
		REQUIRE_NOTHROW(d("5"));

		for (int i=0; i < LOOP_SIZE; ++i) {
			auto l = uniform_int_distribution<uint64_t>(min, max)(gen);
			const char * a = to_string(l).c_str();
			auto A = d(a);
			REQUIRE( A.errors() == IDecimal::Error::None );
			REQUIRE( A.str() == a);

			if (strcmp(a, "0") == 0) {
				REQUIRE( A.is_zero() );
				REQUIRE( !A.is_normal() );
			} else {
				REQUIRE( !A.is_zero() );
				REQUIRE( A.is_normal() );
			}
			
			if (l >= 0) {
				REQUIRE( !A.is_negative() );
			} else {
				REQUIRE( A.is_negative() );
			}
		}
	}	
}

TEST_CASE( "Random Decimals", "[random]" ) {
	auto a = A();
	auto b = B();

	for (int i=0; i < LOOP_SIZE; ++i) {
		CHECK( a[i] != b[i]);
		if (a[i] == b[i]) {
			cout << "!!! The odds of this happening are astronomical !!!" << endl;
		}
	}
}

TEST_CASE( "Unary operators", "[unary]" ) { 
	SECTION("Sanity Check") {
		REQUIRE ( d(2) != d() );
		REQUIRE ( d(-2) != d() );
		
		SECTION("Negation") {
			REQUIRE( -d(2) == d(-2) );
			REQUIRE( -(-(d(2))) == d(2) );
		}
	
		SECTION("Absolute Value") {
			REQUIRE( abs(d(2)) == d(2) );
			REQUIRE( abs(d(-2)) == d(2) );
		}
	}

	SECTION("Random") {
		auto a = A();
		SECTION("Negation") {
			for (int i=0; i < LOOP_SIZE; ++i) {
				REQUIRE( (-a[i]).is_negative() != (a[i]).is_negative() );
			}
		} 
		
		SECTION("Absolute Value") {
			for (int i=0; i < LOOP_SIZE; ++i) {
				if (a[i].is_negative()){
					REQUIRE( abs(a[i]) == -a[i] );
					REQUIRE( abs(-a[i]) == -a[i] );
				} else {
					REQUIRE( abs(a[i]) == a[i] );
					REQUIRE( abs(-a[i]) == a[i] );
				}
			}
		}
	}
}

TEST_CASE( "Binary operators", "binary" ) {
	SECTION("Sanity Check") {
		REQUIRE ( d(2) != d(3) );
		REQUIRE ( d(2) != d(4) );

		SECTION("Addition") {
			auto a = d(2);
			auto b = d(3);
			REQUIRE( a + b == d(5) );

			a += b;
			REQUIRE( a == d(5) );

			a = d(2);
			a.add(b);
			REQUIRE( a == d(5) );
		}
		
		SECTION("Subtraction") {
			auto a = d(2);
			auto b = d(3);
			REQUIRE( a - b == d(-1) );

			a -= b;
			REQUIRE( a == d(-1) );
			
			a = d(2);
			a.subtract(b);
			REQUIRE( a == d(-1) );
		}

		SECTION("Multiplication") {
			auto a = d(2);
			auto b = d(3);
			REQUIRE( a * b == d(6) );

			a *= b;
			REQUIRE( a == d(6) );
			
			a = d(2);
			a.multiply(b);
			REQUIRE( a == d(6) );
		}
		
		SECTION("Division") {
			auto a = d(4);
			auto b = d(2);
			REQUIRE( a / b == d(2) );

			a /= b;
			REQUIRE( a == d(2) );
			REQUIRE( d(1) / d(10) == d("0.1") );
			
			a = d(4);
			a.divide(b);
			REQUIRE( a == d(2) );
		}
	}
	
	SECTION("Random") {
    	std::mt19937_64 gen(rd());
		auto min = numeric_limits<int>::min();
		auto max = numeric_limits<int>::max();
		auto int_dist = uniform_int_distribution<int>(min, max);

		SECTION("Addition") {
			for (int i=0; i < LOOP_SIZE; ++i) {
				auto a = int_dist(gen);
				auto b = int_dist(gen);

				// avoid overflow
				if (((b > 0) && (a > max - b)) || ((b < 0) && (a < min - b))) {
					continue;
				}

				auto A = d(a);
				auto B = d(b);
				REQUIRE( (A + B) == d(a + b) );

				A += B;
				REQUIRE( A == d(a + b) );

				A = d(a);
				A.add(B);
				REQUIRE( A == d(a + b) );
			}
		}
			
		SECTION("Subtraction") {
			for (int i=0; i < LOOP_SIZE; ++i) {
				auto a = int_dist(gen);
				auto b = int_dist(gen);

				// avoid overflow
				if (((b < 0) && (a > max - b)) || ((b > 0) && (a < min - b))) {
					continue;
				}

				auto A = d(a);
				auto B = d(b);
				REQUIRE( (A - B) == d(a - b) );

				A -= B;
				REQUIRE( A == d(a - b) );

				A = d(a);
				A.subtract(B);
				REQUIRE( A == d(a - b) );
			}
		}

		SECTION("Multiplication") {
			for (int i=0; i < LOOP_SIZE; ++i) {
				auto a = int_dist(gen);
				auto b = int_dist(gen);

				// avoid overflow
				if ((a > max / b) || ((a < min / b))) {
					continue;
				}

				auto A = d(a);
				auto B = d(b);
				REQUIRE( A * B == d(a * b) );

				A *= B;
				REQUIRE( A == d(a * b) );
				
				A = d(a);
				A.multiply(B);
				REQUIRE( A == d(a * b) );
			}
		}
		
		SECTION("Division") {
			for (int i=0; i < LOOP_SIZE; ++i) {
				auto a = int_dist(gen);
				auto b = int_dist(gen);

				// avoid overflow
				if (((a == -1) && (b == min)) || ((b == -1) && (a == min))) {
					continue;
				}
				
				auto A = d(a);
				auto B = d(b);
				REQUIRE( truncate(A / B) == d(a / b) );

				A /= B;
				REQUIRE( truncate(A) == d(a / b) );
				
				A = d(a);
				A.divide(B);
				REQUIRE( truncate(A) == d(a / b) );
			}
		}
	}
}

TEST_CASE( "Identity Properties", "[identities]" ) {
	SECTION("Sanity Check") {
		REQUIRE( d(2) + longDecimal::Zero == d(2) );
		REQUIRE( d(2) * longDecimal::One == d(2) );
	}	

	SECTION("Random") {
		std::mt19937_64 gen(rd());
		auto min = numeric_limits<int>::min();
		auto max = numeric_limits<int>::max();
		auto int_dist = uniform_int_distribution<int>(min, max);
		for (int i=0; i < LOOP_SIZE; i++) {
			auto a = int_dist(gen);
			REQUIRE( d(a) + longDecimal::Zero == d(a) );
			REQUIRE( d(a) * longDecimal::One == d(a) );
		}
	}
}

TEST_CASE( "Laws", "[laws]" ) {
	std::mt19937_64 gen(rd());
	auto min = numeric_limits<int>::min();
	auto max = numeric_limits<int>::max();
	auto int_dist = uniform_int_distribution<int>(min, max);

	SECTION("Commutative") {
		SECTION("Sanity Check") {
			REQUIRE( d(2) + d(3) == d(3) + d(2) );
			REQUIRE( d(2) * d(3) == d(2) * d(3) );
		}
	
		SECTION("Random") {
			auto a = int_dist(gen);
			auto b = int_dist(gen);
			REQUIRE( d(a) + d(b) == d(b) + d(a) );
			REQUIRE( d(a) * d(b) == d(b) * d(a) );
		}
	}

	SECTION( "Associative Laws", "[laws]" ) {
		SECTION("Sanity Check") {
			REQUIRE( (d(2) + d(3)) + d(4) == d(2) + (d(3) + d(4)) );
			REQUIRE( (d(2) * d(3)) * d(4) == d(2) * (d(3) * d(4)) );
		}
		
		SECTION("Random") {
			auto a = int_dist(gen);
			auto b = int_dist(gen);
			auto c = int_dist(gen);
			REQUIRE( (d(a) + d(b)) + d(c) == d(a) + (d(b) + d(c)) );
			REQUIRE( (d(a) * d(b)) * d(c) == d(a) * (d(b) * d(c)) );
		}
	}

	SECTION( "Distributive Law", "[laws]" ) {
		SECTION("Sanity Check") {
			REQUIRE( d(2) * (d(3) + d(4)) == (d(2) * d(3)) + (d(2) * d(4)) );
		}
		
		SECTION("Random") {
			auto min = numeric_limits<int>::min();
			auto max = numeric_limits<int>::max();
			auto a = int_dist(gen);
			auto b = int_dist(gen);
			auto c = int_dist(gen);
			REQUIRE( d(a) * (d(b) + d(c)) == (d(a) * d(b)) + (d(a) * d(c)) );
		}
	}
}

TEST_CASE( "Exceptions: Invalid", "[invalid]" ) {
	auto zero = longDecimal::Zero;
	auto inf = longDecimal::Inf;

	SECTION("Indeterminate Forms") {
		REQUIRE_THROWS_AS( zero / zero, d::InvalidException);
		REQUIRE_THROWS_AS( inf / inf, d::InvalidException);
		REQUIRE_THROWS_AS( -inf / inf, d::InvalidException);
		REQUIRE_THROWS_AS( inf / -inf, d::InvalidException);
		REQUIRE_THROWS_AS( -inf / -inf, d::InvalidException);
		REQUIRE_THROWS_AS( zero * inf, d::InvalidException);
		REQUIRE_THROWS_AS( inf * zero, d::InvalidException);
		REQUIRE_THROWS_AS( -inf * zero, d::InvalidException);
		REQUIRE_THROWS_AS( zero * -inf, d::InvalidException);
	}
	
	SECTION("Invalid Conversions") {
		REQUIRE_THROWS_AS( (unsigned char) inf, d::InvalidException);
		REQUIRE_THROWS_AS( (unsigned short) inf, d::InvalidException);
		REQUIRE_THROWS_AS( (unsigned int) inf, d::InvalidException);
		REQUIRE_THROWS_AS( (unsigned long) inf, d::InvalidException);
		REQUIRE_THROWS_AS( (unsigned long long) inf, d::InvalidException);
		REQUIRE_THROWS_AS( (char) inf, d::InvalidException);
		REQUIRE_THROWS_AS( (short) inf, d::InvalidException);
		REQUIRE_THROWS_AS( (int) inf, d::InvalidException);
		REQUIRE_THROWS_AS( (long) inf, d::InvalidException);
		REQUIRE_THROWS_AS( (long long) inf, d::InvalidException);
	}
}

TEST_CASE( "Exceptions: Divide by Zero", "[zerodivide]" ) {
	auto one = longDecimal::One;
	auto zero = longDecimal::Zero;

	auto res = one / zero;
	REQUIRE( res == longDecimal::Inf);
	REQUIRE( res.errors() == d::Error::DivideByZero); 

	one.throw_on(LongDecimal::Error::DivideByZero);
	REQUIRE_THROWS_AS( one / zero, LongDecimal::DivideByZeroException);
	
	one.throw_off(LongDecimal::Error::DivideByZero);
	REQUIRE_NOTHROW( one / zero );

	one.throw_on(LongDecimal::Error::Any);
	REQUIRE_THROWS_AS( one / zero, LongDecimal::DivideByZeroException);
}

TEST_CASE( "Exceptions: Overflow / Underflow", "[overflow]" ) {
	auto one = longDecimal::One;
	auto zero = longDecimal::Zero;
	auto max = longDecimal::Max;
	auto smallest = longDecimal::SmallestPositive;
	
	SECTION("Overflow") {
		auto res = max + max;
		REQUIRE( res.overflow() );
		REQUIRE( res.errors() == (d::Error::Overflow | d::Error::Inexact) ); 

		max.throw_on(LongDecimal::Error::Overflow);
		REQUIRE_THROWS_AS( max + max, LongDecimal::OverflowException );
		
		max.throw_off(LongDecimal::Error::Overflow );
		REQUIRE_NOTHROW( max + max );

		max.throw_on(LongDecimal::Error::Any);
		REQUIRE_THROWS_AS( max + max, LongDecimal::OverflowException );
	}
	
	SECTION("Underflow") {
		auto res = smallest / max;
		REQUIRE( res.underflow() );
		REQUIRE( res.errors() == (d::Error::Underflow | d::Error::Inexact) ); 

		smallest.throw_on(LongDecimal::Error::Underflow);
		REQUIRE_THROWS_AS( smallest / max, LongDecimal::UnderflowException );
		
		smallest.throw_off(LongDecimal::Error::Underflow);
		REQUIRE_NOTHROW( smallest / max );

		smallest.throw_on(LongDecimal::Error::Any);
		REQUIRE_THROWS_AS( smallest / max, LongDecimal::UnderflowException );
	}
}

TEST_CASE( "Exceptions: Inexact", "[inexact]" ) {
	auto a = d(2, LongDecimal::Round::NearestEven);	
	auto b = d(3, LongDecimal::Round::NearestEven);	

	auto res = a / b;
	REQUIRE( res == d("0.6666666666666666666666666666666667"));
	REQUIRE( res.inexact() );
	REQUIRE( res.errors() == d::Error::Inexact);	
	
	a.throw_on(LongDecimal::Error::Inexact);
	REQUIRE_THROWS_AS( a / b, LongDecimal::InexactException );
	
	a.throw_off(LongDecimal::Error::Inexact);
	REQUIRE_NOTHROW( a / b );

	a.throw_on(LongDecimal::Error::Any);
	REQUIRE_THROWS_AS( a / b, LongDecimal::InexactException );
}

TEST_CASE( "Rounding", "[rounding]" ) {
	SECTION( "Nearest Even" ) {
		auto a = d("10000000000000000000000000000000025", LongDecimal::Round::NearestEven);
		REQUIRE( a == d("10000000000000000000000000000000020") );
		
		a = d("-10000000000000000000000000000000025", LongDecimal::Round::NearestEven);
		REQUIRE( a == d("-10000000000000000000000000000000020") );
	}
	
	SECTION( "Nearest Away" ) {	
		auto a = d("10000000000000000000000000000000025", LongDecimal::Round::NearestAway);
		REQUIRE( a == d("10000000000000000000000000000000030") );
		
		a = d("-10000000000000000000000000000000025", LongDecimal::Round::NearestAway);
		REQUIRE( a == d("-10000000000000000000000000000000030") );
	}
	
	SECTION( "Upward" ) {	
		auto a = d("10000000000000000000000000000000025", LongDecimal::Round::Upward);
		REQUIRE( a == d("10000000000000000000000000000000030") );
		
		a = d("-10000000000000000000000000000000025", LongDecimal::Round::Upward);
		REQUIRE( a == d("-10000000000000000000000000000000020") );
	}
		
	SECTION( "Downward" ) {	
		auto a = d("10000000000000000000000000000000025", LongDecimal::Round::Downward);
		REQUIRE( a == d("10000000000000000000000000000000020") );
		
		a = d("-10000000000000000000000000000000025", LongDecimal::Round::Downward);
		REQUIRE( a == d("-10000000000000000000000000000000030") );
	}
	
	SECTION( "Toward Zero" ) {	
		auto a = d("10000000000000000000000000000000025", LongDecimal::Round::TowardZero);
		REQUIRE( a == d("10000000000000000000000000000000020") );
		
		a = d("-10000000000000000000000000000000025", LongDecimal::Round::TowardZero);
		REQUIRE( a == d("-10000000000000000000000000000000020") );
	}

	SECTION( "Mismatched throws by default" ) {
		auto a = d("10000000000000000000000000000000025", LongDecimal::Round::NearestEven);
		auto b = d("10000000000000000000000000000000015", LongDecimal::Round::NearestAway);
		REQUIRE_THROWS_AS( a + b, LongDecimal::MismatchedRoundingException);
	}

	SECTION( "Mismatched error can be overcome by specifying a round mode with a function" ) {
		auto a = d(5);
		auto b = d(9);
		a.divide(b, LongDecimal::Round::TowardZero);
		REQUIRE( a == d("0.5555555555555555555555555555555555") );
	}
}
