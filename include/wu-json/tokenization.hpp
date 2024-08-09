#pragma once

#include <concepts>
#include <ranges>
#include <string_view>

namespace wu::json {
template <typename Iterator> class token {
public:
  using char_type =
      std::iterator_traits<Iterator>::value_type;

  constexpr virtual std::basic_string_view<char_type>
  value() const noexcept = 0;
};

template <typename Iterator>
class object_begin_token : public token<Iterator> {
public:
  using char_type =
      std::iterator_traits<Iterator>::value_type;

  constexpr std::basic_string_view<char_type>
  value() const noexcept override {
    return "{";
  }
};

template <typename Iterator>
class object_end_token : public token<Iterator> {
public:
  using char_type =
      std::iterator_traits<Iterator>::value_type;

  constexpr std::basic_string_view<char_type>
  value() const noexcept override {
    return "}";
  }
};

template <typename Iterator>
class array_begin_token : public token<Iterator> {
public:
  using char_type =
      std::iterator_traits<Iterator>::value_type;

  constexpr std::basic_string_view<char_type>
  value() const noexcept override {
    return "[";
  }
};

template <typename Iterator>
class array_end_token : public token<Iterator> {
public:
  using char_type =
      std::iterator_traits<Iterator>::value_type;

  constexpr std::basic_string_view<char_type>
  value() const noexcept override {
    return "]";
  }
};

template <typename Iterator>
class name_separator_token : public token<Iterator> {
public:
  using char_type =
      std::iterator_traits<Iterator>::value_type;
  constexpr std::basic_string_view<char_type>
  value() const noexcept override {
    return ":";
  }
};

template <typename Iterator>
class value_separator_token : public token<Iterator> {
public:
  using char_type =
      std::iterator_traits<Iterator>::value_type;
  constexpr std::basic_string_view<char_type>
  value() const noexcept override {
    return ",";
  }
};

template <typename Iterator>
class boolean_true_token : public token<Iterator> {
public:
  using char_type =
      std::iterator_traits<Iterator>::value_type;

  constexpr std::basic_string_view<char_type>
  value() const noexcept override {
    return "true";
  }

  static constexpr std::optional<boolean_true_token>
  try_parse(Iterator &begin, Iterator end) noexcept {
    static constexpr std::basic_string_view<char_type>
        true_literal = "true";

    if constexpr (std::contiguous_iterator<Iterator>) {
      size_t distance = std::distance(begin, end);
      if (distance >= true_literal.size()) {
        auto view = std::basic_string_view<char_type>(
            &*begin, true_literal.size());
        if (view == true_literal)
          return boolean_true_token{};
      }
    } else {
      // Fallback to non-contiguous iterator handling
      auto it = begin;
      for (char_type ch : true_literal) {
        if (it == end || *it != ch) {
          return std::nullopt;
        }
        ++it;
      }
      begin = it; // Move the iterator past the parsed token
      return boolean_true_token{};
    }

    return std::nullopt;
  }
};

template <typename Iterator>
class boolean_false_token : public token<Iterator> {
public:
  using char_type =
      std::iterator_traits<Iterator>::value_type;

  constexpr std::basic_string_view<char_type>
  value() const noexcept override {
    return "false";
  }

  static constexpr std::optional<boolean_false_token>
  try_parse(Iterator &begin, Iterator end) noexcept {
    static constexpr std::basic_string_view<char_type>
        false_literal = "false";

    if constexpr (std::contiguous_iterator<Iterator>) {
      size_t distance = std::distance(begin, end);
      if (distance >= false_literal.size()) {
        auto view = std::basic_string_view<char_type>(
            &*begin, false_literal.size());
        if (view == false_literal)
          return boolean_false_token{};
      }
    } else {
      auto it = begin;
      for (char_type ch : false_literal) {
        if (it == end || *it != ch) {
          return std::nullopt;
        }
        ++it;
      }
      begin = it;
      return boolean_false_token{};
    }

    return std::nullopt;
  }
};

template <typename Iterator>
class null_token : public token<Iterator> {
public:
  using char_type =
      std::iterator_traits<Iterator>::value_type;

  constexpr std::basic_string_view<char_type>
  value() const noexcept override {
    return "null";
  }

  static constexpr std::optional<null_token>
  try_parse(Iterator &begin, Iterator end) noexcept {
    static constexpr std::basic_string_view<char_type>
        null_literal = "null";

    if constexpr (std::contiguous_iterator<Iterator>) {
      size_t distance = std::distance(begin, end);
      if (distance >= null_literal.size()) {
        auto view = std::basic_string_view<char_type>(
            &*begin, null_literal.size());
        if (view == null_literal)
          return null_token{};
      }
    } else {
      auto it = begin;
      for (char_type ch : null_literal) {
        if (it == end || *it != ch) {
          return std::nullopt;
        }
        ++it;
      }
      begin = it;
      return null_token{};
    }

    return std::nullopt;
  }
};

template <typename Iterator>
class number_token : public token<Iterator> {
public:
  using char_type =
      std::iterator_traits<Iterator>::value_type;

  using value_type =
      std::conditional_t<std::contiguous_iterator<Iterator>,
                         std::basic_string_view<char_type>,
                         std::basic_string<char_type>>;

  constexpr number_token(const value_type &value)
      : _value(value) {}

  constexpr number_token(value_type &&value)
      : _value(std::forward<value_type>(value)) {}

  constexpr std::basic_string_view<char_type>
  value() const noexcept override {
    return _value;
  }

