#include <iostream>
#include "MyMath.hpp"

using namespace std;
using namespace MyMath;

int main() {
	Vector<2> x{1.0, 2.0}, y{3.0, 4.0};
	cout << (x+y).toString() << endl;
	return 0;
}
