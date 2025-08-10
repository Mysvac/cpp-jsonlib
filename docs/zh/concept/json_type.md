# **json_type**

```cpp
template<typename J, typename T>
concept json_type = std::disjunction_v<
    std::is_same<T, typename J::Nul>,
    std::is_same<T, typename J::Bol>,
    std::is_same<T, typename J::Num>,
    std::is_same<T, typename J::Str>,
    std::is_same<T, typename J::Arr>,
    std::is_same<T, typename J::Obj>
>;
```

In `mysvac:: json` namespace, it is used to determine whether a type ` T ` is a subtype within ` J `.
` J ` must be an instantiated type of the ` json:: Json ` class template.

As json::Json is a class template, its inner types differ based on the template arguments.
Hence, the concept must consider both the type T to be checked and the container type J as input.

The template supports only the six JSON data types and does not account for the JSON container type J.

## Example

```cpp
using namespace mysvac;

...

if constexpr (json_type<Json, T>) {
    std::cout << "T is a JSON object." << std::endl;
} else {
    std::cout << "T is not a JSON object." << std::endl;
}
```

## Version

Since v3.0.0 .


