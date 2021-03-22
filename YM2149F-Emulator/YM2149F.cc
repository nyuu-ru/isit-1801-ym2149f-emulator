#include "YM2149F.h"

static const double s_volume_table[32] {
		0.005, 0.006, 0.007, 0.008, 0.009, 0.011, 0.013, 0.016,
		0.019, 0.022, 0.026, 0.031, 0.037, 0.044, 0.053, 0.063,
		0.074, 0.088, 0.105, 0.125, 0.149, 0.177, 0.210, 0.250,
		0.297, 0.354, 0.420, 0.500, 0.595, 0.707, 0.841, 1.000,
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

void YM2149F_Tone::clock(int cycles)
{
	_clock_counter += cycles;

	while (_clock_counter >= 8) {
		_clock_counter -= 8;

		if (_period_counter > 0)
			_period_counter--;
		else {
			_period_counter = _period;

			_output = not _output;
		}
	}
}

/*

	CNT ATT	ALT	HLD	|	NRM	INV	HLD
	0	0	x	x		31	x	0
	0	1	x	x		0	x	0
	1	0	0	0		31	x	x
	1	0	0	1		31	x	0
	1	0	1	0		31	0	x
	1	0	1	1		31	x	31
	1	1	0	0		0	x	x
	1	1	0	1		0	x	31
	1	1	1	0		0	31	x
	1	1	1	1		0	x	0

	cont() and (not att() xor alt() xor hold())
	(att() xor alt()) and cont()
 */

void YM2149F_Envelope::clock(int cycles)
{
	_clock_counter += cycles;

	while (_clock_counter >= 8) {
		_clock_counter -= 8;

		if (_period_counter > 0)
			_period_counter--;
		else {
			_period_counter = _period;

			switch (_state) {
			case State::NORM:
				if (att()) _output++; else _output--;

				if (_output < 0 or _output >= 32) {
					_output = (cont() and (not att() xor alt() xor hold())) ?
								31 : 0;
					if (hold() or not cont()) _state = State::HOLD;
					else if (alt()) _state = State::INV;
				}
				break;
			case State::INV:
				if (att()) _output--; else _output++;

				if (_output < 0 or _output >= 32) {
					_output = att() ? 0 : 31;
					_state = State::NORM;
				}
				break;
			case State::HOLD:
				_output = ((att() xor alt()) and cont()) ? 31 : 0;
				break;
			default:
				; // В эту ветку мы попасть не должны
			}
		}
	}

}

void YM2149F::clock(int cycles)
{
	_tone[0].clock(cycles);
	_tone[1].clock(cycles);
	_tone[2].clock(cycles);
	_noise.clock(cycles);
	_env.clock(cycles);
}

void YM2149F::write(uint8_t reg, uint8_t val)
{
	if (reg > 15) return;
	_regs[reg] = val;
	switch (reg) {
	case 0:
	case 1:
		_tone[0].set_period( (uint16_t(_regs[0]) + (uint16_t(_regs[1])<<8)) & 0x0fff );
		break;
	case 2:
	case 3:
		_tone[1].set_period( (uint16_t(_regs[2]) + (uint16_t(_regs[3])<<8)) & 0x0fff );
		break;
	case 4:
	case 5:
		_tone[2].set_period( (uint16_t(_regs[4]) + (uint16_t(_regs[5])<<8)) & 0x0fff );
		break;
	case 6:
		_noise.set_period(_regs[6] & 0x1f);
		break;
	case 11:
	case 12:
		_env.set_period( uint16_t(_regs[11]) + (uint16_t(_regs[12]) << 8) );
		break;
	case 13:
		_env.set_mode(_regs[13] & 0x0f);
		break;
	default:
		;
	}
}

double YM2149F::sample_ch(int ch) const
{
	bool t_out = _tone[ch].output() or (_regs[7] & (1 << ch));
	bool n_out = _noise.output() or (_regs[7] & (8 << ch));
	bool out = t_out and n_out;

	double result = out ? 1. : -1.;
	int volume_level = level_ch(ch);
	double volume = s_volume_table[volume_level];

	return result * volume;
}

double YM2149F::sample_a() const
{
	return sample_ch(0);
}

double YM2149F::sample_b() const
{
	return sample_ch(1);
}

double YM2149F::sample_c() const
{
	return sample_ch(2);
}
