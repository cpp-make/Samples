#pragma once
#include <cmath>
#include <format>
#include <iostream>

namespace myLibrary {
/// @brief 二次元座標
struct Point {
  /// @brief X 成分
  int x;

  /// @brief Y 成分
  int y;

  /// @brief デフォルトコンストラクタ。成分を 0 で初期化します。
  [[nodiscard]] constexpr Point() noexcept : x{0}, y{0} {}

  /// @brief 二次元座標を作成します。
  /// @param _x X 成分
  /// @param _y Y 成分
  [[nodiscard]] constexpr Point(int _x, int _y) noexcept : x{_x}, y{_y} {}

  /// @brief 2 つの二次元座標が等しいかを返します。
  /// @param lhs 一方の二次元座標
  /// @param rhs もう一方の二次元座標
  /// @return 2 つの二次元座標が等しい場合 true, それ以外の場合は false
  [[nodiscard]] friend constexpr bool
  operator==(const Point &lhs, const Point &rhs) noexcept = default;

  /// @brief 単項プラス演算子
  /// @return 自身の値
  [[nodiscard]] constexpr Point operator+() const noexcept { return *this; }

  /// @brief 加算演算子
  /// @param p 加算する二次元座標
  /// @return { (x + p.x), (y + p.y) }
  [[nodiscard]] constexpr Point operator+(const Point &p) const noexcept {
    return {(x + p.x), (y + p.y)};
  }

  /// @brief 単項マイナス演算子
  /// @return { -x, -y }
  [[nodiscard]] constexpr Point operator-() const noexcept { return {-x, -y}; }

  /// @brief 減算演算子
  /// @param p 減算する二次元座標
  /// @return { (x - p.x), (y - p.y) }
  [[nodiscard]] constexpr Point operator-(const Point &p) const noexcept {
    return {(x - p.x), (y - p.y)};
  }

  /// @brief 乗算演算子
  /// @param s 乗算するスカラー値
  /// @return { (x * s), (y * s) }
  [[nodiscard]] constexpr Point operator*(int s) const noexcept {
    return {(x * s), (y * s)};
  }

  /// @brief 乗算演算子（左辺がスカラー値）
  /// @param s 乗算するスカラー値
  /// @param p 乗算される二次元座標
  /// @return { (s * p.x), (s * p.y) }
  [[nodiscard]] friend constexpr Point operator*(int s,
                                                 const Point &p) noexcept {
    return {(s * p.x), (s * p.y)};
  }

  /// @brief 除算演算子
  /// @param s 除算するスカラー値
  /// @return { (x / s), (y / s) }
  [[nodiscard]] constexpr Point operator/(int s) const noexcept {
    return {(x / s), (y / s)};
  }

  /// @brief 加算代入演算子
  /// @param p 加算する二次元座標
  /// @return *this
  constexpr Point &operator+=(const Point &p) noexcept {
    x += p.x;
    y += p.y;
    return *this;
  }

  /// @brief 減算代入演算子
  /// @param p 減算する二次元座標
  /// @return *this
  constexpr Point &operator-=(const Point &p) noexcept {
    x -= p.x;
    y -= p.y;
    return *this;
  }

  /// @brief 乗算代入演算子
  /// @param s 乗算するスカラー値
  /// @return *this
  constexpr Point &operator*=(int s) noexcept {
    x *= s;
    y *= s;
    return *this;
  }

  /// @brief 除算代入演算子
  /// @param s 除算するスカラー値
  /// @return *this
  constexpr Point &operator/=(int s) noexcept {
    x /= s;
    y /= s;
    return *this;
  }

  /// @brief 成分がすべて 0 であるかを返します。
  /// @return 成分がすべて 0 である場合 true, それ以外の場合は false
  [[nodiscard]] constexpr bool isZero() const noexcept {
    return ((x == 0) && (y == 0));
  }

  /// @brief 成分をすべて 0 にします。
  constexpr void clear() noexcept {
    x = 0;
    y = 0;
  }

