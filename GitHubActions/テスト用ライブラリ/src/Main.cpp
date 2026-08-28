#include "Point.hpp"
#include <iostream>

using namespace myLibrary; // NOLINT

int main() {
  const Point p1{1, 2};
  const Point p2{3, 4};

  std::cout << std::boolalpha;
  std::cout << ((p1 + p2) == Point{4, 6}) << '\n';
  std::cout << (p2.length() == 5.0) << '\n';
  std::cout << (p2.manhattanDistance() == 7) << '\n';
  std::cout << (Point::All(3) == Point{3, 3}) << '\n';
}
