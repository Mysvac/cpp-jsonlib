# **Json.obj**

```cpp
constexpr Object& obj() & ;
constexpr Object&& obj() && ;
constexpr const Object& obj() const & ;
constexpr const Object&& obj() const && ;
```

Returns a reference to the internal `Object` value.

**Return Value**  
Returns a reference with matching const-qualification and value category (lvalue/rvalue) to the stored object

**Exceptions**  
Throws `std::bad_variant_access` if the contained value is not an object

**Complexity**  
Constant time O(1)

## Version

Since v3.0.0 .
