#include "YM2149F.h"

static const double s_volume_table[32] {
		0.005, 0.006, 0.007, 0.008, 0.009,
		0.011, 0.013, 0.016, 0.019, 0.022, 0.026, 0.031, 0.037, 0.044, 0.053,
		0.063, 0.074, 0.088, 0.105, 0.125, 0.149, 0.177, 0.210, 0.250, 0.297,
		0.354, 0.420, 0.500, 0.595, 0.707, 0.841, 1.000,

};

void YM2149F_Noise::clock(int cycles)
{
	_clock_counter += cycles;

	while (_clock_counter >= 16) {
		_clock_counter -= 16;

		if (_period_counter > 0)
			_period_counter--;
		else {
			_period_counter = _period;

			uint32_t feedback =
					!!(_sr & 1) ^
					!!(_sr & 8) ^
					(_sr == 0);
			_sr = (_sr >> 1) | (feedback << 16);

		}
	}
}

