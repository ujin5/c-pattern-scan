# c-pattern-scan

A simple Pattern scanning / Signature scanning library written in C99.

**This project is still a work in progress.**

## Features and compatibility
* Supports both C and C++ linking.
* Supports both code-style and IDA-style patterns.
* Small memory footprint; simple codebase.
* Currently only tested on **x86** and **x86-64** but it should work on any architecture with a compiler that abides by C99 standards.

## Building
The `build` directory includes Visual Studio Solutions and CMake files. The built library and binaries are placed into the `lib` and `bin` folders in the respective build folders.

## Usage
Add the `c-pattern-scan/includes` directory to your includes path and include the `c-pattern-scan/scan.h` header, then link with the library and you're ready to go.

Heres a simple IDA-style pattern scanning example in C++:
```
#include <c-pattern-scan/scan.h>

wchar_t   *pattern   = L"AA BB CC ? DD EE"; // some unique order of bytes
uintptr_t scan_start = 0x1000;              // some arbitrary address
size_t    scan_size  = 5000;                // scan 5000 bytes

const auto found = ps_find_idastyle( pattern, scan_start, scan_size ); // find bytes, returns 0 on failure
if( found )
{
    // do something with "found"...
}
```

... Or if you prefer code-style patterns for whatever reason:
```
#include <c-pattern-scan/scan.h>

wchar_t   *pattern   = L"\xAA\xBB\xCC\x00\xDD\xEE"; // some unique order of bytes
wchar_t   *mask      = L"xxx?xx"                    // the mask for the pattern above
uintptr_t scan_start = 0x1000;                      // some arbitrary address
size_t    scan_size  = 5000;                        // scan 5000 bytes

const auto found = ps_find_codestyle( pattern, mask, scan_start, scan_size ); // find bytes, returns 0 on failure
if( found )
{
    // do something with "found"...
}
```

Give the `c-pattern-scan/scan.h` header a read through for some extra functionality, if you so desire.

Theres also an included `c-pattern-scan-tests` project with some examples.

## Contributing
Anyone is allowed to contribute! Please feel free to make pull requests with suggestions, changes or fixes and I will review it when I have time.