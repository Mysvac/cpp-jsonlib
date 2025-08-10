# **Json.to_or**

```cpp
template<typename T, typename D = Nul>
requires convertible<Json, T> || convertible_map<Json, T, D> || convertible_array<Json, T, D>
T  to_or( T default_result, D default_range_elem = D{} ) const noexcept;
```

Converts internal data to type `T` (copy) and returns the provided default value on failure.

Extensive compile-time type checking ensures incompatible branches are eliminated during compilation.  
Note: Runtime conversion may still fail due to JSON's dynamic typing, despite template instantiation being valid.

Important: This function doesn't handle allocation failures or constructor exceptions - these will propagate normally.

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
18. Returns provided default value

## Parameters

- `T`: Target type
- `default_result`: Fallback value returned on conversion failure
- `default_range_elem`: Fallback value for failed element conversions (array/object only)

## Exception Safety

No-throw guarantee.

## Complexity

Linear time O(n) (full copy required)

Key characteristics:
- Never throws for type mismatches (uses default value)
- Still propagates memory/constructor exceptions
- Same conversion rules as other variants but with default fallback
- 
## Version

Since v3.0.0 .
