# **Json.dumpf**

```cpp
String dumpf(
    const std::uint16_t space_num = 2, 
    const std::uint16_t depth = 0
) const noexcept;
```

Serializes JSON data to a formatted string and returns the result.

### Parameters
- **`space_num`**: Number of spaces per indentation level (unsigned integer)
- **`depth`**: Initial indentation depth (unsigned integer, default=0)

### Return Value
Returns a `String` object containing the formatted JSON:
- Properly indented with `space_num` spaces per level
- Starting from specified `depth` level

### Exception Safety
No-throw guarantee.

### Complexity
Linear time (O(n)), equivalent to:
1. Creating an empty string
2. Writing formatted JSON via `Json::writef()`

### Complexity

Content length n, recursive depth d:

O(n) usually.
Maximum is O(n * d).

> Because of a large amount of indentation.

## Version

Since v3.0.0 .