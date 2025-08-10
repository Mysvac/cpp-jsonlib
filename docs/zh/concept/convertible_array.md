# **convertible_array**

```cpp
template<typename J, typename T, typename D>
concept convertible_array =  std::ranges::range<T> && requires {
    requires std::is_constructible_v<typename T::value_type,D>;
    requires convertible<J, typename T::value_type>;
    requires std::is_default_constructible_v<T>;
    requires std::is_default_constructible_v<typename T::value_type>;
    requires std::is_copy_constructible_v<typename T::value_type>;
} && requires (T t, typename T::value_type v) {
    t.emplace_back(std::move(v));
};
```

Located in the `mysvac::json` namespace, this concept indicates that a JSON container type `J` can attempt to convert its internal data to array type `T` using `to()`/`move()` member functions.

Satisfying this concept only means `T` can be used as a template parameter for `to()`/`move()`, while actual conversion success depends on the JSON container's internal data.

The template parameter `D` represents the default value type for array type `T`, which need not be `value_type` but must be convertible to `value_type`.
This is used to provide default values for subobjects in conversion functions.

## Version

Since v3.0.0 .





