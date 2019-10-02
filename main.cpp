#include <iostream>
#include <math.h>
using namespace std;

int main()
{
	uint32_t x = 0x9b80bc11;
	bool bits[32];

	// Header
	int type = 0;
	bool tr = false;
	int window = 0;
	bool L = false;
	int Length = 0;


	for (int i = 31; i >= 0; i--) bits[31-i] = ((x >> i) & 1);

	for (int i = 0; i < 32; i++) cout << bits[i]; cout << endl;



	type = bits[0] * 2 + bits[1];
	cout << "type = " << type << endl;

	tr = bits[2];
	cout << "truncated = " << tr << endl;

	for (int i = 0; i < 5; i++) window = 2 * window + bits[3 + i];
	cout << "window = " << window << endl;

	L = bits[8];
	cout << "L = " << L << endl;

	for (int i = 0; i < 7 + 8 * L; i++) Length = 2 * Length + bits[9 + i];
	cout << "Length = " << Length << endl;

	if (type != 1 && tr) cout << "Ignored" << endl;

	if (type == 1 && Length == 0 /* && ... */) cout << "Transfer ended" << endl;

	return 0;
}
