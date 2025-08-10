# **Json.nul**

```cpp
constexpr Nul& nul() & ;
constexpr Nul&& nul() && ;
constexpr const Nul& nul() const & ;
constexpr const Nul&& nul() const && ;
```

Returns a reference to the internal `Nul` value.

**Return Value**  
Returns a reference with matching const/volatile qualification and value category (lvalue/rvalue) to the `Nul` value

**Exceptions**  
Throws `std::bad_variant_access` if the stored value is not null

**Complexity**  
Constant time O(1)

## Version

Since v3.0.0 .
