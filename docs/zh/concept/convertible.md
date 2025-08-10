# **convertible**

```cpp
template<typename J, typename T>
concept convertible = std::disjunction_v<
    std::is_arithmetic<T>,
    std::is_enum<T>,
    std::is_convertible<typename J::Array, T>,
    std::is_convertible<typename J::Object, T>,
    std::is_convertible<typename J::String, T>,
    std::is_convertible<typename J::Num, T>,
    std::is_convertible<typename J::Bol, T>,
    std::is_convertible<typename J::Nul, T>,
    std::is_constructible<T, J>
>;
```

Located in the `mysvac::json` namespace, this concept indicates that a JSON container type `J` can attempt to convert its internal data to type `T` using `to()`/`move()` member functions.

Satisfying this concept only means that `T` can be used as a template parameter for `to()`/`move()`, while the actual success of conversion depends on the JSON container's internal data state.

## Version

Since v3.0.0 .

