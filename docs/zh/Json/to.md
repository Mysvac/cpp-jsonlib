# **Json.to**

```cpp
template<typename T, typename D = Nul>
requires convertible<Json, T> || convertible_map<Json, T, D> || convertible_array<Json, T, D>
T  to( D default_range_elem = D{} ) const;
```

Performs a type conversion (copy) of internal data to type `T`, throwing an exception if conversion fails.

Extensive compile-time type checking is performed - incompatible branches are eliminated during compilation.  
However, conversion may still fail at runtime due to JSON's dynamic typing nature.

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
18. Throws std::runtime_error

## Parameters

- `T`: Target type
- `default_range_elem`: Fallback value for failed element conversions (array/object only)

## Exception Safety

Throws on conversion failure

## Complexity

Linear time O(n) (full copy required)

## Version

Since v3.0.0 .
