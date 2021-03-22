#include <iostream>
#include <iomanip>
#include "YM2149F.h"

int main(int, char **)
{
	YM2149F_Envelope t;

	t.set_period(0);
	t.set_mode(15);

	for (int i = 0; i < 400; ++i) {
		if (i % 20 == 0) std::cout << std::endl;
		std::cout << std::setw(3) << t.output();
		t.clock(8);
	}

	return 0;
}


