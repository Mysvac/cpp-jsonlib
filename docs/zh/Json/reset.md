# **Json.reset**

```cpp
template<typename T = Nul>
requires json_type<Json, T>
void reset() noexcept;
```

Resets internal data to the specified type (defaults to Nul).

## implementation

??? note "implementation"
    ```cpp
    template<typename T = Nul>
    requires json_type<Json, T>
    void reset() noexcept {
        if constexpr(std::is_same_v<T, Nul>) {
            m_data = Nul{};
        } else if constexpr(std::is_same_v<T, Bol>) {
            m_data = Bol{};
        } else if constexpr(std::is_same_v<T, Number>) {
            m_data = Number{};
        } else if constexpr(std::is_same_v<T, String>) {
            m_data = String{};
        } else if constexpr(std::is_same_v<T, Array>) {
            m_data = Array{};
        } else if constexpr(std::is_same_v<T, Object>) {
            m_data = Object{};
        }
    }
    ```

**Exception Safety**  
No-throw guarantee

**Complexity**  
Constant time O(1)


## Version

Since v3.0.0 .
