// ThreadPool_matrixmultiply.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <random>
#include <chrono>
#include <vector>

#include "thread_pool.h"


std::random_device rd;
std::mt19937 mt(rd());
std::uniform_int_distribution<int> dist(-1000, 1000);
auto rnd = std::bind(dist, mt);

void simulate_hard_computation() {
	std::this_thread::sleep_for(std::chrono::milliseconds(100 + rnd()));
}

void multiply_output(int & out, const int a, const int b) {
	simulate_hard_computation();
	out = a * b;
	std::cout << a << " * " << b << " = " << out << std::endl;
}

int main()
{
	ThreadPool pool(std::thread::hardware_concurrency());
	std::vector<std::future<void>> results;

	for (int i = 0; i < 500; i++) {
		results.emplace_back(
			pool.enqueue([i] {
				int multiplication;
				multiply_output(multiplication, rnd(), rnd());
				return;
			})
		);
	}
	//auto res = pool.enqueue([](int answer) { return answer; }, 42);

	//for (auto && result : results)
		//std::cout << result.get() << ' ';
	std::cout << std::endl;

	while (1);
    return 0;
}

