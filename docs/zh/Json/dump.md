# **Json.dump**

```cpp
String dump() const noexcept;
```

Serializes JSON data to a string and returns the result.

Output is compact (no whitespace).

#### Exception Safety
No-throw guarantee.

### Complexity
Linear time (`O(n)`), equivalent to:
1. Creating an empty string
2. Writing JSON data via `Json::write()`


## Version

Since v3.0.0 .
