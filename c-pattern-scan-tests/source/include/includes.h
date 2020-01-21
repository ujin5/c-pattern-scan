#pragma once

/* shorthands */

#define FORCEINLINE  __forceinline
#define NOINLINE     __declspec( noinline )
#define MAYBE_UNUSED [[ maybe_unused ]]
#define NODISCARD    [[ nodiscard ]]
#define NORETURN     [[ noreturn ]]

/* includes */

// windows / stl
#include <Windows.h>
#include <intrin.h>
#include <cstdint>
#include <array>
#include <vector>
#include <string>
#include <optional>
#include <random>
#include <iostream>
#include <iomanip>

// other
#include "rand.h"

#include <c-pattern-scan/scan.h>