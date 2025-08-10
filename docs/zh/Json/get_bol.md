# **Json.bol**

```cpp
constexpr Bool& bol() & ;
constexpr Bool&& bol() && ;
constexpr const Bool& bol() const & ;
constexpr const Bool&& bol() const && ;
```

Returns a reference to the internal `Bool` value.

**Return Value**  
Returns a `const`/non-`const`, lvalue/rvalue reference matching the object's qualification

**Exceptions**  
Throws `std::bad_variant_access` if the contained value is not a boolean

**Complexity**  
Constant time O(1)

## Version

Since v3.0.0 .
