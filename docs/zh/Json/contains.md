# **Json.contains**

```cpp
bool contains(const String& key) const noexcept;
```

Checks if an `Object` contains the specified key, returns `false` for non-object types.


**Return Value**
- `Object`: Result of underlying `contains()` call
- Other types: `false`

**Exception Safety**  
No-throw guarantee

**Complexity**
- Ordered map (`std::map`): O(log n)
- Hash map (`std::unordered_map`): Average O(1)

## Version

Since v3.0.0 .

