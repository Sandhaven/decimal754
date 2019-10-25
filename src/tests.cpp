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

#include <charconv>

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
	
vector<IDecimal::Round> round_modes = {
	IDecimal::Round::NearestEven,
	IDecimal::Round::NearestAway,
	IDecimal::Round::Upward,
	IDecimal::Round::Downward,
	IDecimal::Round::TowardZero
};

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

//TEST_CASE("Temp", "[temp]") {
	//exit(0);
//}

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

TEST_CASE( "Null constructor", "[null constructor]" ) {
	SECTION("Null Constructor") {
		REQUIRE_NOTHROW(d());
		auto a = d();
		REQUIRE( a.errors() == IDecimal::Error::None );
		REQUIRE( !a.is_negative() );
		REQUIRE( !a.is_normal() );
		REQUIRE( a.is_zero() );
		REQUIRE( static_cast<int>(a) == 0);
		REQUIRE( a.str() == "+0E+0");
		REQUIRE( a.round_mode() == IDecimal::Round::NearestEven );
	}
}

TEMPLATE_TEST_CASE( "Integers", "[constructors][integers]", 
	unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long,
	char, short, int, long, long long) {
    	
	std::mt19937_64 gen(rd());
	auto min = numeric_limits<TestType>::min();
	auto max = numeric_limits<TestType>::max();
	auto dist = uniform_int_distribution<TestType>(min, max);

	REQUIRE_NOTHROW(d(static_cast<TestType>('5')));

	for (int i=0; i < LOOP_SIZE; ++i) {
		auto a = dist(gen);
		auto A = d(a);
		REQUIRE( A.errors() == IDecimal::Error::None );
		REQUIRE( static_cast<TestType>(A) == a);
		REQUIRE( A.round_mode() == IDecimal::Round::NearestEven );

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
	
		for (auto m = round_modes.begin(); m != round_modes.end(); ++m) {
			auto b = d(a, *m);
			REQUIRE( b.round_mode() == *m );
		}
	}
}

TEST_CASE( "Constructors: strings", "[constructors][strings]" ) {
	std::mt19937_64 gen(rd());
	auto min = numeric_limits<uint64_t>::min();
	auto max = numeric_limits<uint64_t>::max();
	auto dist = uniform_int_distribution<uint64_t>(min, max);
	
	SECTION("const char *") {
		REQUIRE_NOTHROW(d("5"));

		for (int i=0; i < LOOP_SIZE; ++i) {
			auto l = dist(gen);
			const char * a = to_string(l).c_str();
			auto A = d(a);
			REQUIRE( A.errors() == IDecimal::Error::None );
			REQUIRE( static_cast<uint64_t>(A) == l);
			REQUIRE( A.round_mode() == IDecimal::Round::NearestEven );

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

			for (auto m = round_modes.begin(); m != round_modes.end(); ++m) {
				auto b = d(a, *m);
				REQUIRE( b.round_mode() == *m );
			}
		}
	}	
	
	SECTION("std::string") {
		REQUIRE_NOTHROW(d(std::string("5")));

		for (int i=0; i < LOOP_SIZE; ++i) {
			auto l = dist(gen);
			std::string a = to_string(l);
			auto A = d(a);
			REQUIRE( A.errors() == IDecimal::Error::None );
			REQUIRE( static_cast<uint64_t>(A) == l);
			REQUIRE( A.round_mode() == IDecimal::Round::NearestEven );

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

			for (auto m = round_modes.begin(); m != round_modes.end(); ++m) {
				auto b = d(a, *m);
				REQUIRE( b.round_mode() == *m );
			}
		}
	}	
}

TEMPLATE_TEST_CASE( "Integer conversions", "[conversions][integers]", 
	unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long,
	char, short, int, long, long long) {

	std::mt19937_64 gen(rd());
	auto min = numeric_limits<TestType>::min();
	auto max = numeric_limits<TestType>::max();
	auto dist = uniform_int_distribution<TestType>(min, max);

	for (int i=0; i < LOOP_SIZE; ++i) {
		TestType a = dist(gen);
		auto A = d(a);
		REQUIRE( static_cast<TestType>(A) == a );
	}
}

