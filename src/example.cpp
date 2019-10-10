#include <iostream>
#include "decimal.h"

using namespace decimal754;
using namespace std;

int main(int argc, char ** argv) {
	Decimal128 a("2");
	Decimal128 b("3");
	Decimal128 c = a * b;

	cout << a << " * " << b << " = " << c << endl;
	return 0;
}
