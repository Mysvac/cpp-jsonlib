# **Json.erase**

```cpp
// 1
bool erase(const Str& key) noexcept;

// 2
bool erase(const std::size_t index) noexcept;
```

1. If the internal data type is `Object`, delete the key-value pair for the specified key. If the type does not match or the key does not exist, return `false` (no exception will be thrown). Return `true` if the deletion succeeds.

2. If the internal data type is `Array`, delete the element at the specified index. If the type does not match or the index is out of bounds, return `false` (no exception will be thrown). Return `true` if the deletion succeeds.

## Exceptions

None.

## Complexity

1. Ordered map: logarithmic complexity; hash map: average constant complexity.

2. Linear complexity, depending on the number of trailing elements.

## Version

Since v3.0.0 .