  static constexpr std::optional<number_token>
  try_parse(Iterator &begin, Iterator end) noexcept {
    if (begin == end) {
      return std::nullopt;
    }

    auto it = begin;
    bool is_number = false;

    // Optional minus sign
    if (*it == '-') {
      ++it;
      if (it == end) {
        return std::nullopt;
      }
    }

    auto is_digit = [](char c) constexpr noexcept {
      return c >= '0' && c <= '9';
    };

    // Digits before decimal point
    if (is_digit(*it)) {
      is_number = true;
      while (it != end && is_digit(*it)) {
        ++it;
      }
    }

    // Decimal point and digits after it
    if (it != end && *it == '.') {
      ++it;
      if (it != end && is_digit(*it)) {
        is_number = true;
        while (it != end && is_digit(*it)) {
          ++it;
        }
      }
    }

    // Exponent part
    if (it != end && (*it == 'e' || *it == 'E')) {
      ++it;
      if (it != end && (*it == '+' || *it == '-')) {
        ++it;
      }
      if (it == end || !is_digit(*it)) {
        return std::nullopt;
      }
      while (it != end && is_digit(*it)) {
        ++it;
      }
    }

    if (!is_number) {
      return std::nullopt;
    }

    value_type parsed_value(begin, it);
    begin = it; // Update the iterator
    return number_token(parsed_value);
  }

private:
  value_type _value;
};

template <typename Iterator>
class string_token : public token<Iterator> {
public:
  using char_type =
      std::iterator_traits<Iterator>::value_type;

  using value_type =
      std::conditional_t<std::contiguous_iterator<Iterator>,
                         std::basic_string_view<char_type>,
                         std::basic_string<char_type>>;

  constexpr string_token(const value_type &value)
      : _value(value) {}

  constexpr string_token(value_type &&value)
      : _value(std::forward<value_type>(value)) {}

  constexpr std::basic_string_view<char_type>
  value() const noexcept override {
    return _value;
  }

  // static constexpr std::optional<string_token>
  // try_parse(Iterator &begin, Iterator end) noexcept {
  //   if (begin == end || *begin != '"') {
  //     return std::nullopt;
  //   }

  //   auto it =
  //       std::next(begin); // Move past the opening quote

  //   while (it != end) {
  //     if (*it == '\\') {
  //       // Skip the next character, as it is escaped
  //       ++it;
  //       if (it == end) {
  //         return std::nullopt; // Unterminated or improperly
  //                              // terminated string
  //       }
  //     } else if (*it == '"') {
  //       // Found the closing quote
  //       break;
  //     }
  //     ++it;
  //   }

  //   if (it == end) {
  //     return std::nullopt; // Unterminated string
  //   }

  //   if constexpr (std::contiguous_iterator<Iterator>) {
  //     std::basic_string_view<char_type> parsed_value(
  //         &*begin, std::distance(begin, it) + 1);
  //     begin = std::next(it); // Move past the closing quote
  //     return string_token(parsed_value);
  //   } else {
  //     std::basic_string<char_type> parsed_value(
  //         begin, std::next(it)); // Include the quotes
  //     begin = std::next(it); // Move past the closing quote
  //     return string_token(parsed_value);
  //   }
  // }

  static constexpr std::optional<string_token>
  try_parse(Iterator &begin, Iterator end) noexcept {
    if (begin == end || *begin != '"') {
      return std::nullopt;
    }

    auto start = std::next(begin); // Move past the opening quote
    auto it = start;

    while (it != end) {
      if (*it == '\\') {
        // Skip the next character, as it is escaped
        ++it;
        if (it == end) {
          return std::nullopt; // Unterminated or improperly terminated string
        }
      } else if (*it == '"') {
        // Found the closing quote
        auto trimmed_value = std::basic_string_view<char_type>(start, it - start);
        begin = std::next(it); // Move past the closing quote
        return string_token(trimmed_value);
      }
      ++it;
    }

    return std::nullopt; // Unterminated string
  }
  
private:
  value_type _value;
};

template <std::input_iterator Iterator>
constexpr bool tokenize(Iterator begin, Iterator end,
                        auto &&predicate) {
  auto it = begin;
  while (it != end) {
    switch (*it) {
    case '{': {
      predicate(object_begin_token<Iterator>{});
      ++it;
      break;
    }
    case '}': {
      predicate(object_end_token<Iterator>{});
      ++it;
      break;
    }
    case '[': {
      predicate(array_begin_token<Iterator>{});
      ++it;
      break;
    }
    case ']': {
      predicate(array_end_token<Iterator>{});
      ++it;
      break;
    }
    case ':': {
      predicate(name_separator_token<Iterator>{});
      ++it;
      break;
    }
    case ',': {
      predicate(value_separator_token<Iterator>{});
      ++it;
      break;
    }
    case 't': {
      const auto tok =
          boolean_true_token<Iterator>::try_parse(it, end);
      if (!tok.has_value())
        return false;

      predicate(*tok);
      ++it;
      break;
    }
    case 'f': {
      const auto tok =
          boolean_false_token<Iterator>::try_parse(it, end);
      if (!tok.has_value())
        return false;

      predicate(*tok);
      ++it;
      break;
    }
    case 'n': {
      const auto tok =
          null_token<Iterator>::try_parse(it, end);
      if (!tok.has_value())
        return false;

      predicate(*tok);
      ++it;
      break;
    }
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '-': {
      const auto tok =
          number_token<Iterator>::try_parse(it, end);
      if (!tok.has_value())
        return false;

      predicate(*tok);
      break;
    }
    case '\"': {
      const auto tok =
          string_token<Iterator>::try_parse(it, end);
      if (!tok.has_value())
        return false;

      predicate(*tok);
      break;
    }
    default:
      ++it;
    }
  }

  return true;
}

template <std::ranges::input_range Range>
constexpr bool tokenize(const Range &range,
                        auto &&predicate) noexcept {
  return tokenize(std::begin(range), std::end(range),
                  predicate);
}
} // namespace json