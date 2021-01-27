#include <buffer/Buffer.h>
#include <catch2/catch.hpp>
#include <correlation/Correlation.h>

TEST_CASE ("CancellationTest", "[unit][correlation][cancellation]")
{
    Buffer lhs (1, 16, 44100.0);
    Buffer rhs (1, 16, 44100.0);

    SECTION ("Empty buffers has empty result")
    {
        auto result = Correlation::cancel (lhs, rhs);
        REQUIRE (result.getSampleRate() == 44100.0);
        REQUIRE (result.getNumChannels() == 1);
        REQUIRE (result.getNumSamples() == 16);

        for ( int i = 0; i < 16; ++i )
            REQUIRE (result.getSample (0, i) == 0.0f);
    }

    SECTION ("Buffers with different dimensions yield exception")
    {
        Buffer other1 (2, 16, 44100.0);
        Buffer other2 (1, 32, 44100.0);
        Buffer other3 (1, 16, 48000.0);
        Buffer other4 (2, 32, 48000.0);
        using Catch::Matchers::Contains;

        REQUIRE_THROWS_WITH (Correlation::cancel (lhs, other1), Contains ("Cancellation"));
        REQUIRE_THROWS_WITH (Correlation::cancel (lhs, other2), Contains ("Cancellation"));
        REQUIRE_THROWS_WITH (Correlation::cancel (lhs, other3), Contains ("Cancellation"));
        REQUIRE_THROWS_WITH (Correlation::cancel (lhs, other4), Contains ("Cancellation"));
    }

    SECTION ("Filled equal buffers yields empty result")
    {
        lhs.fill (1.0f);
        rhs.fill (1.0f);

        auto result = Correlation::cancel (lhs, rhs);

        for ( int i = 0; i < 16; ++i )
            REQUIRE (result.getSample (0, i) == 0.0f);
    }

    SECTION ("Different buffers yield filled result")
    {
        lhs.fill (1.0f);
        rhs.fill (-1.0f);

        auto result = Correlation::cancel (lhs, rhs);

        for ( int i = 0; i < 16; ++i )
            REQUIRE (result.getSample (0, i) == Approx (2.0f));
    }

    SECTION ("RMSE works (1)")
    {
        lhs.fill (1.0f);
        rhs.fill (1.0f);
        auto result = Correlation::rmse (lhs, rhs);
        REQUIRE (result == Approx (0.0).epsilon (0.00001));
    }

    SECTION ("RMSE works (2)")
    {
        lhs.fill (1.0f);
        rhs.fill (0.0f);
        auto result1 = Correlation::rmse (lhs, rhs);
        auto result2 = Correlation::rmse (rhs, lhs);
        REQUIRE (result1 == Approx (1.0));
        REQUIRE (result2 == Approx (1.0));
    }

    SECTION ("RMSE throws exception if dimensions are not equal")
    {
        Buffer other1 (2, 16, 44100.0);
        REQUIRE_THROWS (Correlation::rmse (lhs, other1));
    }
}
