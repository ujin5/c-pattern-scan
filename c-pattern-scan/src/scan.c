#include "includes.h"
#include "c-pattern-scan/scan.h"
#include "utils.h"

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

PS_NOINLINE bool ps_add_pattern_byte( PS_Pattern *pattern, PS_PatternByte *byte )
{
    if( pattern && byte )
    {
        // save off last amount and increment total
        uint8_t amount = pattern->m_amount++;

        // make more room if needed and add a new byte
        PS_PatternByte **bytes = (PS_PatternByte **)( realloc( pattern->m_bytes, sizeof( PS_PatternByte * ) * ( (size_t)amount + 1 ) ) );
        if( bytes )
        {
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
    }
}

PS_NOINLINE bool ps_build_codestyle( PS_Pattern *out_pattern, const wchar_t *pattern, const wchar_t *mask )
{
    if( pattern && mask )
    {

    }

    return false;
}

PS_NOINLINE bool ps_build_idastyle( PS_Pattern *out_pattern, const wchar_t *pattern )
{
    if( out_pattern && pattern )
    {
        size_t ptrn_len = wcslen( pattern );
        if( ptrn_len )
        {
            // make a copy of the input string
            wchar_t *str = _wcsdup( pattern );

            // clean up the input string
            str = util_trim_pattern_wstr( str );
            if( str )
            {
                bool       valid       = true;
                wchar_t    *next_token = 0;
                PS_Pattern out         = { 0 };

                // iteratively split the input string by space
                for(
                    wchar_t *token = wcstok_s( str, L" ", &next_token );
                    token;
                    token = wcstok_s( 0, L" ", &next_token )
                )
                {
                    PS_PatternByte *byte;

                    // make sure tokens aren't too long
                    size_t token_len = wcslen( token );
                    if( !token_len || token_len > 2 )
                    {
                        ps_free_pattern( &out );
                        valid = false;

                        break;
                    }

                    if( token[ 0 ] == L'?' ) // wildcard
                    {
                        // add a wildcard byte to array
                        byte = ps_make_pattern_byte( 0, true );
                        if( !byte )
                        {
                            ps_free_pattern( &out );
                            valid = false;

                            break;
                        }

                        if( !ps_add_pattern_byte( &out, byte ) )
                        {
                            ps_free_pattern( &out );
                            valid = false;

                            break;
                        }
                    }
                    else // probably a byte
                    {
                        // ensure the current token is a valid byte (should only be 1 or 2 chars and valid hex digit(s))
                        bool is_valid_byte = false;

                        if( token_len == 1 && iswxdigit( token[ 0 ] ) ) // 1-char
                            is_valid_byte = true;
                        else if( token_len == 2 && iswxdigit( token[ 0 ] ) && iswxdigit( token[ 1 ] ) ) // 2-char
                            is_valid_byte = true;

                        if( !is_valid_byte )
                        {
                            ps_free_pattern( &out );
                            valid = false;

                            break;
                        }

                        // attempt to convert the string to a base 16 int and make sure it's in bounds of a uint8_t
                        ulong_t val = wcstoul( token, 0, 16 );
                        if( !val || val == ULONG_MAX || val > UINT8_MAX )
                        {
                            ps_free_pattern( &out );
                            valid = false;

                            break;
                        }

                        // add normal byte to array
                        byte = ps_make_pattern_byte( (uint8_t)val, false );
                        if( !byte )
                        {
                            ps_free_pattern( &out );
                            valid = false;

                            break;
                        }

                        if( !ps_add_pattern_byte( &out, byte ) )
                        {
                            ps_free_pattern( &out );
                            valid = false;

                            break;
                        }
                    }
                }

                free( str );

                if( valid )
                {
                    *out_pattern = out;
                    return true;
                }
            }
        }
    }

    return false;
}

PS_NOINLINE uintptr_t ps_find_internal( uintptr_t start, size_t size, PS_Pattern *pattern )
{
    if( start && size && pattern && pattern->m_bytes && pattern->m_amount )
    {
        uintptr_t found   = 0;
        uint8_t   matched = 0;

        // calculate scan end
        uintptr_t end = start + size;

        // iterate over the range of memory
        for( uintptr_t i = start; i < end; ++i )
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
        PS_Pattern ptrn;

        if( ps_build_codestyle( &ptrn, pattern, mask ) )
        {
            return ps_find_internal( start, size, &ptrn );
        }
    }

    return 0;
}

PS_NOINLINE uintptr_t ps_find_idastyle( uintptr_t start, size_t size, const wchar_t *pattern )
{
    if( start && size )
    {
        PS_Pattern ptrn;

        if( ps_build_idastyle( &ptrn, pattern ) )
        {
            return ps_find_internal( start, size, &ptrn );
        }
    }

    return 0;
}

PS_NOINLINE void ps_free_pattern_batch( PS_PatternBatches *batch )
{
    if( batch )
    {

    }
}

PS_NOINLINE void ps_add_idastyle_batch( PS_PatternBatches *batch, uintptr_t *found, const wchar_t *pattern )
{
    if( batch && found )
    {

    }
}

PS_NOINLINE void ps_find_batch( uintptr_t start, size_t size, PS_PatternBatches *batch )
{

}