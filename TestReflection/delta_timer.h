#pragma once
#include <chrono>

class delta_timer
{
public:
	delta_timer()
		: delta_time_(0.f)
	{}
	~delta_timer() = default;

	void Reset();
	float Tick();
	float GetDelta() const { return delta_time_; }

private:
	std::chrono::steady_clock::time_point	current_;
	std::chrono::steady_clock::time_point	prev_;
	float delta_time_;
};

inline void delta_timer::Reset()
{
	current_ = std::chrono::high_resolution_clock::now();
	prev_ = current_;
}

inline float delta_timer::Tick()
{
	current_ = std::chrono::high_resolution_clock::now();

	const double delta_time_double = std::chrono::duration<double>(current_ - prev_).count();
	prev_ = current_;

	delta_time_ = static_cast<float>(delta_time_double);
	return delta_time_;
}
