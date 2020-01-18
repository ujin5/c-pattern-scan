#include "includes.h"
#include "c-pattern-scan/scan.h"
#include "utils.h"

PS_NOINLINE PS_Pattern *ps_make_pattern()
{
    PS_Pattern *ptrn = (PS_Pattern *)( malloc( sizeof( PS_Pattern ) ) );
    if( ptrn )
    {
        ptrn->m_amount = 0;
        ptrn->m_bytes  = 0;

        return ptrn;
    }

    return 0;
}

PS_NOINLINE bool ps_add_pattern_byte( PS_Pattern *pattern, PS_PatternByte *byte )
{
    if( pattern && byte )
    {
        // save off last amount and increment
        uint8_t amount = pattern->m_amount++;

        // make more room if needed
        PS_PatternByte **bytes = (PS_PatternByte **)( realloc( pattern->m_bytes, sizeof( PS_PatternByte * ) * ( (size_t)amount + 1 ) ) );
        if( bytes )
        {
            // add new byte
            pattern->m_bytes           = bytes;
            pattern->m_bytes[ amount ] = byte;

            return true;
        }
    }

    return false;
}

PS_NOINLINE void ps_free_pattern( PS_Pattern *pattern )
{
    if( pattern )
    {
        if( pattern->m_bytes )
        {
            // free each byte
            for( uint8_t i = 0; i < pattern->m_amount; ++i )
            {
                PS_PatternByte *byte = pattern->m_bytes[ i ];
                if( byte )
                {
                    free( byte );
                }
            }

            free( pattern->m_bytes );
        }

        pattern->m_amount = 0;
        pattern->m_bytes  = 0;
    }
}

PS_NOINLINE PS_PatternByte *ps_make_pattern_byte( uint8_t value, bool is_wildcard )
{
    PS_PatternByte *byte = (PS_PatternByte *)( malloc( sizeof( PS_PatternByte ) ) );
    if( byte )
    {
        byte->m_value       = value;
        byte->m_is_wildcard = is_wildcard;

        return byte;
    }

    return 0;
}

PS_NOINLINE PS_Pattern *ps_build_codestyle( const wchar_t *pattern, const wchar_t *mask )
{
    PS_Pattern *ptrn = ps_make_pattern();
    if( ptrn )
    {
        // TODO:
    }

    return 0;
}

PS_NOINLINE PS_Pattern *ps_build_idastyle( const wchar_t *pattern )
{
    if( pattern )
    {
        size_t ptrnlen = wcslen( pattern );
        if( ptrnlen )
        {
            PS_Pattern *ptrn = ps_make_pattern();
            if( ptrn )
            {
                // make a copy of the input string
                wchar_t *str = _wcsdup( pattern );

                // clean up input string
                str = util_trim_pattern_wstr( str );
                if( str )
                {
                    wchar_t *next_token = 0;

                    // split pattern string by space and iteratively check token
                    for(
                        wchar_t *token = wcstok_s( str, L" ", &next_token );
                        token;
                        token = wcstok_s( 0, L" ", &next_token )
                    )
                    {
                        PS_PatternByte *byte;

                        if( token[ 0 ] == L'?' ) // wildcard
                        {
                            // add wildcard byte to array
                            byte = ps_make_pattern_byte( 0, true );
                            if( !byte )
                            {
                                ps_free_pattern( ptrn );
                                return 0;
                            }

                            if( !ps_add_pattern_byte( ptrn, byte ) )
                            {
                                ps_free_pattern( ptrn );
                                return 0;
                            }
                        }
                        else // probably a byte
                        {
                            bool is_vald_byte = false;
                            size_t tknlen     = wcslen( token );

                            // ensure current token is a valid byte
                            {
                                if( tknlen == 1 && iswxdigit( token[ 0 ] ) ) // 1-char
                                    is_vald_byte = true;
                                else if( tknlen == 2 && iswxdigit( token[ 0 ] ) && iswxdigit( token[ 1 ] ) ) // 2-char
                                    is_vald_byte = true;
                            }

                            if( !is_vald_byte )
                            {
                                ps_free_pattern( ptrn );
                                return 0;
                            }

                            // attempt to convert the string to a base 16 int and make sure it's still in bounds of a uint8_t
                            ulong_t val = wcstoul( token, 0, 16 );
                            if( !val || val == ULONG_MAX || val > UINT8_MAX )
                            {
                                ps_free_pattern( ptrn );
                                return 0;
                            }

                            // add normal byte to array
                            byte = ps_make_pattern_byte( (uint8_t)val, false );
                            if( !byte )
                            {
                                ps_free_pattern( ptrn );
                                return 0;
                            }

                            if( !ps_add_pattern_byte( ptrn, byte ) )
                            {
                                ps_free_pattern( ptrn );
                                return 0;
                            }
                        }
                    }

                    free( str );
                }

                return ptrn;
            }
        }
    }

    return 0;
}

PS_NOINLINE uintptr_t ps_find_internal( uintptr_t start, size_t size, PS_Pattern *pattern )
{
    if( start && size && pattern && pattern->m_bytes && pattern->m_amount )
    {
        uintptr_t found   = 0;
        uint8_t   matched = 0;

        // move end down... we can't scan outsize of the pattern length
        uintptr_t new_end = ( start + size ) - pattern->m_amount;

        // iterate over range of memory
        for( uintptr_t i = start; i < new_end; ++i )
        {
            if( pattern->m_bytes[ matched ]->m_is_wildcard ) // wildcards are always a 'match'
            {
                ++matched;
            }
            else // check byte value
            {
                uint8_t value = *(uint8_t *)i;
                if( value == pattern->m_bytes[ matched ]->m_value )
                {
                    // save the first position
                    if( !matched )
                        found = i;

                    ++matched;
                }
                else // didn't match
                {
                    if( matched > 0 )
                        --matched;
                }
            }

            // are we done? return the address to the first match
            if( matched == pattern->m_amount )
                return found;
        }
    }

    return 0;
}

PS_NOINLINE uintptr_t ps_find_codestyle( uintptr_t start, size_t size, const wchar_t *pattern, const wchar_t *mask )
{
    if( start && size )
    {
        PS_Pattern *ptrn = ps_build_codestyle( pattern, mask );
        if( ptrn )
        {
            uintptr_t found = ps_find_internal( start, size, ptrn );

            ps_free_pattern( ptrn );

            return found;
        }
    }

    return 0;
}

PS_NOINLINE uintptr_t ps_find_idastyle( uintptr_t start, size_t size, const wchar_t *pattern )
{
    if( start && size )
    {
        PS_Pattern *ptrn = ps_build_idastyle( pattern );
        if( ptrn )
        {
            uintptr_t found = ps_find_internal( start, size, ptrn );

            ps_free_pattern( ptrn );

            return found;
        }
    }

    return 0;
}