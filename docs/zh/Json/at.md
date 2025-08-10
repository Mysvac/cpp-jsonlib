# **Json.at**

```cpp
// 1
Json& at(const Str& key);

// 2
const Json& at(const Str& key) const;

// 3
Json& at(const std::size_t index);

// 4
const Json& at(const std::size_t index) const;
```

1. Accesses object member by key - returns mutable reference, throws on invalid key
2. Accesses object member by key - returns const reference, throws on invalid key
3. Accesses array element by index - returns mutable reference, throws on out-of-bounds
4. Accesses array element by index - returns const reference, throws on out-of-bounds

**Exception Safety**
- Throws `std::bad_variant_access` if underlying data isn't Object/Array
- Throws `std::out_of_range` for invalid keys/indices

**Complexity**
- Array access: O(1)
- Object access: O(logN) (ordered map) / amortized O(1) (hash map)

## Version

Since v3.0.0 .

