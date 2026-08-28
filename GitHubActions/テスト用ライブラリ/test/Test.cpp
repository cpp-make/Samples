#include "Point.hpp"
#include "doctest.h"

using namespace myLibrary; // NOLINT

TEST_CASE("lengthSq") {
  SUBCASE("3-4-5") {
    const Point p{3, 4};
    CHECK(p.lengthSq<int>() == 25);
  }
  SUBCASE("5-12-13") {
    const Point p{5, 12};
    CHECK(p.lengthSq<int>() == 169);
  }
}
