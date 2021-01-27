#include <catch2/catch.hpp>
#include <util/Random.h>
TEST_CASE ("Random test", "[util][unit][random]")
{
    Random r (0);
    REQUIRE (r.next (1, 1) == 1);
    REQUIRE (r.next (1, 2) == 1);
    REQUIRE (r.next (1, 2) == 1);
    REQUIRE (r.next (1, 2) == 1);
    REQUIRE (r.next (1, 2) == 1);
    REQUIRE (r.next (1, 2) == 1);
    REQUIRE (r.next (1, 2) == 1);
    REQUIRE (r.next (1, 2) == 1);
    REQUIRE (r.next (1, 2) == 2);
    REQUIRE (r.next (1, 2) == 1);
    REQUIRE (r.next (1, 2) == 1);
    REQUIRE (r.next (1, 2) == 2);

    REQUIRE (r.next (1.0, 1.0) == Approx (1.0));
    REQUIRE (r.next (1.0, 2.0) == Approx (1.5680596232));
    REQUIRE (r.next (1.0, 2.0) == Approx (1.5560943484));

    std::vector<int> vec1 = {42};
    REQUIRE (r.nextFrom (vec1) == 42);

    std::vector<double> vec2 = {42.0};
    REQUIRE (r.nextFrom (vec2) == Approx (42.0));

    std::vector<int> vec3 = {1, 2, 3};
    REQUIRE (r.nextFrom (vec3) == 1);
    REQUIRE (r.nextFrom (vec3) == 3);
    REQUIRE (r.nextFrom (vec3) == 1);
    REQUIRE (r.nextFrom (vec3) == 1);
    REQUIRE (r.nextFrom (vec3) == 1);
    REQUIRE (r.nextFrom (vec3) == 3);
    REQUIRE (r.nextFrom (vec3) == 2);
    REQUIRE (r.nextFrom (vec3) == 3);
    REQUIRE (r.nextFrom (vec3) == 2);
    REQUIRE (r.nextFrom (vec3) == 3);
}