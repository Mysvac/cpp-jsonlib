# **Json.insert**

```cpp
// 1
template<typename  K, typename V>
requires std::convertible_to<K, Str> && std::convertible_to<V, Json>
bool insert(K&& key, V&& value) noexcept;

// 2
template<typename V>
requires std::convertible_to<V, Json>
bool insert(const std::size_t index, V&& value) noexcept;
```

1. If the internal data type is `Object`, insert the specified key-value pair (overwriting if it already exists) and return `true`. If the type does not match, return `false` (no exception will be thrown).

2. If the internal data type is `Array` and the index is within range (end index allowed), insert the specified element and return `true`. If the type does not match or the index is out of bounds, return `false` (no exception will be thrown).

## Exceptions

None.

## Complexity

1. Ordered map: logarithmic complexity; hash map: average constant complexity.

2. Linear complexity, depending on the number of trailing elements.

## Version

Since v3.0.0 .

