#include "TestHarness.hpp"

int main() {
    return gamescript::test::TestRegistry::instance().runAll();
}
