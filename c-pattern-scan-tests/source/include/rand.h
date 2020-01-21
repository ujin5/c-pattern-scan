#pragma once

/* Rand implementation */

namespace Rand
{

    /* PRNG implementation details */

    namespace Impl
    {

        /* implementation utils */

        // true if T is in Ts
        template< class T, class... Ts > inline constexpr bool is_any_of =
            std::disjunction_v< std::is_same< T, Ts >... >;

        // true if T is an 8-bit type
        template< class T > inline constexpr bool is_byte_type =
            is_any_of< T, char,
                          unsigned char >;

        // always false for Ts
        template< class... Ts > inline constexpr bool always_false = false;

    }

    /* utils */

    // returns the current mt19937 engine
    static NOINLINE std::mt19937 &get_rng_engine() noexcept
    {
        static std::mt19937 mt;
        static auto         once = false;

        if( !once )
        {
            std::array< std::mt19937::result_type, std::mt19937::state_size > seeds;
            std::random_device                                                rd;

            // generate enough seeds to fill the entire state
            std::generate( seeds.begin(), seeds.end(), std::ref( rd ) );

            // and finally, seed the PRNG
            std::seed_seq seq( seeds.begin(), seeds.end() );

            mt.seed( seq );

            once = true;
        }

        return mt;
    }

    /* run-time PRNG using Mersenne Twister */

    // bad type for get, should never call this
    template< class T, std::enable_if_t< !Impl::is_byte_type< T >, int > = 0 >
    NORETURN inline constexpr T get( MAYBE_UNUSED T min = { 0 }, MAYBE_UNUSED T max = { 0 } ) noexcept
    {
        static_assert( Impl::always_false< T >,
            "invalid template argument for Rand::get: requires one of "
            "char, unsigned char"
        );
    }

    // generate random uniform byte in range
    template< class T, std::enable_if_t< Impl::is_byte_type< T >, int > = 0 >
    static NOINLINE T get( T min = 0, T max = std::numeric_limits< T >::max() ) noexcept
    {
        // check what short type to use
        using ST = std::conditional_t< std::is_signed_v< T >, short, unsigned short >;

        std::uniform_int_distribution< ST > dist( min, max );

        return (T)( dist( get_rng_engine() ) );
    }

} // namespaced Rand