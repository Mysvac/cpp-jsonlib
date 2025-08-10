# **Json.num**

```cpp
constexpr Num& num() & ;
constexpr Num&& num() && ;
constexpr const Num& num() const & ;
constexpr const Num&& num() const && ;
```

Returns a reference to the internal numeric value.

**Return Value**  
Returns a reference with matching const-qualification and value category (lvalue/rvalue) to the stored number

**Exceptions**  
Throws `std::bad_variant_access` if the contained value is not numeric

**Complexity**  
Constant time O(1)

## Version

Since v3.0.0 .
