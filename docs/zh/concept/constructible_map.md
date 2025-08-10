# **constructible_map**

```cpp
template<typename J, typename T>
concept constructible_map = std::ranges::range<T> &&
    std::is_convertible_v<typename T::key_type, typename J::Str> &&
    std::is_constructible_v<J, typename T::mapped_type>;
```

Located in the `mysvac::json` namespace, this concept represents key-value pair-like types that can serve as constructor arguments for JSON container type `J`.

If the type already satisfies the [`constructible`](./constructible.md) concept, it can be used for implicit construction or assignment operations.  
If it doesn't satisfy [`constructible`](./constructible.md) but meets this concept, it can only be used for explicit construction and cannot be directly assigned.

When a type satisfies both this concept and [`constructible_array`](./constructible_array.md), it will be treated as a `map` rather than an `array` by default.

## Version

Since v3.0.0 .


