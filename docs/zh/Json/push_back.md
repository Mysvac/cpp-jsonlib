# **Json.push_back**

```cpp
template<typename V>
requires std::convertible_to<V, Json>
bool push_back(V&& value) noexcept;
```

If the internal data type is `Arr`, append the specified element to the end and return `true`. If the type does not match, return `false` (no exception will be thrown).

## Exceptions

None.

## Complexity

Constant.

## Version

Since v3.0.0 .
