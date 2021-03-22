#ifndef YM2149F_H_
#define YM2149F_H_

#include <cstdint>

class YM2149F_Noise
{
protected:
	uint32_t _sr { 0 };

	int _period { 0 };
	int _period_counter { 0 };
	int _clock_counter { 0 };
public:
	YM2149F_Noise() = default;

	void clock(int cycles);
	bool output() const { return _sr & 1; }
	void set_period(int period)
	{
		if (period < 0) period = 0;
		if (period >31) period = 31;
		_period = period;
	}
};

class YM2149F_Tone
{
protected:
	bool _output { false };

	int _period { 0 };
	int _period_counter { 0 };
	int _clock_counter { 0 };
public:
	YM2149F_Tone() = default;

	void clock(int cycles);
	bool output() const { return _output; }
	void set_period(int period)
	{
		if (period < 0)		period = 0;
		if (period > 4095)	period = 4095;
		_period = period;
	}
};

class YM2149F_Envelope
{
protected:
	enum class State { NORM, INV, HOLD };
	int _output { 0 };
	uint8_t _mode { 0 };
	State _state { State::NORM };

	int _period { 0 };
	int _period_counter { 0 };
	int _clock_counter { 0 };

	inline bool hold() const { return _mode & 1; }
	inline bool alt() const  { return _mode & 2; }
	inline bool att() const  { return _mode & 4; }
	inline bool cont() const { return _mode & 8; }
public:
	YM2149F_Envelope() = default;

	void clock(int cycles);
	int output() const { return _output; }
	void set_period(int period)
	{
		if (period < 0)		period = 0;
		if (period > 4095)	period = 4095;
		_period = period;
	}
	void set_mode(uint8_t mode)
	{
		_mode = mode & 0x0f;
		_output = att() ? 0 : 31;
		_state = State::NORM;
	}
};


class YM2149F
{
public:
	static constexpr int MASTER_FREQ = 3579545 / 2;
protected:
	YM2149F_Tone _tone[3];
	YM2149F_Noise _noise;
	YM2149F_Envelope _env;

	uint8_t _regs[16];

	uint8_t mixer() const { return _regs[7] & 0x3f; }
	int level_a() const {	if (_regs[8] & 0x10) return _env.output();
							else return (_regs[8] << 1) + (_regs[8] == 0)?0:1; }
	int level_b() const {	if (_regs[9] & 0x10) return _env.output();
							else return (_regs[9] << 1) + (_regs[9] == 0)?0:1; }
	int level_c() const {	if (_regs[10] & 0x10) return _env.output();
							else return (_regs[10] << 1) + (_regs[10] == 0)?0:1; }
	int level_ch(int ch) const
	{
		if (_regs[8+ch] & 0x10) return _env.output();
		else return (_regs[8+ch] << 1) + ((_regs[8+ch] == 0)?0:1);
	}
	double sample_ch(int ch) const;
public:
	YM2149F() = default;

	void clock(int cycles);
	void write(uint8_t reg, uint8_t val);
	uint8_t read(uint8_t reg) const { return _regs[reg]; }
	double sample_a() const;
	double sample_b() const;
	double sample_c() const;
};


#endif /* YM2149F_H_ */
