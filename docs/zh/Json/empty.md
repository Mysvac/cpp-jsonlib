# **Json.empty**

```cpp
bool empty() const noexcept;
```

Checks if the container is empty for `Object`/`Array`, returns `true` for other types.

**Return Value**
- `Object`/`Array`: `true` if empty (equivalent to `size() == 0`)
- All other types: `true` (including `Str`)

**Exception Safety**  
No-throw guarantee

**Complexity**  
Constant

## Version

Since v3.0.0 .



