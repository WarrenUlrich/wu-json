#include <array>
#include <iostream>
#include <ranges>
#include <string_view>
#include <typeindex>
#include <vector>
#include <cassert>
#include <wu-json/tokenization.hpp>

using namespace wu;

class tokenization_test {
public:
    std::string_view test_string;
    std::vector<std::pair<std::type_index, std::string_view>> expected;

    bool run() const {
        std::vector<std::pair<std::type_index, std::string_view>> result;
        json::tokenize(test_string, [&](const auto &tok) {
            result.emplace_back(typeid(tok), tok.value());
        });

        if (expected.size() != result.size()) {
            std::cout << "\t\tFailed to tokenize string: "
                      << test_string << '\n'
                      << "\t\tExpected size of tokens (" << expected.size() 
                      << ") is not equal to the actual number of tokens (" << result.size() << ").\n";
            return false;
        }

        for (size_t i = 0; i < expected.size(); ++i) {
            if (expected[i].first != result[i].first ||
                expected[i].second != result[i].second) {
                std::cout << "Expected: " << expected[i].first.name() << ": " << expected[i].second << '\n'
                          << "Actual:   " << result[i].first.name() << ": " << result[i].second << '\n';
                return false;
            }
        }

        return true;
    }
};

int run_all_tests() {
    std::vector<tokenization_test> tests = {
        // Test for a simple string
        {
            .test_string = "\"Hello, World\"",
            .expected = {
                {std::type_index(typeid(json::string_token<std::string_view::iterator>)), "Hello, World"}
            }
        },

        // Test for a boolean true value
        {
            .test_string = "true",
            .expected = {
                {std::type_index(typeid(json::boolean_true_token<std::string_view::iterator>)), "true"}
            }
        },

        // Test for a boolean false value
        {
            .test_string = "false",
            .expected = {
                {std::type_index(typeid(json::boolean_false_token<std::string_view::iterator>)), "false"}
            }
        },

        // Test for a null value
        {
            .test_string = "null",
            .expected = {
                {std::type_index(typeid(json::null_token<std::string_view::iterator>)), "null"}
            }
        },

        // Test for a simple number
        {
            .test_string = "123.45",
            .expected = {
                {std::type_index(typeid(json::number_token<std::string_view::iterator>)), "123.45"}
            }
        },

        // Test for an object with one key-value pair
        {
            .test_string = "{\"key\": \"value\"}",
            .expected = {
                {std::type_index(typeid(json::object_begin_token<std::string_view::iterator>)), "{"},
                {std::type_index(typeid(json::string_token<std::string_view::iterator>)), "key"},
                {std::type_index(typeid(json::name_separator_token<std::string_view::iterator>)), ":"},
                {std::type_index(typeid(json::string_token<std::string_view::iterator>)), "value"},
                {std::type_index(typeid(json::object_end_token<std::string_view::iterator>)), "}"}
            }
        },

        // Test for an array of numbers
        {
            .test_string = "[1, 2, 3]",
            .expected = {
                {std::type_index(typeid(json::array_begin_token<std::string_view::iterator>)), "["},
                {std::type_index(typeid(json::number_token<std::string_view::iterator>)), "1"},
                {std::type_index(typeid(json::value_separator_token<std::string_view::iterator>)), ","},
                {std::type_index(typeid(json::number_token<std::string_view::iterator>)), "2"},
                {std::type_index(typeid(json::value_separator_token<std::string_view::iterator>)), ","},
                {std::type_index(typeid(json::number_token<std::string_view::iterator>)), "3"},
                {std::type_index(typeid(json::array_end_token<std::string_view::iterator>)), "]"}
            }
        },

        // Test for nested objects
        {
            .test_string = "{\"outer\": {\"inner\": 42}}",
            .expected = {
                {std::type_index(typeid(json::object_begin_token<std::string_view::iterator>)), "{"},
                {std::type_index(typeid(json::string_token<std::string_view::iterator>)), "outer"},
                {std::type_index(typeid(json::name_separator_token<std::string_view::iterator>)), ":"},
                {std::type_index(typeid(json::object_begin_token<std::string_view::iterator>)), "{"},
                {std::type_index(typeid(json::string_token<std::string_view::iterator>)), "inner"},
                {std::type_index(typeid(json::name_separator_token<std::string_view::iterator>)), ":"},
                {std::type_index(typeid(json::number_token<std::string_view::iterator>)), "42"},
                {std::type_index(typeid(json::object_end_token<std::string_view::iterator>)), "}"},
                {std::type_index(typeid(json::object_end_token<std::string_view::iterator>)), "}"}
            }
        }
    };

    bool all_tests_passed = true;

    int failing = 0;
    for (const auto& test : tests) {
        if (!test.run()) {
            ++failing;
            std::cout << "Test failed for input: " << test.test_string << '\n';
        }
    }

    return failing;
}

int main() {
    std::cout << run_all_tests() << " tests failed." << '\n';
    return 0;
}
