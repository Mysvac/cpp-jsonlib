# **Json.writef**

```cpp
void writef(
    String& out,
    std::uint16_t space_num = 2,
    std::uint16_t depth = 0,
    std::uint32_t max_space = 512
) const noexcept;

void writef(
    std::ostream& out, 
    std::uint16_t space_num = 2,
    std::uint16_t depth = 0,
    std::uint32_t max_space = 512
) const;
```


Serializes JSON data with indentation to either:

- The end of a string
- An output stream

### Parameters

- **`space_num`**: Number of spaces per indentation level (unsigned integer)
- **`depth`**: Initial indentation depth (unsigned integer, default=0)

### Exception Safety

- **String output**: No-throw guarantee
-
- **Stream output**: Depends on stream implementation
    - Standard library streams: Never throws (sets `failbit` on errors)
    - Custom streams: May propagate their exceptions

### Complexity

Content length n, recursive depth d:

O(n) usually.
Maximum is O(n * d).

> Because of a large amount of indentation.

## Version

Since v3.0.0 .
