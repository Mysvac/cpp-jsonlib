# **Json.operator=**

```cpp
// 1
Json& operator=(const Json&) = default;

// 2
Json& operator=(Json&& other) noexcept;

// 3
template<typename T>
requires constructible<Json, std::remove_cvref_t<T>>
Json& operator=(T&& other) noexcept;
```

赋值运算符重载。

## Complexity

1. Linear time O(n)
2. Constant time O(1)
3. Constant time O(1) for simple types or direct moves, otherwise linear time O(n)

## Version

Since v3.0.0 .

