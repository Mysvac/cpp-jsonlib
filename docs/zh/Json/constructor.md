# **Json.constructor**

```cpp
// 1
constexpr Json() noexcept = default;

//  2
Json(const Json&) noexcept = default;

// 3
Json(Json&& other) noexcept;

// 4
template<typename T>
requires constructible<Json, std::remove_cvref_t<T>>
Json(T&& other) noexcept;

// 5
template<typename T>
requires !constructible<Json, std::remove_cvref_t<T>> && !constructible_map<Json, std::remove_cvref_t<T>> && constructible_array<Json, std::remove_cvref_t<T>>
explicit Json(T&& other) noexcept;

// 6
template<typename T>
requires !constructible<Json, std::remove_cvref_t<T>> && constructible_map<Json, std::remove_cvref_t<T>>
explicit Json(T&& other) noexcept;
```

1. Default constructor - initializes to `Nul`

2. Copy constructor - creates new `Json` object with copied value

3. Move constructor - transfers value to new `Json` object (source becomes `Nul`), noexcept

4. Implicit constructor - accepts any type convertible to JSON (arithmetic/enum→Num, JSON types, custom convertible types)
    - Noexcept guaranteed only for simple types or rvalue complex types

5. Explicit constructor - accepts array-like types (`std::vector`, `std::list` etc.) with constructible elements

6. Explicit constructor - accepts map-like types (`std::map`, `std::unordered_map` etc.) with constructible elements

## Exceptions

No-throw guarantee

## Complexity

1. O(1)
2. O(n)
3. O(1)
4. O(1) for simple types, O(n) for copyable types, O(1) for movable types
5. O(n)
6. O(n)

## Version

Since v3.0.0 .

