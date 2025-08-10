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
2. Obj → Obj
3. Arr → Arr
4. Str → Str
5. Bol → Bol
6. Num → Enum (rounded to nearest integer)
7. Num → Integer types (rounded)
8. Num → Floating-point types
9. Any → Types with Json constructors
10. Obj → Implicitly convertible types
11. Arr → Implicitly convertible types
12. Str → Implicitly convertible types
13. Num → Implicitly convertible types
14. Bol → Implicitly convertible types
15. Nul → Implicitly convertible types (except bool)
16. Obj → Convertible key-value types (element-wise)
17. Arr → Convertible container types (element-wise)
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

