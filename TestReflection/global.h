#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <unordered_map>
#include <set>
#include <queue>
#include <atomic>
#include <any>
#include <functional>

#include <conio.h>
#include <iostream>


using type_info_ref = std::reference_wrapper<const std::type_info>;
struct hasher
{
    std::size_t operator()(type_info_ref code) const
    {
        return code.get().hash_code();
    }
};

struct equal_to
{
	bool operator() (type_info_ref lhs, type_info_ref rhs) const
	{
		return lhs.get() == rhs.get();
	}
};

