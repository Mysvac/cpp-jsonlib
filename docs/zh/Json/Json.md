# **Json**

```cpp
template<
    bool UseOrderedMap = true,
    template<typename U> class VecAllocator = std::allocator,
    template<typename U> class MapAllocator = std::allocator,
    template<typename U> class StrAllocator = std::allocator
>
requires requires{
    typename std::basic_string<char, std::char_traits<char>, StrAllocator<char>>;
    typename std::vector<int, VecAllocator<int>>;
    typename std::map<std::string, int, std::less<std::string>, MapAllocator<std::pair<const std::string, int>>>;
    typename std::unordered_map<std::string, int, std::hash<std::string>, std::equal_to<std::string>, MapAllocator<std::pair<const std::string, int>>>;
}
class Json;
```

Located within the `mysvac::json` namespace, this template provides a generic JSON container class featuring comprehensive JSON data operations including CRUD (Create, Read, Update, Delete), serialization, and deserialization capabilities.

For convenience, a default `Json` type alias is exposed directly in the `mysvac` namespace to simplify usage:

```cpp
namespace mysvac {
    using Json = ::mysvac::json::Json<>;
}
```

## Template Parameters

1. `UseOrderedMap` : Specifies whether to use ordered mapping (`std::map`) or unordered mapping (`std::unordered_map`)
2. `VecAllocator` : Allocator template for array type (`std::vector`)
3. `MapAllocator` : Allocator template for object type (mapping)
4. `StrAllocator` : Allocator template for string type (`std::basic_string`)

## 内部类型

Each `Json` class internally contains six types corresponding to the six JSON data formats, defined as follows:

```cpp
// public
using Null = std::nullptr_t;
using Bool = bool;
using Number = double;
using String = std::basic_string<char, std::char_traits<char>, StrAllocator<char>>;
using Array = std::vector<Json, VecAllocator<Json>>;
using Object = std::conditional_t<UseOrderedMap,
    std::map<String, Json, std::less<String>, MapAllocator<std::pair<const String, Json>>>,
    std::unordered_map<String, Json, std::hash<String>, std::equal_to<String>, MapAllocator<std::pair<const String, Json>>>
>;
```

The types for `Null`, `Bool` and `Number` are completely fixed. For `String`, `Array` and `Object`, the memory allocator can be customized through template parameter `AllocatorType`.
While `Object` can choose between ordered or hash-based implementations, the class templates themselves remain fixed.

By default:
- `String` equals `std::string`
- `Array` equals `std::vector<Json>`
- `Object` equals `std::map<String, Json>`

## Member Variables

The class contains only one member - a `std::variant` type that stores any of the six possible values, defaulting to `Null`.

```cpp
// protected
std::variant<
    Null,
    Bool,
    Number,
    String,
    Array,
    Object
> m_data { Null{} };
```

## Member Functions

### 1. Construction

- [constructor](constructor.md): Constructors supporting various initialization types
- [destructor](destructor.md): Destructor releasing internal resources
- [operator=](operator_assign.md): Assignment operator supporting multiple value types

### 2. Basic Operations

- [type](type.md): Get current JSON data type
- [is_nul](is_nul.md): Check if current JSON is `Null`
- [is_bol](is_bol.md): Check if current JSON is `Bool`
- [is_num](is_num.md): Check if current JSON is `Number`
- [is_str](is_str.md): Check if current JSON is `String`
- [is_arr](is_arr.md): Check if current JSON is `Array`
- [is_obj](is_obj.md): Check if current JSON is `Object`
- [nul](get_nul.md): Get reference to internal `Null` data
- [bol](get_bol.md): Get reference to internal `Bool` data
- [num](get_num.md): Get reference to internal `Number` data
- [str](get_str.md): Get reference to internal `String` data
- [arr](get_arr.md): Get reference to internal `Array` data
- [obj](get_obj.md): Get reference to internal `Object` data

### 3. Data Operations

- ['[]'](operator_bracket.md): Subscript operator for accessing/modifying array/object elements
- [at](at.md): Access elements with bounds checking
- [to](to.md): Convert/copy internal data to specified type (throws on failure)
- [to_if](to_if.md): Attempt conversion returning `optional`
- [to_or](to_or.md): Convert with fallback to default value
- [move](move.md): Move-convert internal data (throws on failure)
- [move_if](move_if.md): Attempt move-conversion returning `optional`
- [move_or](move_or.md): Move-convert with fallback to default value
- [operator==](operator_eq.md): Equality comparison

### 4. Serialization

- [parse](parse.md): Static method parsing JSON text to `Json` object
- [dump](dump.md): Serialize to compact string
- [dumpf](dumpf.md): Serialize to formatted string
- [write](write.md): Serialize compact output to stream/string
- [writef](writef.md): Serialize formatted output to stream/string

### 5. Container Operations

- [reset](reset.md): Reset JSON data
- [size](size.md): Get element count
- [contains](contains.md): Check if key/index exists
- [empty](empty.md): Check if container has no elements
- [erase](erase.md): Remove element by key/index
- [insert](insert.md): Insert new key-value pair/element
- [push_back](push_back.md): Append element to array
- [pop_back](pop_back.md): Remove last array element
- 
## Version

Since v3.0.0 .
