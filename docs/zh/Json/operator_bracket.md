# **Json.operator[]**

```cpp
// 1
Json& operator[](const Str& key) { return std::get<Obj>(m_data)[key]; }

// 2
const Json& operator[](const Str& key) const { return std::get<Obj>(m_data).at(key); }

// 3
Json& operator[](const std::size_t index) { return std::get<Arr>(m_data)[index]; }

// 4
const Json& operator[](const std::size_t index) const { return std::get<Arr>(m_data).at(index); }
```

1. Accesses object member by key - returns mutable reference, creates default-valued object if key doesn't exist (no-throw)
2. Accesses object member by key - returns const reference, throws if key doesn't exist
3. Accesses array element by index - returns mutable reference, undefined behavior if out-of-bounds
4. Accesses array element by index - returns const reference, throws std::out_of_range if out-of-bounds

**Exceptions**
- Throws std::bad_variant_access if internal data isn't Obj/Arr when attempting access
- Const accesses throw std::out_of_range for invalid indices/keys

**Complexity**
- Arr access: O(1)
- Obj access: O(logN) for ordered maps, amortized O(1) for hash maps

## Version

Since v3.0.0 .
