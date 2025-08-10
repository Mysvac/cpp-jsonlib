# **Json.arr**

```cpp
constexpr Arr& arr() & ;
constexpr Arr&& arr() && ;
constexpr const Arr& arr() const & ;
constexpr const Arr&& arr() const && ;
```

Returns a reference to the internal `Arr` value.

**Return Value**  
Returns `const`/non-`const`, lvalue/rvalue reference to the `Arr` based on the object's qualifiers

**Exceptions**  
Throws `std::bad_variant_access` if the internal data is not an `Arr`

**Complexity**  
Constant time O(1)

## Version

Since v3.0.0 .
