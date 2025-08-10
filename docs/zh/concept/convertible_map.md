# **convertible_map**

```cpp
template<typename J, typename T, typename D>
concept convertible_map = std::ranges::range<T> && requires {
    requires std::is_constructible_v<typename T::mapped_type, D>;
    requires std::is_convertible_v<typename J::String, typename T::key_type>;
    requires convertible<J, typename T::mapped_type>;
    requires std::is_default_constructible_v<T>;
    requires std::is_default_constructible_v<typename T::mapped_type>;
    requires std::is_copy_constructible_v<typename T::mapped_type>;
} && requires (T t, typename J::String s, typename T::mapped_type m) {
    t.emplace(static_cast<typename T::key_type>(s), std::move(m));
};
```

Located in the `mysvac::json` namespace, this concept indicates that a JSON container type `J` can attempt to convert its internal data to key-value pair type `T` using `to()`/`move()` member functions.

Satisfying this concept only means `T` can be used as a template parameter for `to()`/`move()`, while actual conversion success depends on the JSON container's internal data.

The template parameter `D` represents the default value type for key-value pair type `T`, which need not be `mapped_type` but must be convertible to `mapped_type`. This is used to provide default values for subobjects in conversion functions.

## Version

Since v3.0.0 .



