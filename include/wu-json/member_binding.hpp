#pragma once

#include <string_view>

namespace json {
  class member_binding {
  public:
    std::string_view key;
  };
}