#pragma once

#include <stdint.h>
#include <stdbool.h>

/* shorthands */

#define PS_NOINLINE __declspec( noinline )

/* types */

typedef struct PS_PatternByte
{
    uint8_t m_value;
    bool    m_is_wildcard;
} PS_PatternByte;

typedef struct PS_Pattern
{
    size_t         m_amount;  // amount of bytes in array
    PS_PatternByte **m_bytes; // array of bytes
} PS_Pattern;

typedef struct PS_PatternBatch
{
    size_t     m_amount;     // amount of patterns in array
    PS_Pattern **m_patterns; // array of patterns
} PS_PatternBatch;

/* funcs */

#ifdef __cplusplus
extern "C"
{
#endif

    // construct "PS_Pattern", must be freed with "ps_free_pattern"
    // returns NULL on failure
    PS_NOINLINE PS_Pattern *ps_make_pattern();

    // attempt to add a byte to a "PS_Pattern", byte must be created with "ps_make_pattern_byte"
    // returns false on failure
    PS_NOINLINE bool ps_add_pattern_byte( PS_Pattern *pattern, PS_PatternByte *byte );

    // free "PS_Pattern"
    PS_NOINLINE void ps_free_pattern( PS_Pattern *pattern );

    // construct "PS_PatternByte", must be freed with "ps_free_pattern"
    // returns NULL on failure
    PS_NOINLINE PS_PatternByte *ps_make_pattern_byte( uint8_t value, bool is_wildcard );

    // convert a code-style pattern string to a "PS_Pattern" type
    // returns NULL on failure
    PS_NOINLINE PS_Pattern *ps_build_codestyle( const wchar_t *pattern, const wchar_t *mask );

    // convert an IDA-style pattern string to a "PS_Pattern" type
    // returns NULL on failure
    PS_NOINLINE PS_Pattern *ps_build_idastyle( const wchar_t *pattern );

    // find pattern by "PS_Pattern"
    // must construct a "PS_Pattern" from "ps_make_pattern" and "ps_add_pattern_byte", free with "ps_free_pattern"
    // returns NULL on failure
    PS_NOINLINE uintptr_t ps_find_internal( uintptr_t start, size_t size, PS_Pattern *pattern );

    // find pattern by code-style pattern
    // example: "\xAA\xBB\xCC\x00\xDD", "xxx?x"
    // returns NULL on failure
    PS_NOINLINE uintptr_t ps_find_codestyle( uintptr_t start, size_t size, const wchar_t *pattern, const wchar_t *mask );

    // find pattern by IDA-style pattern
    // example: "AA B CC ? DD" (double question marks and single bytes are supported)
    // returns NULL on failure
    PS_NOINLINE uintptr_t ps_find_idastyle( uintptr_t start, size_t size, const wchar_t *pattern );

#ifdef __cplusplus
}
#endif