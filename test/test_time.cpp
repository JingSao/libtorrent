/*

Copyright (c) 2013, 2015-2017, 2019-2021, Arvid Norberg
Copyright (c) 2018, Alden Torres
All rights reserved.

You may use, distribute and modify this code under the terms of the BSD license,
see LICENSE file.
*/

#include "test.hpp"

#include "libtorrent/time.hpp"
#include "libtorrent/aux_/time.hpp"

#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>

using namespace lt;

namespace {

void check_timer_loop(std::mutex& m, time_point& last, std::condition_variable& cv)
{
	std::unique_lock<std::mutex> l(m);
	cv.wait(l);
	l.unlock();

	for (int i = 0; i < 10000; ++i)
	{
		std::lock_guard<std::mutex> ll(m);
		time_point now = clock_type::now();
		TEST_CHECK(now >= last);
		last = now;
	}
}

} // anonymous namespace

TORRENT_TEST(time)
{
	// make sure the time classes have correct semantics

	TEST_EQUAL(total_milliseconds(milliseconds(100)), 100);
	TEST_EQUAL(total_milliseconds(milliseconds(1)),  1);
	TEST_EQUAL(total_milliseconds(seconds(1)), 1000);
	TEST_EQUAL(total_seconds(minutes(1)), 60);
	TEST_EQUAL(total_seconds(hours(1)), 3600);

	// make sure it doesn't wrap at 32 bit arithmetic
	TEST_EQUAL(total_seconds(seconds(281474976)), 281474976);
	TEST_EQUAL(total_milliseconds(milliseconds(281474976)), 281474976);

	// make sure the timer is monotonic

	time_point now = clock_type::now();
	time_point last = now;
	for (int i = 0; i < 1000; ++i)
	{
		now = clock_type::now();
		TEST_CHECK(now >= last);
		last = now;
	}

	std::mutex m;
	std::condition_variable cv;
	std::thread t1(&check_timer_loop, std::ref(m), std::ref(last), std::ref(cv));
	std::thread t2(&check_timer_loop, std::ref(m), std::ref(last), std::ref(cv));
	std::thread t3(&check_timer_loop, std::ref(m), std::ref(last), std::ref(cv));
	std::thread t4(&check_timer_loop, std::ref(m), std::ref(last), std::ref(cv));

	std::this_thread::sleep_for(lt::milliseconds(100));

	cv.notify_all();

	t1.join();
	t2.join();
	t3.join();
	t4.join();
}

TORRENT_TEST(test_time_conversion)
{
	int success = 0;
	const int test_count = 10;

	for (int i = 0; i < test_count; ++i)
	{
		auto now = aux::time_now32() + seconds32(rand() % 1000);
		auto ctime = aux::to_time_t(now);
		auto converted = aux::from_time_t(ctime);
		if (now == converted)
			++success;
		else
			std::cout << "now: " << now.time_since_epoch().count() << " converted: " << converted.time_since_epoch().count() << '\n';
	}

	// conversion depends on wall clock and may be flaky
	TEST_CHECK(success >= test_count - 1);
}

TORRENT_TEST(test_time_conversion_with_offset)
{
	int success = 0;
	const int test_count = 10;

	for (int i = 0; i < test_count; ++i)
	{
		auto now = aux::time_now32() + seconds32(rand() % 1000);
		auto ctime = aux::to_time_t(now);
		// offset by 100 seconds
		auto converted = aux::from_time_t(ctime + 100);
		if (now + seconds32(100) == converted)
			++success;
		else
			std::cout << "now: " << now.time_since_epoch().count() << " converted: " << converted.time_since_epoch().count() << '\n';
	}

	// conversion depends on wall clock and may be flaky
	TEST_CHECK(success >= test_count - 1);
}
