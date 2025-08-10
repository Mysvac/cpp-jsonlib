# **Json.nul**

```cpp
constexpr Null& nul() & ;
constexpr Null&& nul() && ;
constexpr const Null& nul() const & ;
constexpr const Null&& nul() const && ;
```

Returns a reference to the internal `Null` value.

**Return Value**  
Returns a reference with matching const/volatile qualification and value category (lvalue/rvalue) to the `Null` value

**Exceptions**  
Throws `std::bad_variant_access` if the stored value is not null

**Complexity**  
Constant time O(1)

## Version

Since v3.0.0 .
