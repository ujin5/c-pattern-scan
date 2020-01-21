#include "utils.h"

PS_NOINLINE wchar_t *util_trim_pattern_wstr( wchar_t *str )
{
    if( str )
    {
        size_t len = wcslen( str );
        if( len )
        {
            wchar_t *start = str;
            wchar_t *end   = ( start + len ) - 1;

            // move start ptr up
            for( size_t i = 0; i < len; ++i )
            {
                if( iswspace( *start ) || *start == L'?' )
                    ++start;
            }

            // move end ptr down
            for( size_t i = len; len--; )
            {
                if( iswspace( *end ) || *end == L'?' )
                    --end;
            }

            // add null terminator
            *( end + 1 ) = L'\0';

            // shift string bytes, this must be done in case the string is dynamically allocated and the original ptr isn't changed
            size_t new_len = wcslen( start );
            if( new_len )
            {
                return memmove( str, start, ( new_len * sizeof( wchar_t ) ) + 1 );
            }
        }
    }

    return 0;
}