  /// @brief 成分を設定します。
  /// @param _x 新しい X 成分
  /// @param _y 新しい Y 成分
  /// @return *this
  constexpr Point &set(int _x, int _y) noexcept {
    x = _x;
    y = _y;
    return *this;
  }

  /// @brief 成分を設定します。
  /// @param p 新しい成分
  /// @return *this
  constexpr Point &set(const Point &p) noexcept {
    x = p.x;
    y = p.y;
    return *this;
  }

  /// @brief 原点からの距離を返します。
  /// @tparam Type 距離の型
  /// @return 原点からの距離
  template <class Type = double> [[nodiscard]] Type length() const noexcept {
    return std::sqrt(lengthSq<Type>());
  }

  /// @brief 原点からの距離の 2 乗を返します。
  /// @tparam Type 距離の型
  /// @return 原点からの距離の 2 乗
  template <class Type = double>
  [[nodiscard]] constexpr Type lengthSq() const noexcept {
    const Type x_ = static_cast<Type>(x);
    const Type y_ = static_cast<Type>(y);
    return ((x_ * x_) + (y_ * y_));
  }

  /// @brief 原点からのマンハッタン距離を返します。
  /// @return 原点からのマンハッタン距離
  [[nodiscard]] constexpr long long manhattanDistance() const noexcept {
    const long long x_ = (x < 0) ? -x : x;
    const long long y_ = (y < 0) ? -y : y;
    return (x_ + y_);
  }

  /// @brief { x, x } を返します。
  /// @return { x, x }
  [[nodiscard]] constexpr Point xx() const noexcept { return {x, x}; }

  /// @brief { y, y } を返します。
  /// @return { y, y }
  [[nodiscard]] constexpr Point yy() const noexcept { return {y, y}; }

  /// @brief { x, y } を返します。
  /// @return { x, y }
  [[nodiscard]] constexpr Point xy() const noexcept { return *this; }

  /// @brief { y, x } を返します。
  /// @return { y, x }
  [[nodiscard]] constexpr Point yx() const noexcept { return {y, x}; }

  /// @brief { 0, 0 } を返します。
  /// @return { 0, 0 }
  [[nodiscard]] static constexpr Point Zero() noexcept { return {0, 0}; }

  /// @brief { 1, 1 } を返します。
  /// @return { 1, 1 }
  [[nodiscard]] static constexpr Point One() noexcept { return {1, 1}; }

  /// @brief { 1, 0 } を返します。
  /// @return { 1, 0 }
  [[nodiscard]] static constexpr Point UnitX() noexcept { return {1, 0}; }

  /// @brief { 0, 1 } を返します。
  /// @return { 0, 1 }
  [[nodiscard]] static constexpr Point UnitY() noexcept { return {0, 1}; }

  /// @brief { value, value } を返します。
  /// @param value 成分の値
  /// @return { value, value }
  [[nodiscard]] static constexpr Point All(int value = 1) noexcept {
    return {value, value};
  }

  /// @brief 出力ストリームに書き込みます。
  /// @param os 出力ストリーム
  /// @param p 書き込む値
  /// @return 出力ストリーム
  friend std::ostream &operator<<(std::ostream &os, const Point &p) {
    return os << '(' << p.x << ", " << p.y << ')';
  }

  /// @brief 入力ストリームから読み込みます。
  /// @param is 入力ストリーム
  /// @param p 読み込んだ値の格納先
  /// @return 入力ストリーム
  friend std::istream &operator>>(std::istream &is, Point &p) {
    char t;
    return is >> t >> p.x >> t >> p.y >> t;
  }
};
} // namespace myLibrary

/// @brief Point 型を std::format に対応させるための std::formatter 特殊化
template <> struct std::formatter<myLibrary::Point> {
  template <class ParseContext> constexpr auto parse(ParseContext &ctx) {
    return ctx.begin();
  }

  template <class FormatContext>
  auto format(const myLibrary::Point &p, FormatContext &ctx) const {
    return std::format_to(ctx.out(), "({}, {})", p.x, p.y);
  }
};
