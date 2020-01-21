#include "includes.h"

static void print_scan_result( uintptr_t found, const std::vector< uint8_t > &bytes )
{
    if( !found || bytes.empty() )
    {
        return;
    }

    std::cout << std::setfill( '0' ) << std::setw( 2 ) << std::uppercase << std::hex;
    std::cout << "found: 0x" << found << std::endl << std::endl;

    std::cout << "[";

    const auto size = bytes.size();

    for( size_t i = 0; i < size; ++i )
    {
        std::cout << "0x" << (size_t)( *(uint8_t *)( found + i ) );

        if( i < ( size - 1 ) )
            std::cout << " ";
    }

    std::cout << "]" << std::endl << std::endl;
}

int __cdecl main( int argc, char **argv, char **envp )
{
    std::vector< uint8_t > bytes;

    static std::vector< uint8_t > pattern_bytes =
    {
        0x55, 0x48, 0x89, 0xE5, 0x89, 0x7D, 0xFC, 0x8B, 0x45, 0xFC, 0x0F, 0xAF, 0xC0, 0x5D, 0xC3
    };

    // generate some 'random' bytes
    for( uint16_t i = 1024; i--; )
    {
        bytes.push_back( Rand::get< uint8_t >() );
    }

    // place some pattern of bytes in the middle
    std::copy( pattern_bytes.begin(), pattern_bytes.end(), bytes.begin() + 512 );

    // IDA-style pattern
    const auto found_1 = ps_find_idastyle(
        "55 48 89 E5 89 ? ? 8B ? ? 0F AF C0 5D C3",
        (uintptr_t)( bytes.data() ),
        bytes.size()
    );

    // code-style pattern
    const auto found_2 = ps_find_codestyle(
        "\x55\x48\x89\xE5\x89\x00\x00\x8B\x00\x00\x0F\xAF\xC0\x5D\xC3",
        "xxxxx??x??xxxxx",
        (uintptr_t)( bytes.data() ),
        bytes.size()
    );

    // find multiple patterns in one pass
    // PS_PatternBatches batch;
    //
    // uintptr_t batch_found_1 = 0;
    // uintptr_t batch_found_2 = 0;
    //
    // ps_add_idastyle_batch( &batch, &batch_found_1, "AA BB CC ? DD" );
    // ps_add_idastyle_batch( &batch, &batch_found_2, "AA BB CC ? DD" );
    //
    // ps_find_batch( bytes.size(), bytes.size(), &batch );

    // print all results
    print_scan_result( found_1, pattern_bytes );
    print_scan_result( found_2, pattern_bytes );

    std::cout << "Press any key to continue...";
    _getwch();

    return 1;
}