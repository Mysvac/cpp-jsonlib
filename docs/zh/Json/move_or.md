# **Json.move_or**

```cpp
template<typename T, typename D = Nul>
requires convertible<Json, T> || convertible_map<Json, T, D> || convertible_array<Json, T, D>
T  move_or( T default_result, D default_range_elem = D{} ) noexcept;
```

Performs move-conversion of internal data to type `T`, returning specified default value on failure.

**Key Features**:
- Full compile-time type checking (incompatible types rejected during compilation)
- Runtime type safety (may still fail due to JSON's dynamic nature)
- Source object becomes Nul after successful move (even if return value is unused)

**Conversion Rules**:
1. Nul → Nul
2. Object → Object (move)
3. Array → Array (move)
4. String → String (move)
5. Bool → Bool (copy)
6. Number → Enum (rounded)
7. Number → Integer (rounded)
8. Number → Floating-point
9. Any → Json-constructible (move preferred)
10. Object → Implicitly convertible (move preferred)
11. Array → Implicitly convertible (move preferred)
12. String → Implicitly convertible (move preferred)
    13-15. Primitive → Implicitly convertible (Nul excludes bool)
16. Object → Key-value container (element-wise, move preferred)
17. Array → Sequence container (element-wise, move preferred)
18. Returns default_result

**Parameters**:
- `T`: Target type
- `default_result`: Fallback return value
- `default_range_elem`: Element-level fallback (array/object only)

## Exception Safety

No-throw guarantee.

**Complexity**:
- O(1) for movable types
- O(n) for element-wise conversions

**Best Practice**:
- Explicitly reset source object after move
- Prefer move semantics for complex types
- Use default_result for safe fallback handling

## Version

Since v3.0.0 .
