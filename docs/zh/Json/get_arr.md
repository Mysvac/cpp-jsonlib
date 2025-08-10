# **Json.arr**

```cpp
constexpr Array& arr() & ;
constexpr Array&& arr() && ;
constexpr const Array& arr() const & ;
constexpr const Array&& arr() const && ;
```

Returns a reference to the internal `Array` value.

**Return Value**  
Returns `const`/non-`const`, lvalue/rvalue reference to the `Array` based on the object's qualifiers

**Exceptions**  
Throws `std::bad_variant_access` if the internal data is not an `Array`

**Complexity**  
Constant time O(1)

## Version

Since v3.0.0 .
