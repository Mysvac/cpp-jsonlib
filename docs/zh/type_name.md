# **type_name**

```cpp
constexpr const char* type_name(const Type type) noexcept;
```

Located in the `mysvac::json` namespace, this utility converts [`Type`](./Type.md) enumeration values to their corresponding string representations.

**Recommended Use:** Primarily for debugging and log output purposes.

## implementation

??? note "implementation"
    ```cpp
    constexpr const char* type_name(const Type type) noexcept{
        switch ( type ) {
            case Type::eObject: return "Object";
            case Type::eArray:  return "Array";
            case Type::eString: return "String";
            case Type::eNumber: return "Number";
            case Type::eBool:   return "Bool";
            case Type::eNull:   return "Null";
            default: return "Unknown Enum Value"; // should never happen
        }
    }
    ```

## Exception

No exception.

## Complexity

Constant.

## Version

Since v3.0.0 .

