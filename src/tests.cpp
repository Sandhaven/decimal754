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

TEST_CASE( "Unary operators", "[unary operators]" ) {
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

		auto res = d(2) / d(3);
		REQUIRE( res == d("0.6666666666666666666666666666666667"));
		REQUIRE( res.Errors() == d::Error::Inexact);

		res = d(1) / d(0);
		REQUIRE( res == d("Inf"));
		REQUIRE( res.Errors() == d::Error::DivideByZero); // TODO: MAKE ZERO-DIVIDE EXCEPTION
	}
}

TEST_CASE( "Identity Properties", "[identities]" ) {
    REQUIRE( two + zero == two );
    REQUIRE( two * one == two );
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

/*
TEST_CASE( "Overflow / Underflow", "[overflow]" ) {
	d max(string("9999999999999999999999999999999999") + "E+6111"); // TODO: Make these static instances
	d min(string("9999999999999999999999999999999999") + "E-6176");
	
	SECTION("Overflow") {
		REQUIRE( (max + max).Errors() == d::Error::Overflow);
	}
	
	SECTION("Underflow") {
		REQUIRE( (min - min).Errors() == d::Error::Underflow);
	}
}
*/

TEST_CASE( "Exceptions", "[exceptions]" ) {
	d zero(0);
	d inf(string("Inf"));

	SECTION("Indeterminate Forms") {
		REQUIRE_THROWS_AS( zero / zero, d::Exception); // TODO: Create InvalidException
		REQUIRE_THROWS_AS( inf / inf, d::Exception);
		REQUIRE_THROWS_AS( -inf / inf, d::Exception);
		REQUIRE_THROWS_AS( inf / -inf, d::Exception);
		REQUIRE_THROWS_AS( -inf / -inf, d::Exception);
		REQUIRE_THROWS_AS( zero * inf, d::Exception);
		REQUIRE_THROWS_AS( inf * zero, d::Exception);
		REQUIRE_THROWS_AS( -inf * zero, d::Exception);
		REQUIRE_THROWS_AS( zero * -inf, d::Exception);
	}
	
	SECTION("Invalid Conversion") {
		REQUIRE_THROWS_AS( (long) inf, d::Exception);
	}
}
