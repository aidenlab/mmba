
#pragma once

#ifndef __TIMING_H__
#define __TIMING_H__

#include <chrono>
using namespace std;


using clk=std::chrono::steady_clock;
// using clk=std::chrono::system_clock;


static std::chrono::time_point<clk> t0, t1;

static void timingBegin() {
	t0 = clk::now();
}

static void timingEnd() {
	t1 = clk::now();
}

static double timingCount() {
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
	return elapsed.count();
}

#endif
