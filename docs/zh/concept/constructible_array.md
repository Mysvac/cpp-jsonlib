# **constructible_array**

```cpp
template<typename J, typename T>
concept constructible_array = std::ranges::range<T> &&
    std::is_constructible_v<J, typename T::value_type>;
```

Located in the `mysvac::json` namespace, this concept identifies array-like types that can be used as constructor arguments for JSON container type `J`.

**Behavior Specifications**:
1. If the type already satisfies [`constructible`](./constructible.md):
    - Can be used for implicit construction and assignment operations
2. If only satisfying this concept (without [`constructible`](./constructible.md)):
    - Restricted to explicit construction only
    - Cannot be used in assignment operations
3. Conflict Resolution:
    - If a type satisfies both this concept and [`constructible_map`](./constructible_map.md):
    - The type is preferentially treated as a map rather than an array

## Version

Since v3.0.0 .
