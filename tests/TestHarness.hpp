#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <sstream>

namespace gamescript::test {

struct TestCase {
    std::string name;
    std::function<void()> func;
};

class TestRegistry {
public:
    static TestRegistry& instance() {
        static TestRegistry reg;
        return reg;
    }

    void addTest(std::string name, std::function<void()> func) {
        tests_.push_back({std::move(name), std::move(func)});
    }

    int runAll() {
        int passed = 0;
        int failed = 0;
        std::cout << "========================================\n";
        std::cout << " Running GameScript Automated Test Suite \n";
        std::cout << "========================================\n";

        for (const auto& test : tests_) {
            std::cout << "[ RUN      ] " << test.name << "\n";
            try {
                test.func();
                std::cout << "[       OK ] " << test.name << "\n";
                passed++;
            } catch (const std::exception& e) {
                std::cout << "[  FAILED  ] " << test.name << "\n";
                std::cout << "  Error: " << e.what() << "\n";
                failed++;
            } catch (...) {
                std::cout << "[  FAILED  ] " << test.name << "\n";
                std::cout << "  Error: Unknown exception thrown\n";
                failed++;
            }
        }

        std::cout << "========================================\n";
        std::cout << " Test Summary: " << passed << " passed, " << failed << " failed, "
                  << tests_.size() << " total\n";
        std::cout << "========================================\n";
        return failed == 0 ? 0 : 1;
    }

private:
    std::vector<TestCase> tests_;
};

struct TestRegistrar {
    TestRegistrar(const std::string& name, std::function<void()> func) {
        TestRegistry::instance().addTest(name, std::move(func));
    }
};

#define GS_TEST(Suite, Name) \
    static void Suite##_##Name(); \
    static ::gamescript::test::TestRegistrar registrar_##Suite##_##Name(#Suite "." #Name, Suite##_##Name); \
    static void Suite##_##Name()

#define GS_ASSERT(cond) \
    do { \
        if (!(cond)) { \
            std::ostringstream oss; \
            oss << "Assertion failed: (" #cond ") at " << __FILE__ << ":" << __LINE__; \
            throw std::runtime_error(oss.str()); \
        } \
    } while (0)

#define GS_ASSERT_EQ(val1, val2) \
    do { \
        if (!((val1) == (val2))) { \
            std::ostringstream oss; \
            oss << "Assertion failed: " #val1 " == " #val2 " (actual: " << (val1) << " vs " << (val2) << ") at " << __FILE__ << ":" << __LINE__; \
            throw std::runtime_error(oss.str()); \
        } \
    } while (0)

#define GS_ASSERT_NE(val1, val2) \
    do { \
        if ((val1) == (val2)) { \
            std::ostringstream oss; \
            oss << "Assertion failed: " #val1 " != " #val2 " at " << __FILE__ << ":" << __LINE__; \
            throw std::runtime_error(oss.str()); \
        } \
    } while (0)

} // namespace gamescript::test
