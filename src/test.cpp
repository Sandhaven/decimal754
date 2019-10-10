/*
 *  test.cpp
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

// These are basic sanity checks, not a full-blown unit testing suite.

#include <iomanip>
#include <iostream>
#include "decimal.h"

using namespace decimal754;
using namespace std;

struct Result {
	const char * op;
	const Decimal128 d;

	Result(const char * op, const Decimal128 & d) : op(op), d(d){}

	void report() {
		cout << left << setw(60) << this->op + string(" = ") + this->d.str() 
			<< "Errors: " + Decimal128::error_str(d.Errors()) << endl;
	}
};

int main(int argc, char ** argv) {
	Decimal128 zero("0");
	Decimal128 one("1");
	Decimal128 two("2");
	Decimal128 three("3");
	Decimal128 four("4");
	Decimal128 ten("10");
	Decimal128 max(string("9999999999999999999999999999999999") + "E+6111");
	Decimal128 min(string("9999999999999999999999999999999999") + "E-6176");
	Decimal128 inf(string("Inf"));
	Decimal128 ninf(string("-Inf"));
	
	cout << "Addition" << endl;
	Result("2 + 3", (two + three)).report();
	
	cout << "Subtraction" << endl;
	Result("2 - 3", (two - three)).report();
	
	cout << "Multiplication" << endl;
	Result("2 * 3", (two * three)).report();
	
	cout << "Division" << endl;
	Result("4 / 2", (four / two)).report();
	
	cout << "Negation" << endl;
	Result("-2", (-two)).report();
	Result("-(-2)", (-(-2))).report();
	
	cout << "Absolute Value" << endl;
	Result("|3|", (abs(three))).report();
	Result("|-3|", (abs(-three))).report();

	cout << "Identity Laws:" << endl;
	Result("0 + 2", (zero + two)).report();
	Result("1 * 2", (one * two)).report();

	cout << "Commutative Laws:" << endl;
	Result("2 + 3", (two + three)).report();
	Result("3 + 2", (three + two)).report();
	Result("2 * 3", (two * three)).report();
	Result("3 * 2", (three * two)).report();
	
	cout << "Associative Laws:" << endl;
	Result("(1 + 2) + 3", ((one + two) + 3)).report();
	Result("1 + (2 + 3)", (one + (two + 3))).report();
	Result("(1 * 2) * 3", ((one * two) * 3)).report();
	Result("1 * (2 * 3)", (one * (two * 3))).report();

	cout << "Distributive Law:" << endl;
	Result("1 * (2 + 3)", (one * (two + three))).report();
	Result("(1 * 2) + (1 * 3)", ((one * two) + (one * three))).report();
	
	cout << "Decimal division" << endl;
	Result("1 / 10", (one / ten)).report();
	
	cout << "Repeating digits:" << endl; // TODO: THIS PRINTS EVIL
	Result("2 / 3", (two / three)).report();

	cout << "Division by zero:" << endl;
	Result("1 / 0", (one / zero)).report();
	
	cout << "Overflow:" << endl;
	Result("max + max", (max + max)).report();
	
	cout << "Underflow:" << endl;
	Result("min * min", (min * min)).report();

	// Invalid operations
	try {
		cout << "Indeterminate form: 0 / 0" << endl;
		Result("0 / 0", (zero / zero)).report();
	} catch (const Decimal128::Exception & e) {
		cout << Decimal128::error_str(e.flags) << endl;
	}
	
	try {
		cout << "Indeterminate form: Inf / Inf" << endl;
		Result("Inf / Inf", (inf / inf)).report();
	} catch (const Decimal128::Exception & e) {
		cout << Decimal128::error_str(e.flags) << endl;
	}
	
	try {
		cout << "Indeterminate form: -Inf / -Inf" << endl;
		Result("-Inf / -Inf", (ninf / ninf)).report();
	} catch (const Decimal128::Exception & e) {
		cout << Decimal128::error_str(e.flags) << endl;
	}
	
	try {
		cout << "Indeterminate form: -Inf + Inf" << endl;
		Result("-Inf + Inf", (ninf + inf)).report();
	} catch (const Decimal128::Exception & e) {
		cout << Decimal128::error_str(e.flags) << endl;
	}
	
	try {
		cout << "Indeterminate form: 0 * Inf" << endl;
		Result("0 * Inf", (zero * inf)).report();
	} catch (const Decimal128::Exception & e) {
		cout << Decimal128::error_str(e.flags) << endl;
	}
	
	try {
		cout << "Indeterminate form: -Inf * 0" << endl;
		Result("-Inf * 0", (ninf * 0)).report();
	} catch (const Decimal128::Exception & e) {
		cout << Decimal128::error_str(e.flags) << endl;
	}
	
	try {
		cout << "Conversion of Inf to integer" << endl;
		Result("Inf -> long", ((long) inf)).report();
	} catch (const Decimal128::Exception & e) {
		cout << Decimal128::error_str(e.flags) << endl;
	}




	// Comparisons

	return 0;	



	// Basic Arithmetic

	//if (a == a) {
		//cout << a << " == " << a << endl;
	//}

	//cout << " 1 / 10 " << endl;
	//Decimal128 d(1);
	//Decimal128 e(10);
	//cout << d / e << endl;

	return 0;
}
