#include <iostream>
#include <iomanip>
#include <chrono>
#include <functional>
#include <thread>
#include "YM2149F.h"
#include "AudioDriver.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>

class Timer
{
protected:
	std::function<void()> _func;
	int _period;
	std::thread _timer_thread;
	bool _stop { false };
public:
	Timer(long period_ns, std::function<void()> func): _func(func), _period(period_ns)
	{
		_timer_thread = std::thread( [&]() {
			using clk = std::chrono::high_resolution_clock;
			auto now = clk::now();
			auto per = std::chrono::nanoseconds(_period);
			std::cout << "period is " << per.count() << "ns" << std::endl;
			auto next = now + per;
			while (not _stop) {
				while (clk::now() < next) {} // Ничего не делаем, ждём
				_func();
				next += per;
			}
		});
		_timer_thread.detach();
	}

	void stop() { _stop = true; }
};

int main(int, char **)
{
	SDL_Init(SDL_INIT_EVERYTHING);

	AudioDriver drv;
	YM2149F ymchip;

	Timer t1(1'000'000'000 / AudioDriver::SAMPLING_RATE, [&](){
		ymchip.clock(YM2149F::MASTER_FREQ / AudioDriver::SAMPLING_RATE);
		short buffer = 	5000 * ymchip.sample_a() +
						5000 * ymchip.sample_b() +
						5000 * ymchip.sample_c();
		drv.enqueue(&buffer, 1);
//		std::cout << buffer << std::endl;
	});



	ymchip.write(0, 254);
	ymchip.write(1, 0);
	ymchip.write(2, 126);
	ymchip.write(3, 0);
	ymchip.write(8, 15);
	ymchip.write(9, 15);
	ymchip.write(7, 0x00);

	using clk = std::chrono::high_resolution_clock;
	auto now = clk::now();
	auto next = now + std::chrono::seconds(2);
	while (clk::now() < next) {} // Ничего не делаем, ждём


	t1.stop();


	return 0;
}


