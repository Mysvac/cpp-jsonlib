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
2. Obj → Obj (moved)
3. Arr → Arr (moved)
4. Str → Str (moved)
5. Bol → Bol (copied)
6. Num → Enum (rounded)
7. Num → Integer (rounded)
8. Num → Floating-point
9. Any → Json-constructible types (move preferred)
10. Obj → Implicitly convertible (move preferred)
11. Arr → Implicitly convertible (move preferred)
12. Str → Implicitly convertible (move preferred)
13. Num → Implicitly convertible
14. Bol → Implicitly convertible
15. Nul → Implicitly convertible (except bool)
16. Obj → Convertible key-value types (element-wise, move preferred)
17. Arr → Convertible container types (element-wise, move preferred)
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