TEMPLATE_TEST_CASE( "Floating conversions", "[conversions][floating]", float, double) {
	std::mt19937_64 gen(rd());
	auto min = numeric_limits<TestType>::min();
	auto max = numeric_limits<TestType>::max();
	auto dist = uniform_int_distribution<TestType>(min, max);

	for (int i=0; i < LOOP_SIZE; ++i) {
		TestType a = dist(gen);
		auto A = d(to_string(a));
		REQUIRE( static_cast<TestType>(A) == a );
	}
}

TEST_CASE("String Conversion", "[conversions][strings]") {
	std::mt19937_64 gen(rd());
	auto min = numeric_limits<long long>::min();
	auto max = numeric_limits<long long>::max();
	auto dist = uniform_int_distribution<long long>(min, max);
	
	SECTION("Special cases") {
		SECTION("str()") {
			REQUIRE(d("NaN").str() == string("+NaN"));
			REQUIRE(d("+NaN").str() == string("+NaN"));
			REQUIRE(d("-NaN").str() == string("-NaN"));
			REQUIRE(d("nan").str() == string("+NaN"));
			REQUIRE(d("-nan").str() == string("-NaN"));
			REQUIRE(d("SNaN").str() == string("+SNaN"));
			REQUIRE(d("+SNaN").str() == string("+SNaN"));
			REQUIRE(d("-SNaN").str() == string("-SNaN"));
			REQUIRE(d("snan").str() == string("+SNaN"));
			REQUIRE(d("-snan").str() == string("-SNaN"));
			REQUIRE(d("Inf").str() == string("+Inf"));
			REQUIRE(d("+Inf").str() == string("+Inf"));
			REQUIRE(d("-Inf").str() == string("-Inf"));
			REQUIRE(d("inf").str() == string("+Inf"));
			REQUIRE(d("-inf").str() == string("-Inf"));
		}

		SECTION("c_str()") {
			REQUIRE(strcmp(d("NaN").c_str(), "+NaN") == 0);
			REQUIRE(strcmp(d("+NaN").c_str(), "+NaN") == 0);
			REQUIRE(strcmp(d("-NaN").c_str(), "-NaN") == 0);
			REQUIRE(strcmp(d("nan").c_str(), "+NaN") == 0);
			REQUIRE(strcmp(d("-nan").c_str(), "-NaN") == 0);
			REQUIRE(strcmp(d("SNaN").c_str(), "+SNaN") == 0);
			REQUIRE(strcmp(d("+SNaN").c_str(), "+SNaN") == 0);
			REQUIRE(strcmp(d("-SNaN").c_str(), "-SNaN") == 0);
			REQUIRE(strcmp(d("snan").c_str(), "+SNaN") == 0);
			REQUIRE(strcmp(d("-snan").c_str(), "-SNaN") == 0);
			REQUIRE(strcmp(d("Inf").c_str(), "+Inf") == 0);
			REQUIRE(strcmp(d("+Inf").c_str(), "+Inf") == 0);
			REQUIRE(strcmp(d("-Inf").c_str(), "-Inf") == 0);
			REQUIRE(strcmp(d("inf").c_str(), "+Inf") == 0);
			REQUIRE(strcmp(d("-inf").c_str(), "-Inf") == 0);
		}
	}

	SECTION("Zero") {
		SECTION("str()") {
			REQUIRE(d(0).str() == "+0E+0");
			REQUIRE(d("00000").str() == "+0E+0");
			REQUIRE(d(-0).str() == "+0E+0");
			REQUIRE(d("-00000").str() == "-0E+0");
		}
		
		SECTION("c_str()") {
			REQUIRE(strcmp(d(0).c_str(), "+0E+0") == 0);
			REQUIRE(strcmp(d("00000").c_str(), "+0E+0") == 0);
			REQUIRE(strcmp(d(-0).c_str(), "+0E+0") == 0);
			REQUIRE(strcmp(d("-00000").c_str(), "-0E+0") == 0);
		}
	}
	
	SECTION("One") {
		SECTION("str()") {
			REQUIRE(d(1).str() == "+1E+0");
			REQUIRE(d(-1).str() == "-1E+0");
		}
		
		SECTION("c_str()") {
			REQUIRE(strcmp(d(1).c_str(), "+1E+0") == 0);
			REQUIRE(strcmp(d(-1).c_str(), "-1E+0") == 0);
		}
	}

	SECTION("str() for integers") { 
		for (int i=0; i < LOOP_SIZE; ++i) {
			auto l = dist(gen);
			
			string a = to_string(l);
			auto A = d(a);
			auto B = d(A.str());
			auto C = d(A.c_str());

			// are std::string and decimal::str convertible?
			REQUIRE( A == B );
			REQUIRE( A.str() == B.str() );
			REQUIRE( B.str() == C.str() );
			REQUIRE( strcmp(A.c_str(), B.c_str()) == 0);
			REQUIRE( strcmp(B.c_str(), C.c_str()) == 0);

			// can we recover std::string from a decimal constructed from decimal::str?
			auto c = static_cast<long long>(B);
			REQUIRE( to_string(c) == a );
		}
	}

	SECTION("str() for rationals") { 
		for (int i=0; i < LOOP_SIZE; ++i) {
			auto num = dist(gen);
			auto denom = dist(gen);
			if (denom == 0) {
				continue;
			}
			
			auto A = d(num) / d(denom);
			if (A.overflow() || A.underflow()) {
				continue;
			}
			
			auto B = d(A.str());
			auto C = d(A.c_str());

			// are std::string and decimal::str convertible?
			REQUIRE( A == B );
			REQUIRE( A.str() == B.str() );
			REQUIRE( B.str() == C.str() );
			REQUIRE( strcmp(A.c_str(), B.c_str()) == 0);
			REQUIRE( strcmp(B.c_str(), C.c_str()) == 0);

			// can we convert decimal::str to double?
			REQUIRE( static_cast<double>(B) == std::stod(B.str()) ); 
		}
	}
}

