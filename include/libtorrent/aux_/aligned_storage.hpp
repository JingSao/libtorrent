/*

Copyright (c) 2017, Alden Torres
Copyright (c) 2017, 2020, 2022, Arvid Norberg
All rights reserved.

You may use, distribute and modify this code under the terms of the BSD license,
see LICENSE file.
*/

#ifndef TORRENT_ALIGNED_STORAGE_HPP_INCLUDE
#define TORRENT_ALIGNED_STORAGE_HPP_INCLUDE

#include <type_traits>

namespace libtorrent { namespace aux {

#if __cplusplus >= 202302L || defined __GNUC__ && __GNUC__ < 5 && !defined(_LIBCPP_VERSION)

// this is for backwards compatibility with not-quite C++11 compilers
// and for C++23 which deprecated std::aligned_storage
template <std::size_t Len, std::size_t Align = alignof(void*)>
struct aligned_storage
{
	struct type
	{
		alignas(Align) char buffer[Len];
	};
};

#else

using std::aligned_storage;

#endif

}}

#endif
