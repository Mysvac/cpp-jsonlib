# **Json.write**

```cpp
void write(Str& out) const noexcept;

void write(std::ostream& out) const;
```

Serializes JSON data to either:

1. The end of a string, or
2. An output stream

### Exception Safety

- **Str output**: No-throw guarantee
- 
- **Stream output**: Depends on stream implementation
    - Standard library streams: Never throws (sets `failbit` on errors)
    - Custom streams: May propagate their exceptions

### Complexity
Linear time (O(n))

## Version

Since v3.0.0 .
