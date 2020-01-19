# c-pattern-scan

A simple Pattern scanning / Signature scanning library written in C99.

**This project is still a work in progress.**

## Features and compatibility
* Simple and small code and memory footprint.
* Supports both C and C++ linking.
* Supports both code-style and IDA-style patterns.
* Currently only tested on **x86** and **MSVC 14.24** but it should work on any architecture and any compiler that supports C99.

## Usage
Include the `c-pattern-scan/scan.h` header and link with the library and you're ready to go.

Heres a simple IDA-style pattern scanning example in C++:
```
#include <c-pattern-scan/scan.h>

uintptr_t scan_start = 0x1000; // some start address
size_t    scan_size  = 5000;   // scan 5000 bytes

const auto found = ps_find_idastyle(
    scan_start,
    scan_end,
    "AA BB CC ? DD"
);

std::wcout << std::uppercase << std::hex << std::setfill( L'0' ) << std::setw( 2 );
std::wcout << L"found: 0x" << found << std::endl << std::endl;
```

Give the `c-pattern-scan/scan.h` header a read through for some extra functionality, if you so desire.

Theres also an included `c-pattern-scan-tests` project with some examples.

## Contributing
Anyone is allowed to contribute! Please freel free to make pull requests with suggestions, changes or fixes and I will review it when I have time.
