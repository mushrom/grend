#pragma once
#include <vector>
#include <chrono>

namespace grendx {

class sma_counter {
	public:
		sma_counter(unsigned n_samples=8) {
			samples.resize(n_samples, 0);
		}

		void start(void);
		void stop(void);
		double average(void);

	private:
		void add_sample(double sample);
		double get_sample(int i);

		std::vector<double> samples;
		size_t frameptr = 0;

		std::chrono::time_point<std::chrono::high_resolution_clock> begin;
};

// namespace grendx
}
