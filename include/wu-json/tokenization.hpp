#pragma once

#include <concepts>
#include <iterator>
#include <ranges>

namespace json {

template <typename Iterator> class base_token {
public:
  using char_t = typename Iterator::value_type;

  using value_t =
      std::conditional_t<std::contiguous_iterator<Iterator>,
                         std::basic_string_view<char_t>,
                         std::basic_string<char_t>>;

  value_t value;

  base_token(const value_t &value) : value(value) {}

  base_token(value_t &&value) : value(std::move(value)) {}
};

template <typename Iterator>
class string_token : public base_token<Iterator> {
public:
  string_token(const base_token<Iterator>::value_t &value)
      : base_token<Iterator>(value) {}

  string_token(base_token<Iterator>::value_t &&value)
      : base_token<Iterator>(std::move(value)) {}
};

template <typename Iterator>
class number_token : public base_token<Iterator> {
public:
  number_token(const base_token<Iterator>::value_t &value)
      : base_token<Iterator>(value) {}

  number_token(base_token<Iterator>::value_t &&value)
      : base_token<Iterator>(std::move(value)) {}
};

class object_begin_token {};

class object_end_token {};

class array_begin_token {};

class array_end_token {};

class name_seperator_token {};

class value_seperator_token {};

class true_token {};

class false_token {};

class null_token {};

template <std::input_iterator Iterator>
  requires std::same_as<typename Iterator::value_type,
                        char> ||
           std::same_as<typename Iterator::value_type,
                        wchar_t>
bool tokenize(Iterator begin, Iterator end,
              const auto &predicate) noexcept {
  using char_t = typename Iterator::value_type;
  auto it = begin;
  while (it != end) {
    while (it != end && std::isspace(*it))
      ++it;

    if (it == end) {
      break;
    }

    switch (*it) {
    case 'n': { // null_token
      constexpr char null_str[] = "null";
      if constexpr (std::contiguous_iterator<Iterator>) {
        if (!(std::distance(it, end) >= 4 &&
              std::equal(it, it + 4, std::begin(null_str),
                         std::end(null_str) - 1)))
          return false;

        if (!predicate(null_token()))
          return false;

        std::advance(it, 4);
      } else {
        for (int i = 0; i < 4 && it != end; ++i, ++it) {
          if (*it != null_str[i]) {
            return false; // tokenization error
          }
        }

        if (!predicate(null_token()))
          return false;
      }
      break;
    }
    case 't': { // true_token
      constexpr char true_str[] = "true";
      if constexpr (std::contiguous_iterator<Iterator>) {
        if (!(std::distance(it, end) >= 4 &&
              std::equal(it, it + 4, std::begin(true_str),
                         std::end(true_str) - 1)))
          return false;

        if (!predicate(true_token()))
          return false;

        std::advance(it, 4);
      } else {
        for (int i = 0; i < 4 && it != end; ++i, ++it) {
          if (*it != true_str[i]) {
            return false; // tokenization error
          }
        }

        if (!predicate(true_token()))
          return false;
      }
      break;
    }
    case 'f': { // false_token
      constexpr char false_str[] = "false";
      if constexpr (std::contiguous_iterator<Iterator>) {
        if (!(std::distance(it, end) >= 5 &&
              std::equal(it, it + 5, std::begin(false_str),
                         std::end(false_str) - 1)))
          return false;

        if (!predicate(false_token()))
          return false;

        std::advance(it, 5);
      } else {
        for (int i = 0; i < 5 && it != end; ++i, ++it) {
          if (*it != false_str[i]) {
            return false; // tokenization error
          }
        }

        if (!predicate(false_token()))
          return false;
      }
      break;
    }
    case '{': { // object_begin_token
      if (!predicate(object_begin_token()))
        return false;

      ++it;
      break;
    }
    case '}': { // object_end_token
      if (!predicate(object_end_token()))
        return false;

      ++it;
      break;
    }
    case '[': { // array_begin_token
      if (!predicate(array_begin_token()))
        return false;

      ++it;
      break;
    }
    case ']': { // array_end_token
      if (!predicate(array_end_token()))
        return false;

      ++it;
      break;
    }
    case ':': {
      if (!predicate(name_seperator_token()))
        return false;

      ++it;
      break;
    }
    case ',': {
      if (!predicate(value_seperator_token()))
        return false;

      ++it;
      break;
    }
    case '\"': { // string_token
      if constexpr (std::contiguous_iterator<Iterator>) {
        ++it; // Skip the initial quotation mark
        auto start = it;
        bool escaped = false;

        while (it != end) {
          if (*it == '\\' && !escaped) { // Escape character
            escaped = true;
          } else if (*it == '\"' &&
                     !escaped) { // End of string
            break;
          } else {
            escaped = false;
          }
          ++it;
        }

        if (it == end || *it != '\"') {
          return false; // End of string not found before
                        // iterator end
        }

        typename base_token<Iterator>::value_t string_value(
            start, it);

        if (!predicate(string_token<Iterator>(
                std::move(string_value))))
          return false;

        ++it; // Skip the ending quotation mark
        break;
      } else {
        ++it; // Skip the initial quotation mark
        std::basic_string<char_t> string_value;
        bool escaped = false;

        while (it != end) {
          if (*it == '\\' && !escaped) { // Escape character
            escaped = true;
          } else if (*it == '\"' &&
                     !escaped) { // End of string
            break;
          } else {
            if (escaped && *it != '\"' && *it != '\\') {
              // If the character following a backslash is
              // not a quote or another backslash, it should
              // be treated as a normal character. For
              // example, in the string "ab\\nc", the
              // character 'n' follows a backslash but is
              // not an escape sequence and should be added
              // to the string as is.
              string_value.push_back('\\');
            }
            string_value.push_back(*it);
            escaped = false;
          }
          ++it;
        }

        if (it == end || *it != '\"') {
          return false; // End of string not found before
                        // iterator end
        }

        if (!predicate(string_token<Iterator>(
                std::move(string_value))))
          return false;

        ++it; // Skip the ending quotation mark
        break;
      }
    }
    case '-':
    case '+':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9': { // number_token
      if constexpr (std::contiguous_iterator<Iterator>) {
        auto start = it;

        // Optional sign part
        if (*it == '-' || *it == '+') {
          ++it;
          if (it == end ||
              !std::isdigit(
                  *it)) // After a sign, a digit is expected
            return false;
        }

        // Integer part
        while (it != end && std::isdigit(*it)) {
          ++it;
        }

        // Optional fractional part
        if (it != end && *it == '.') {
          ++it;
          if (it == end ||
              !std::isdigit(*it)) // There must be at least
                                  // one digit after '.'
            return false;
          while (it != end && std::isdigit(*it)) {
            ++it;
          }
        }

        // Optional exponent part
        if (it != end && (*it == 'e' || *it == 'E')) {
          ++it;
          if (it != end &&
              (*it == '+' ||
               *it == '-')) // Optional sign in exponent
            ++it;

          if (it == end ||
              !std::isdigit(*it)) // There must be at least
                                  // one digit in exponent
            return false;

          while (it != end && std::isdigit(*it)) {
            ++it;
          }
        }

        // Construct number string
        typename base_token<Iterator>::value_t number_str(
            start, it);

        // Create token and invoke predicate
        if (!predicate(number_token<Iterator>(
                std::move(number_str))))
          return false;
      } else { // Non-contiguous single-pass iterator case
        std::basic_string<char_t> number_chars;

        // Optional sign part
        if (*it == '-' || *it == '+') {
          number_chars += *it++;
          if (it == end ||
              !std::isdigit(
                  *it)) // After a sign, a digit is expected
            return false;
        }

        // Integer part
        while (it != end && std::isdigit(*it)) {
          number_chars += *it++;
        }

        // Optional fractional part
        if (it != end && *it == '.') {
          number_chars += *it++;
          if (it == end ||
              !std::isdigit(*it)) // There must be at least
                                  // one digit after '.'
            return false;
          while (it != end && std::isdigit(*it)) {
            number_chars += *it++;
          }
        }

        // Optional exponent part
        if (it != end && (*it == 'e' || *it == 'E')) {
          number_chars += *it++;
          if (it != end &&
              (*it == '+' ||
               *it == '-')) // Optional sign in exponent
            number_chars += *it++;

          if (it == end ||
              !std::isdigit(*it)) // There must be at least
                                  // one digit in exponent
            return false;

          while (it != end && std::isdigit(*it)) {
            number_chars += *it++;
          }
        }

        // Create token and invoke predicate
        if (!predicate(number_token<Iterator>(
                std::move(number_chars))))
          return false;
      }
      break;
    }
    default:
      return false; // unrecognized token error
    }
  }

  return true;
}

template <std::ranges::input_range Range>
  requires std::same_as<std::ranges::range_value_t<Range>,
                        char> ||
           std::same_as<std::ranges::range_value_t<Range>,
                        wchar_t>
bool tokenize(const Range &range,
              const auto &predicate) noexcept {
  return tokenize(range.begin(), range.end(), predicate);
}
} // namespace json