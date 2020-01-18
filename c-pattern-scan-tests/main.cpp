#include "includes.h"

int __cdecl wmain( int argc, wchar_t **argv, wchar_t **envp )
{
    std::vector< uint8_t > bytes;

    static std::array< uint8_t, 15 > pattern_bytes = {
        0x55, 0x48, 0x89, 0xE5, 0x89, 0x7D, 0xFC, 0x8B, 0x45, 0xFC, 0x0F, 0xAF, 0xC0, 0x5D, 0xC3
    };

    // generate some 'random' bytes
    for( uint16_t i = 1024; i--; )
    {
        bytes.push_back( Rand::get< uint8_t >() );
    }

    // place some pattern of bytes in the middle
    std::copy( pattern_bytes.begin(), pattern_bytes.end(), bytes.begin() + 512 );

    // scan for them
    const auto found = ps_find_idastyle( (uintptr_t)( bytes.data() ), bytes.size(), L"??? ?  ?  ?55 48 89 E5 89 ? ? 8B ? ? 0F AF C0 5D C3? ?  ?   ? ?? ???" );

    // print result
    std::wcout << std::uppercase << std::hex << std::setfill( L'0' ) << std::setw( 2 );
    std::wcout << L"found: 0x" << found << std::endl << std::endl;

    std::wcout << L"[";

    const auto size = pattern_bytes.size();

    for( size_t i = 0; i < size; ++i )
    {
        std::wcout << L"0x" << *(uint8_t *)( found + i );

        if( i < ( size - 1 ) )
            std::wcout << " ";
    }

    std::wcout << L"]" << std::endl << std::endl;

    std::wcout << L"Press any key to continue...";
    _getwch();

    return 1;
}