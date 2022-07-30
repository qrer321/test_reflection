#pragma once
#include <chrono>

class delta_timer
{
public:
	delta_timer();
	~delta_timer() = default;

	void Tick();
	float GetDelta() const;

private:
	std::chrono::time_point<std::chrono::system_clock> chrono_start;
	std::chrono::time_point<std::chrono::system_clock> chrono_end;
	std::chrono::duration<float, std::ratio<1, 1000>> chrono_delta;
};

inline delta_timer::delta_timer()
	: chrono_start()
	, chrono_end()
	, chrono_delta()
{
}

inline void delta_timer::Tick()
{
	chrono_start = std::chrono::system_clock::now();
	chrono_end = std::chrono::system_clock::now();

	chrono_delta = chrono_end - chrono_start;
}

inline float delta_timer::GetDelta() const
{
	return chrono_delta.count();
}