# **Json.move_if**

```cpp
template<typename T, typename D = Nul>
requires convertible<Json, T> || convertible_map<Json, T, D> || convertible_array<Json, T, D>
std::optional<T>  move_if( D default_range_elem = D{} ) noexcept;
```

Attempts to move-convert internal data to type `T`, returning `std::nullopt` on failure.

Extensive compile-time checks eliminate incompatible conversions during compilation.  
Runtime conversion may still fail due to JSON's dynamic typing nature.

Warning: Successful move operations will leave the source object empty (set to Nul), even if the result isn't used.  
Recommended practice: Reset source object after moving.

## Move Conversion Rules

1. Nul → Nul
2. Object → Object (moved)
3. Array → Array (moved)
4. Str → Str (moved)
5. Bol → Bol (copied)
6. Num → Enum (rounded to nearest integer)
7. Num → Integer types (rounded)
8. Num → Floating-point
9. Any → Json-constructible types (move preferred)
10. Object → Implicitly convertible (move preferred)
11. Array → Implicitly convertible (move preferred)
12. Str → Implicitly convertible (move preferred)
13. Num → Implicitly convertible
14. Bol → Implicitly convertible
15. Nul → Implicitly convertible (except bool)
16. Object → Convertible key-value types (element-wise, move preferred)
17. Array → Convertible container types (element-wise, move preferred)
18. Returns `std::nullopt`

## Parameters

- `T`: Target type
- `default_range_elem`: Fallback for failed element conversions (array/object only)

## Exception Safety

No-throw guarantee.

## Complexity

- O(1) for movable types
- O(n) for element-wise conversions

Key characteristics:
- Never throws for type mismatches (uses nullopt)
- Source object becomes Nul after move
- Preserves move semantics optimization opportunities

## Version

Since v3.0.0 .