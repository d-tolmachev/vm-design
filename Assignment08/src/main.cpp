#include <array>
#include <cassert>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#define STRING_PTR_DEBUG
#include "string_ptr.h"
#undef STRING_PTR_DEBUG

template <class TestCase>
void test(const std::string& test_name, TestCase& test_case) {
    std::cout << test_name << " started" << std::endl;
    test_case();
    std::cout << test_name << " finished" << std::endl << std::endl << std::endl;
}

void test_constructor() {
    assignment_08::string_ptr empty;
    assert(!empty.unique());
    assert(empty == std::string_view{""});
    assignment_08::string_ptr some_string("some string");
    assert(some_string.unique());
    assert(some_string == std::string_view{"some string"});
}

void test_copy_constructor() {
    assignment_08::string_ptr src("copyable string");
    assert(src.unique());
    assignment_08::string_ptr dst1(src);
    assert(!src.unique());
    assert(!dst1.unique());
    assert(src == std::string_view{"copyable string"});
    assert(dst1 == std::string_view{"copyable string"});
    assignment_08::string_ptr dst2("replaceable string");
    dst2 = src;
    assert(!dst2.unique());
    assert(!src.unique());
    assert(dst2 == std::string_view{"copyable string"});
}

void test_move_constructor() {
    assignment_08::string_ptr src("movable string");
    assert(src.unique());
    assignment_08::string_ptr dst1(std::move(src));
    assert(dst1.unique());
    assert(!src.unique());
    assert(dst1 == std::string_view{"movable string"});
    assert(src == std::string_view{""});
    assignment_08::string_ptr dst2("replaceable string");
    dst2 = std::move(dst1);
    assert(dst2.unique());
    assert(!dst1.unique());
    assert(dst2 == std::string_view{"movable string"});
}

void test_assignment_operator() {
    assignment_08::string_ptr src("assignable string");
    src = src;
    assert(src.unique());
    assert(src == std::string_view{"assignable string"});
    src = std::move(src);
    assert(src.unique());
    assert(src == std::string_view{"assignable string"});
    src = "new string";
    assert(src.unique());
    assert(src == std::string_view{"new string"});
}

void test_vector() {
    std::vector<assignment_08::string_ptr> strings;
    strings.push_back("one");
    strings.push_back("two");
    strings.push_back("three");
    for (const assignment_08::string_ptr& str : strings) {
        std::cout << str << std::endl;
    }
    strings.clear();
    assignment_08::string_ptr first("one");
    strings.push_back(first);
    strings.push_back(assignment_08::string_ptr{"two"});
    strings.push_back(assignment_08::string_ptr{"three"});
    for (const assignment_08::string_ptr& str : strings) {
        std::cout << str << std::endl;
    }
    strings.clear();
    strings.emplace_back(first);
    strings.emplace_back(assignment_08::string_ptr{"two"});
    strings.emplace_back(assignment_08::string_ptr{"three"});
    for (const assignment_08::string_ptr& str : strings) {
        std::cout << str << std::endl;
    }
    strings.clear();
}

void test_bubble_sort() {
    std::array<assignment_08::string_ptr, 10> arr{{"cache probe", "interpreter", "idiom analyzer", "static verifier", "truffle language", "memory pool", "concurrent pools", "rai string", "read byte", "parallel memcpy"}};
    for (const assignment_08::string_ptr& item : arr) {
        assert(item.unique());
    }
    for (size_t i = 0; i < arr.size(); ++i) {
        for (size_t j = 0; j + i + 1 < arr.size(); ++j) {
            if (static_cast<std::string_view>(arr.at(j)) > static_cast<std::string_view>(arr.at(j + 1))) {
                std::swap(arr[j], arr[j + 1]);
            }
        }
    }
    assert(arr.at(0) == std::string_view{"cache probe"});
    assert(arr.at(1) == std::string_view{"concurrent pools"});
    assert(arr.at(2) == std::string_view{"idiom analyzer"});
    assert(arr.at(3) == std::string_view{"interpreter"});
    assert(arr.at(4) == std::string_view{"memory pool"});
    assert(arr.at(5) == std::string_view{"parallel memcpy"});
    assert(arr.at(6) == std::string_view{"rai string"});
    assert(arr.at(7) == std::string_view{"read byte"});
    assert(arr.at(8) == std::string_view{"static verifier"});
    assert(arr.at(9) == std::string_view{"truffle language"});
    for (const assignment_08::string_ptr& item : arr) {
        assert(item.unique());
    }
}

int main() {
    test("constructor test", test_constructor);
    test("copy constructor test", test_copy_constructor);
    test("move constructor test", test_move_constructor);
    test("assignment operator test", test_assignment_operator);
    test("vector test", test_vector);
    test("bubble-sort test", test_bubble_sort);
    return 0;
}
