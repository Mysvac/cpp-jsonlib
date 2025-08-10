# **Type**

```cpp
enum class Type{
    eNul = 0,
    eBol,    
    eNumber,  
    eString,  
    eArray,   
    eObject   
};
```

Located in the `mysvac::json` namespace, this enumeration represents the six fundamental JSON types.

Since any `Json` container type can only store these six possible data types internally, a unified enumeration is used for type identification.

## Version

Since v3.0.0 .
