#include <iostream>
#include <istream>
#include <iterator>
#include <sstream>

#include <wu-json/tokenization.hpp>

constexpr const char *test_chars =
    R"json({
  "string": "Hello, World!",
  "number": 12345.6789,
  "boolean_true": true,
  "boolean_false": false,
  "null_value": null,
  "object": {
    "nested_string": "Nested hello",
    "nested_number": 42,
    "nested_array": [1, 2, 3, 4, 5]
  },
  "array": ["string in array", 9876, false, {"obj_in_array": "hello"}],
  "escaped_characters": "Line 1\\nLine 2\\r\\nTab\\tQuotationMark\\\"Backslash\\\\"
})json";

bool string_test() {
  auto test_str = std::string(test_chars);

  if (!json::tokenize(test_str, [](auto &&tok) {
        std::cout << typeid(decltype(tok)).name() << '\n';
        return true;
      }))
    return false;

  return true;
}

bool istream_test() {
  std::istringstream iss(test_chars);
  std::istream_iterator<char> it(iss), end;

  if (!json::tokenize(it, end, [](auto &&tok) {
        std::cout << typeid(decltype(tok)).name() << '\n';
        return true;
      })) {
    return false;
  }

  return true;
}

int main(int argc, char **args) {
  if (!string_test())
    return -1;

  std::cout << '\n';
  if (!istream_test())
    return -1;

  return 0;
}