/*
TEST_CASE("Scientific notation", "[conversions][sci]") {
	std::mt19937_64 gen(rd());
	auto min = numeric_limits<long long>::min();
	auto max = numeric_limits<long long>::max();
	auto dist = uniform_int_distribution<long long>(min, max);
	
	SECTION("Special cases") {
		SECTION("sci()") {
			REQUIRE(d("NaN").sci() == string("+NaN"));
			REQUIRE(d("+NaN").sci() == string("+NaN"));
			REQUIRE(d("-NaN").sci() == string("-NaN"));
			REQUIRE(d("nan").sci() == string("+NaN"));
			REQUIRE(d("-nan").sci() == string("-NaN"));
			REQUIRE(d("SNaN").sci() == string("+SNaN"));
			REQUIRE(d("+SNaN").sci() == string("+SNaN"));
			REQUIRE(d("-SNaN").sci() == string("-SNaN"));
			REQUIRE(d("snan").sci() == string("+SNaN"));
			REQUIRE(d("-snan").sci() == string("-SNaN"));
			REQUIRE(d("Inf").sci() == string("+Inf"));
			REQUIRE(d("+Inf").sci() == string("+Inf"));
			REQUIRE(d("-Inf").sci() == string("-Inf"));
			REQUIRE(d("inf").sci() == string("+Inf"));
			REQUIRE(d("-inf").sci() == string("-Inf"));
		}

		SECTION("c_sci()") {
			REQUIRE(strcmp(d("NaN").c_sci(), "+NaN") == 0);
			REQUIRE(strcmp(d("+NaN").c_sci(), "+NaN") == 0);
			REQUIRE(strcmp(d("-NaN").c_sci(), "-NaN") == 0);
			REQUIRE(strcmp(d("nan").c_sci(), "+NaN") == 0);
			REQUIRE(strcmp(d("-nan").c_sci(), "-NaN") == 0);
			REQUIRE(strcmp(d("SNaN").c_sci(), "+SNaN") == 0);
			REQUIRE(strcmp(d("+SNaN").c_sci(), "+SNaN") == 0);
			REQUIRE(strcmp(d("-SNaN").c_sci(), "-SNaN") == 0);
			REQUIRE(strcmp(d("snan").c_sci(), "+SNaN") == 0);
			REQUIRE(strcmp(d("-snan").c_sci(), "-SNaN") == 0);
			REQUIRE(strcmp(d("Inf").c_sci(), "+Inf") == 0);
			REQUIRE(strcmp(d("+Inf").c_sci(), "+Inf") == 0);
			REQUIRE(strcmp(d("-Inf").c_sci(), "-Inf") == 0);
			REQUIRE(strcmp(d("inf").c_sci(), "+Inf") == 0);
			REQUIRE(strcmp(d("-inf").c_sci(), "-Inf") == 0);
		}
	}

	SECTION("Zero") {
		SECTION("sci()") {
			REQUIRE(d(0).sci() == "0E+0");
			REQUIRE(d("00000").sci() == "0E+0");
			REQUIRE(d(-0).sci() == "0E+0");
			REQUIRE(d("-00000").sci() == "0E+0");
		}
		
		SECTION("c_sci()") {
			REQUIRE(strcmp(d(0).c_sci(), "0E+0") == 0);
			REQUIRE(strcmp(d("00000").c_sci(), "0E+0") == 0);
			REQUIRE(strcmp(d(-0).c_sci(), "0E+0") == 0);
			REQUIRE(strcmp(d("-00000").c_sci(), "0E+0") == 0);
		}
	}
	
	SECTION("One") {
		SECTION("sci()") {
			REQUIRE(d(1).sci() == "1E+0");
			REQUIRE(d(-1).sci() == "-1E+0");
		}
		
		SECTION("c_sci()") {
			REQUIRE(strcmp(d(1).c_sci(), "1E+0") == 0);
			REQUIRE(strcmp(d(-1).c_sci(), "-1E+0") == 0);
		}
	}

	SECTION("sci() for integers") { 
		for (int i=0; i < LOOP_SIZE; ++i) {
			auto l = dist(gen);
			
			string a = to_string(l);
			auto A = d(a);
			auto B = d(A.sci());
			auto C = d(A.c_sci());

			// are std::string and decimal::sci convertible?
			REQUIRE( A == B );
			REQUIRE( A.sci() == B.sci() );
			REQUIRE( B.sci() == C.sci() );
			REQUIRE( strcmp(A.c_sci(), B.c_sci()) == 0);
			REQUIRE( strcmp(B.c_sci(), C.c_sci()) == 0);

			// can we recover std::string from a decimal constructed from decimal::sci?
			auto c = static_cast<long long>(B);
			REQUIRE( to_string(c) == a );
		}
	}

	SECTION("sci() for rationals") { 
		for (int i=0; i < LOOP_SIZE; ++i) {
			auto num = dist(gen);
			auto denom = dist(gen);
			if (denom == 0) {
				continue;
			}
			
			auto A = d(num) / d(denom);
			if (A.overflow() || A.underflow()) {
				continue;
			}
			
			auto B = d(A.sci());
			auto C = d(A.c_sci());

			// are std::string and decimal::str convertible?
			REQUIRE( A == B );
			REQUIRE( A.sci() == B.sci() );
			REQUIRE( B.sci() == C.sci() );
			REQUIRE( strcmp(A.c_sci(), B.c_sci()) == 0);
			REQUIRE( strcmp(B.c_sci(), C.c_sci()) == 0);

			// can we convert decimal::str to double?
			REQUIRE( static_cast<double>(B) == std::stod(B.sci()) ); 
		}
	}
}
*/

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
