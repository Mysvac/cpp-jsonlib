# **Json.operator=**

```cpp
// 1
bool operator==(const Json& other) const noexcept;

// 2
template<typename  T>
requires (!std::is_same_v<T, Json>)
bool operator==(const T& other) const noexcept;
```

### Comparison Operations

1. **Same-type comparison** (between two `Json` objects): Returns `true` if equal, `false` otherwise.
2. **Cross-type comparison** (between `Json` and any other type): Returns `true` if conversion and comparison succeed, otherwise `false`.

#### Comparison Rules

**1. Same-type (`Json` vs `Json`)**
- First checks if internal data types match.
- If types match, delegates to the underlying type's comparison:
    - `std::vector`/`std::map` comparisons are recursive.
    - Floating-point comparisons are **strict** (no epsilon tolerance).

**2. Cross-type (`Json` vs `T`)**
- Only invoked if `T` is not `Json` (otherwise, delegates to same-type comparison).
- Rules applied in order:
    1. **JSON-native types**: Directly compares internal data (returns `false` if types differ).
    2. **Integer/enum types**:
        - Returns `false` if not a `Num`.
        - Otherwise, rounds to nearest integer and compares.
    3. **Floating-point types**:
        - Returns `false` if not a `Num`.
        - Otherwise, performs exact comparison.
    4. **String-compatible types**:
        - Returns `false` if not a `String`.
        - Converts to `string_view` for comparison.
    5. **Convertible to `Json`-constructible types**:
        - Attempts conversion to `T` (if `T` supports comparison).
    6. **Types convertible to `Json`**:
        - Converts `T` to `Json` and delegates to same-type comparison.
    7. **Fallback**: Returns `false` if none of the above apply.

#### Exception Safety
No-throw guarantee.

#### Complexity
Linear (full content comparison).

## Version

Since v3.0.0 .
