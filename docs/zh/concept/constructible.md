# **constructible**

```cpp
template<typename J, typename T>
concept constructible = std::disjunction_v<
    std::is_arithmetic<T>,
    std::is_enum<T>,
    std::is_convertible<T, typename J::Array>,
    std::is_convertible<T, typename J::Object>,
    std::is_convertible<T, typename J::String>,
    std::is_convertible<T, typename J::Number>,
    std::is_convertible<T, typename J::Bol>,
    std::is_convertible<T, typename J::Nul>
>;
```

Located in the `mysvac::json` namespace, this concept indicates that a JSON container type `J` can be **directly** constructed from type `T`.

`J` must be an instantiation of the json::Json class template


Types satisfying this concept can be used for:

- **Implicit construction** of `J` objects
- Assignment operations of `J` type

## Version

Since v3.0.0 .
    