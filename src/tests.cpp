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
#include "decimal.h"
#include "catch.hpp"

using namespace decimal754;
using namespace std;

Decimal128 ntwo("-2");
Decimal128 none("-1");
Decimal128 zero("0");
Decimal128 one("1");
Decimal128 two("2");
Decimal128 three("3");
Decimal128 four("4");
Decimal128 five("5");
Decimal128 six("6");
Decimal128 ten("10");

typedef Decimal128 d;

TEST_CASE( "Constants", "[constants]" ) {
	SECTION("Values") {
		REQUIRE( decimal128::Zero == d(0) );
		REQUIRE( decimal128::One == d(1) );
		REQUIRE( decimal128::Max == d(std::string("9999999999999999999999999999999999") + "E+6111") );
		REQUIRE( decimal128::Min == d(std::string("-9999999999999999999999999999999999") + "E+6111") );
		REQUIRE( decimal128::SmallestPositive == d(std::string("9999999999999999999999999999999999") + "E-6176") );
		REQUIRE( decimal128::SmallestNegative == d(std::string("-9999999999999999999999999999999999") + "E-6176") );
		REQUIRE( decimal128::Inf == d("Inf") );
	}

	SECTION("Uniqueness") {
		vector<Decimal128> v = { 
			decimal128::Zero, 
			decimal128::One, 
			decimal128::Max, 
			decimal128::Min, 
			decimal128::SmallestPositive, 
			decimal128::SmallestNegative, 
			decimal128::Inf 
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

// TODO: constructors

TEST_CASE( "Unary operators", "[unary operators]" ) { // TODO: Use random values
	REQUIRE ( d(2) != d() );
	REQUIRE ( d(-2) != d() );

	SECTION("Negation") {
		REQUIRE( -d(2) == d(-2) );
		REQUIRE( -(-(d(2))) == d(2) );
	}

	SECTION("Absolute Value") {
		REQUIRE( abs(d(-2)) == d(2) );
		REQUIRE( abs(d(2)) == d(2) );
	}
}

TEST_CASE( "Binary operators", "binary operators" ) {
	REQUIRE ( d(2) != d(3) );
	REQUIRE ( d(2) != d(4) );

	SECTION("Addition") {
		REQUIRE( d(2) + d(3) == d(5) );
	}
	
	SECTION("Subtraction") {
		REQUIRE( d(2) - d(3) == -d(1) );
	}

	SECTION("Multiplication") {
		REQUIRE( d(2) * d(3) == d(6) );
	}
	
	SECTION("Division") {
		REQUIRE( d(4) / d(2) == d(2) );
		REQUIRE( d(1) / d(10) == d("0.1") );
	}
}

TEST_CASE( "Identity Properties", "[identities]" ) {
    REQUIRE( d(2) + decimal128::Zero == d(2) );
    REQUIRE( d(2) * decimal128::One == d(2) );
}

TEST_CASE( "Laws", "[laws]" ) {
	SECTION("Commutative") {
		REQUIRE( d(2) + d(3) == d(3) + d(2) );
		REQUIRE( d(2) * d(3) == d(2) * d(3) );
	}
	SECTION( "Associative Laws", "[laws]" ) {
		REQUIRE( (d(2) + d(3)) + d(4) == d(2) + (d(3) + d(4)) );
		REQUIRE( (d(2) * d(3)) * d(4) == d(2) * (d(3) * d(4)) );
	}
	SECTION( "Distributive Law", "[laws]" ) {
		REQUIRE( d(2) * (d(3) + d(4)) == (d(2) * d(3)) + (d(2) * d(4)) );
	}
}

TEST_CASE( "Exceptions: Invalid", "[invalid]" ) {
	auto zero = decimal128::Zero;
	auto inf = decimal128::Inf;

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
	auto one = decimal128::One;
	auto zero = decimal128::Zero;

	auto res = one / zero;
	REQUIRE( res == decimal128::Inf);
	REQUIRE( res.errors() == d::Error::DivideByZero); 

	one.throw_on(Decimal128::Error::DivideByZero);
	REQUIRE_THROWS_AS( one / zero, Decimal128::DivideByZeroException);
	
	one.throw_off(Decimal128::Error::DivideByZero);
	REQUIRE_NOTHROW( one / zero );

	one.throw_on(Decimal128::Error::Any);
	REQUIRE_THROWS_AS( one / zero, Decimal128::DivideByZeroException);
}

TEST_CASE( "Exceptions: Overflow / Underflow", "[overflow]" ) {
	auto one = decimal128::One;
	auto zero = decimal128::Zero;
	auto max = decimal128::Max;
	auto smallest = decimal128::SmallestPositive;
	
	SECTION("Overflow") {
		auto res = max + max;
		REQUIRE( res.overflow() );
		REQUIRE( res.errors() == (d::Error::Overflow | d::Error::Inexact) ); 

		max.throw_on(Decimal128::Error::Overflow);
		REQUIRE_THROWS_AS( max + max, Decimal128::OverflowException );
		
		max.throw_off(Decimal128::Error::Overflow );
		REQUIRE_NOTHROW( max + max );

		max.throw_on(Decimal128::Error::Any);
		REQUIRE_THROWS_AS( max + max, Decimal128::OverflowException );
	}
	
	SECTION("Underflow") {
		auto res = smallest / max;
		REQUIRE( res.underflow() );
		REQUIRE( res.errors() == (d::Error::Underflow | d::Error::Inexact) ); 

		smallest.throw_on(Decimal128::Error::Underflow);
		REQUIRE_THROWS_AS( smallest / max, Decimal128::UnderflowException );
		
		smallest.throw_off(Decimal128::Error::Underflow);
		REQUIRE_NOTHROW( smallest / max );

		smallest.throw_on(Decimal128::Error::Any);
		REQUIRE_THROWS_AS( smallest / max, Decimal128::UnderflowException );
	}
}

TEST_CASE( "Exceptions: Inexact", "[inexact]" ) {
	auto a = d(2, Decimal128::Round::NearestEven);	
	auto b = d(3, Decimal128::Round::NearestEven);	

	auto res = a / b;
	REQUIRE( res == d("0.6666666666666666666666666666666667"));
	REQUIRE( res.inexact() );
	REQUIRE( res.errors() == d::Error::Inexact);	
	
	a.throw_on(Decimal128::Error::Inexact);
	REQUIRE_THROWS_AS( a / b, Decimal128::InexactException );
	
	a.throw_off(Decimal128::Error::Inexact);
	REQUIRE_NOTHROW( a / b );

	a.throw_on(Decimal128::Error::Any);
	REQUIRE_THROWS_AS( a / b, Decimal128::InexactException );
}

TEST_CASE( "Rounding", "[rounding]" ) {
	SECTION( "Nearest Even" ) {
		auto a = d("10000000000000000000000000000000025", Decimal128::Round::NearestEven);
		REQUIRE( a == d("10000000000000000000000000000000020") );
		
		a = d("-10000000000000000000000000000000025", Decimal128::Round::NearestEven);
		REQUIRE( a == d("-10000000000000000000000000000000020") );
	}
	
	SECTION( "Nearest Away" ) {	
		auto a = d("10000000000000000000000000000000025", Decimal128::Round::NearestAway);
		REQUIRE( a == d("10000000000000000000000000000000030") );
		
		a = d("-10000000000000000000000000000000025", Decimal128::Round::NearestAway);
		REQUIRE( a == d("-10000000000000000000000000000000030") );
	}
	
	SECTION( "Upward" ) {	
		auto a = d("10000000000000000000000000000000025", Decimal128::Round::Upward);
		REQUIRE( a == d("10000000000000000000000000000000030") );
		
		a = d("-10000000000000000000000000000000025", Decimal128::Round::Upward);
		REQUIRE( a == d("-10000000000000000000000000000000020") );
	}
		
	SECTION( "Downward" ) {	
		auto a = d("10000000000000000000000000000000025", Decimal128::Round::Downward);
		REQUIRE( a == d("10000000000000000000000000000000020") );
		
		a = d("-10000000000000000000000000000000025", Decimal128::Round::Downward);
		REQUIRE( a == d("-10000000000000000000000000000000030") );
	}
	
	SECTION( "Toward Zero" ) {	
		auto a = d("10000000000000000000000000000000025", Decimal128::Round::TowardZero);
		REQUIRE( a == d("10000000000000000000000000000000020") );
		
		a = d("-10000000000000000000000000000000025", Decimal128::Round::TowardZero);
		REQUIRE( a == d("-10000000000000000000000000000000020") );
	}

	SECTION( "Mismatched" ) {
		auto a = d("10000000000000000000000000000000025", Decimal128::Round::NearestEven);
		auto b = d("10000000000000000000000000000000015", Decimal128::Round::NearestAway);
		REQUIRE_THROWS_AS( a + b, Decimal128::MismatchedRoundingException);
	}
}
