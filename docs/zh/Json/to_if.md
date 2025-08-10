# **Json.to_if**

```cpp
template<typename T, typename D = Nul>
requires convertible<Json, T> || convertible_map<Json, T, D> || convertible_array<Json, T, D>
std::optional<T>  to_if( D default_range_elem = D{} ) const noexcept;
```

Converts internal data to type `T` (copy) and returns `std::optional<T>`. Returns `std::nullopt` on conversion failure.

Extensive compile-time type checking ensures incompatible branches are eliminated during compilation.  
Note: Runtime conversion may still fail due to JSON's dynamic typing, despite template instantiation being valid.

Important: This function doesn't handle exceptions - allocation errors or constructor exceptions will propagate through.

## Conversion Rules

1. Nul → Nul
2. Object → Object
3. Array → Array
4. String → String
5. Bool → Bool
6. Number → Enum (rounded to nearest integer)
7. Number → Integer types (rounded)
8. Number → Floating-point types
9. Any → Types with Json constructors
10. Object → Implicitly convertible types
11. Array → Implicitly convertible types
12. String → Implicitly convertible types
13. Number → Implicitly convertible types
14. Bool → Implicitly convertible types
15. Nul → Implicitly convertible types (except bool)
16. Object → Convertible key-value types (element-wise)
17. Array → Convertible container types (element-wise)
18. Returns `std::nullopt`

## Parameters

- `T`: Target type
- `default_range_elem`: Fallback value for failed element conversions (array/object only)

## Exception Safety

No-throw guarantee.

## Complexity

Linear time O(n) (full copy required)

Key differences from throwing version:
- Uses `optional` instead of exceptions for type failures
- Still propagates allocation/constructor exceptions
- Same conversion rules but returns nullopt instead of throwing runtime_error

## Version

Since v3.0.0 .

