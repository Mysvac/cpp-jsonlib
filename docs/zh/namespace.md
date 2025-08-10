# **namespace**

The library's contents are primarily located in the mysvac::json namespace, but a default alias for the Json class is provided in the mysvac namespace for ease of use.

## Module

You can import the module as follows:

```cpp
import std; // STD module or header files
import mysvac.json;
```

## Example

```cpp
// recommend
using namespace mysvac;

...

Json my_json{};
my_json = Json::object{
    {"key1", "value1"},
    {"key2", 42},
    {"key3", true}
};

json::Type type = my_json.type();
std::cout << json::type_name(type);
```

Note that the `mysvac::Json` type is actually a default instantiation of the `mysvac::json::Json` class template, defined as follows:

```cpp
namespace mysvac {
    using Json = ::mysvac::json::Json<>;
}
```

All type-specific operations (including the six JSON subtype handlers, serialization/deserialization, etc.) are implemented within the `Json` class template.

Non-type-dependent features are organized in the `json` namespace, including:

- The type enumeration json::Type
- Various concepts (C++20 concepts)
- Other generic utilities

## Version

Since v3.0.0 .
