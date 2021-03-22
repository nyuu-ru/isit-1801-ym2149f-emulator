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


class YM2149F
{

};


#endif /* YM2149F_H_ */
