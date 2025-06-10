#include "test.h"

#include <vector>
#include <list>
#include <set>
#include <map>
#include <string>


int main()
{
    // Test conversion of boolean false to std::vector<bool>
    test (KwargsValue value(false)) expect (value.value<std::vector<bool>>() == std::vector<bool>{false});

    // Test conversion of boolean true to std::vector<bool>
    test (KwargsValue value(true)) expect (value.value<std::vector<bool>>() == std::vector<bool>{true});

    // Test conversion of boolean true to std::vector<int>
    test (KwargsValue value(true)) expect (value.value<std::vector<int>>() == std::vector<int>{true});

    // Test conversion of integer to std::vector<bool>
    test (KwargsValue value(12345)) expect (value.value<std::vector<bool>>() == std::vector<bool>{true});

    // Test conversion of std::vector<int> to std::vector<int>
    test (KwargsValue value(std::vector({1, 2, 3, 4, 5}))) expect (value.value<std::vector<int>>() == std::vector({1, 2, 3, 4, 5}));

    // Test conversion of std::list<int> to std::list<int>
    test (KwargsValue value(std::list({4, 5, 6}))) expect (value.value<std::list<int>>() == std::list({4, 5, 6}));

    // Test conversion of std::set<int> to std::set<int>
    test (KwargsValue value(std::set({7, 8, 9}))) expect (value.value<std::set<int>>() == std::set({7, 8, 9}));

    // Test conversion of std::map<int, std::string> to std::map<int, std::string>
    test (KwargsValue value(std::map<int, std::string>({{1, "one"}, {2, "two"}}))) expect ((value.value<std::map<int, std::string>>()) == (std::map<int, std::string>({{1, "one"}, {2, "two"}})));

    // Test conversion of nested std::vector<std::vector<int>> to std::vector<std::vector<int>>
    test (KwargsValue value(std::vector<std::vector<int>>{{1, 2}, {3, 4}})) expect ((value.value<std::vector<std::vector<int>>>()) == std::vector<std::vector<int>>({{1, 2}, {3, 4}}));

    // Test conversion of empty std::vector<int> to std::vector<int>
    test (KwargsValue value(std::vector<int>{})) expect (value.value<std::vector<int>>() == std::vector<int>{});

    // Test conversion of empty std::list<std::string> to std::list<std::string>
    test (KwargsValue value(std::list<std::string>{})) expect (value.value<std::list<std::string>>() == std::list<std::string>{});

    // Test conversion of std::vector<int> to std::list<int>
    test (KwargsValue value(std::vector({1, 2, 3, 4, 5}))) expect (value.value<std::list<int>>() == std::list({1, 2, 3, 4, 5}));

    // Test conversion of std::vector<int> to std::list<int>
    test (KwargsValue value(std::vector({10, 20, 30}))) expect (value.value<std::list<int>>() == std::list({10, 20, 30}));

    // Test conversion of std::vector<int> to std::set<int>
    test (KwargsValue value(std::vector({10, 20, 30, 30}))) expect (value.value<std::set<int>>() == std::set({10, 20, 30}));

    // Test conversion of std::list<int> to std::vector<int>
    test (KwargsValue value(std::list({40, 50, 60}))) expect (value.value<std::vector<int>>() == std::vector({40, 50, 60}));

    // Test conversion of std::list<int> to std::set<int>
    test (KwargsValue value(std::list({70, 80, 80, 90}))) expect (value.value<std::set<int>>() == std::set({70, 80, 90}));

    // Test conversion of std::set<int> to std::vector<int>
    test (KwargsValue value(std::set({100, 200, 300}))) expect (value.value<std::vector<int>>() == std::vector({100, 200, 300}));

    // Test conversion of std::set<int> to std::list<int>
    test (KwargsValue value(std::set({400, 500, 600}))) expect (value.value<std::list<int>>() == std::list({400, 500, 600}));

    // Test conversion of nested std::vector<std::list<int>> to std::list<std::vector<int>>
    test (KwargsValue value(std::vector<std::list<int>>{{1, 2}, {3, 4}})) expect (value.value<std::list<std::vector<int>>>() == std::list<std::vector<int>>({{1, 2}, {3, 4}}));

    // Test conversion of nested std::list<std::set<int>> to std::set<std::list<int>>
    test (KwargsValue value(std::list<std::set<int>>{{5, 6}, {7, 8}})) expect (value.value<std::set<std::list<int>>>() == std::set<std::list<int>>({{5, 6}, {7, 8}}));

    // Test conversion of empty std::vector<int> to std::list<int>
    test (KwargsValue value(std::vector<int>{})) expect (value.value<std::list<int>>() == std::list<int>{});

    // Test conversion of empty std::set<std::string> to std::vector<std::string>
    test (KwargsValue value(std::set<std::string>{})) expect (value.value<std::vector<std::string>>() == std::vector<std::string>{});

    // Test conversion of std::map<int, std::string> to std::vector<std::pair<int, std::string>>
    test (KwargsValue value(std::map<int, std::string>{{1, "a"}, {2, "b"}})) expect ((value.value<std::vector<std::pair<int, std::string>>>()) == (std::vector<std::pair<int, std::string>>({{1, "a"}, {2, "b"}})));

    // Test conversion of std::vector<std::pair<int, std::string>> to std::map<int, std::string>
    test (KwargsValue value(std::vector<std::pair<int, std::string>>{{3, "c"}, {4, "d"}})) expect ((value.value<std::map<int, std::string>>()) == (std::map<int, std::string>{{3, "c"}, {4, "d"}}));

    return testing_completed;
}