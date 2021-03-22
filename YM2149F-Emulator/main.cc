#include <iostream>
#include "YM2149F.h"

int main(int, char **)
{
	YM2149F_Noise t;

	t.set_period(0);

	for (int i = 0; i < 100000; ++i) {
		t.clock(16);
		std::cout << t.output();
		if (i % 60 == 0) std::cout << std::endl;
	}

	return 0;
}


