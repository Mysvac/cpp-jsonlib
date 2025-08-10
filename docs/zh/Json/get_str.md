# **Json.str**

```cpp
constexpr Str& str() & ;
constexpr Str&& str() && ;
constexpr const Str& str() const & ;
constexpr const Str&& str() const && ;
```

Returns a reference to the internal string value.

**Return Value**  
Returns a reference with matching constness and value category (lvalue/rvalue) to the stored string

**Exceptions**  
Throws `std::bad_variant_access` if the contained value is not a string

**Complexity**  
Constant time O(1)

## Version

Since v3.0.0 .
