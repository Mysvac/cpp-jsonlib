# **Json.num**

```cpp
constexpr Number& num() & ;
constexpr Number&& num() && ;
constexpr const Number& num() const & ;
constexpr const Number&& num() const && ;
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
