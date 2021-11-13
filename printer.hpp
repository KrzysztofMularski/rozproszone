#pragma once

#include <iostream>
#include <cstdarg>

#include "main.hpp"

//         foreground  background
// black        30         40       // not using due to contrast
// red          31         41
// green        32         42
// yellow       33         43
// blue         34         44
// magenta      35         45
// cyan         36         46
// white        37         47

void print(const char* fmt, ...);
