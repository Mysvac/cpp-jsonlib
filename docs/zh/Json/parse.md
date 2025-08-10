# **Json.parse**

```cpp
static std::optional<Json> parse(const std::string_view text, const std::int32_t max_depth = 256) noexcept;

static std::optional<Json> parse(std::istream& is_text, const std::int32_t max_depth = 256) noexcept;
```

### Static Member Function: `parse`

Parses JSON text from a string or input stream into a `Json` object.

#### Parameters
- **`text`**: A `std::string_view` containing JSON text to parse.
- **`is_text`**: An input stream (`std::istream`) with JSON text.
- **`max_depth`**: Maximum nesting depth allowed (default: 256).
  - If exceeded, returns `ParseError::eDepthExceeded`.
  - Prevents stack overflow from malicious/erroneous data (e.g., `[[[[...]]]]`).

#### Return Value
Returns `std::optional<Json>`:
- **Success**: Contains the parsed `Json` object.
- **Failure**: `std::nullopt` (invalid JSON or depth exceeded).

#### Exception Safety
No-throw guarantee.

#### Complexity
Linear (`O(n)`) in input length, **independent of nesting depth**.


## Version

Since v3.0.0 .
