# **Json.move**

```cpp
template<typename T, typename D = Nul>
requires convertible<T> || convertible_map<T, D> || convertible_array<T, D>
[[nodiscard]]
T  move( D default_range_elem = D{} );
```

Attempts to move-convert internal data to type `T`, throwing on failure.

Extensive compile-time checks eliminate incompatible conversions during compilation.  
Runtime conversion may still fail due to JSON's dynamic typing nature.

Warning: Successful move operations will leave the source object empty, even if the result isn't used.  
Recommended to reset source object after moving.

## Move Conversion Rules

1. Nul → Nul
2. Object → Object (moved)
3. Array → Array (moved)
4. String → String (moved)
5. Bool → Bool (copied)
6. Number → Enum (rounded)
7. Number → Integer (rounded)
8. Number → Floating-point
9. Any → Json-constructible types (move preferred)
10. Object → Implicitly convertible (move preferred)
11. Array → Implicitly convertible (move preferred)
12. String → Implicitly convertible (move preferred)
13. Number → Implicitly convertible
14. Bool → Implicitly convertible
15. Nul → Implicitly convertible (except bool)
16. Object → Convertible key-value types (element-wise, move preferred)
17. Array → Convertible container types (element-wise, move preferred)
18. Throws std::runtime_error

## Parameters

- `T`: Target type
- `default_range_elem`: Fallback for failed element conversions (array/object only)

## Exception Safety

Throws on Move construction failures.

## Complexity

- O(1) for movable types
- O(n) for element-wise conversions

Key Notes:
- Source object becomes Nul after successful move
- Prefers move semantics where possible
- Numeric/bool conversions remain copy operations

## Version

Since v3.0.0 .
