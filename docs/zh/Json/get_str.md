# **Json.str**

```cpp
constexpr String& str() & ;
constexpr String&& str() && ;
constexpr const String& str() const & ;
constexpr const String&& str() const && ;
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
