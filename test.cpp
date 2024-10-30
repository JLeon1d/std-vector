#include <string>
#include <vector>
#include <random>
#include <iostream>
#include <cassert>
#include <algorithm>

#include "vector.h"

#define REQUIRE(x) assert(x)

void Check(const Vector<int>& actual, const std::vector<int>& expected) {
    REQUIRE(actual.size() == expected.size());
    for (size_t i = 0; i < actual.size(); ++i) {
        REQUIRE(actual[i] == expected[i]);
    }
}

void Constructors() {
    {
        Vector<int> a;
        REQUIRE(a.size() == 0u);
    }
    {
        Vector<int> a{1, 2, 3, 4};
        Check(a, std::vector<int>{1, 2, 3, 4});
    }
    {
        Vector<int> a(5);
        Check(a, std::vector<int>(5));
    }
}

void Basic() {
    Vector<int> a{1, 3, 5};
    Check(a, std::vector<int>{1, 3, 5});

    a.pop_back();
    Check(a, std::vector<int>{1, 3});
    a.push_back(1);
    Check(a, std::vector<int>{1, 3, 1});

    a.clear();
    Check(a, std::vector<int>());

    a.push_back(3);
    Vector<int> b{2, 4};
    a.swap(b);
    Check(a, std::vector<int>{2, 4});
    Check(b, std::vector<int>{3});
}

void ElementAccess() {
    Vector<int> a{9, 1, 1};
    a[0] = 1;
    a[1] = 2;
    a[2] = 3;
    Check(a, std::vector<int>{1, 2, 3});

    REQUIRE(a.at(0) == 1);
    a.at(1) = 4;
    REQUIRE(a.at(1) == 4);

    try {
        a.at(4) = 1;
        REQUIRE(false);
    } catch (std::out_of_range&) {
    }
}

void MemoryLayout() {
    Vector<int> a(129);
    for (int i = 0; i < 127; ++i) {
        REQUIRE(&a[i] + 1 == &a[i + 1]);
    }
}

void Copy() {
    Vector<int> a;
    Vector<int> b(a);
    b.push_back(1);
    Check(a, std::vector<int>());
    Check(b, std::vector<int>{1});
    b = b;  // NOLINT
    Check(b, std::vector<int>{1});
    a = b;
    Check(a, std::vector<int>{1});

    b = std::move(a);
    Check(b, std::vector<int>{1});
    Vector<int> c(std::move(b));
    Check(c, std::vector<int>{1});

    Vector<int> d{3, 4, 5};
    Vector<int> e(d);
    Check(e, std::vector<int>{3, 4, 5});
    d.swap(c);
    Check(e, std::vector<int>{3, 4, 5});
    Check(d, std::vector<int>{1});
    Check(c, std::vector<int>{3, 4, 5});
}

void Empty() {
    const size_t test_size = 1e3;
    {
        Vector<int> a;
        for (size_t idx = 0; idx < test_size; ++idx) {
            a.push_back(idx);
            REQUIRE(a.size() <= a.capacity());
        }
        for (size_t idx = 0; idx < test_size; ++idx) {
            a.pop_back();
        }
        REQUIRE(a.size() == 0u);
        Check(a, std::vector<int>());
    }
}

void Stress() {
    const int iterations = 1e5;
    Vector<int> a;
    std::vector<int> b;
    std::mt19937 rnd(4);
    for (size_t it = 0; it < iterations; ++it) {
        int op = rnd()%100;
        if (op <= 40) {
            auto elem = rnd();
            a.push_back(elem);
            b.push_back(elem);
        } else if (op <= 70) {
            if (b.empty()) {
                REQUIRE(a.empty());
                continue;
            }
            auto elem = rnd();
            auto id = rnd()%a.size();
            a[id] = elem;
            b[id] = elem;
        } else if (op <= 80) {
            if (b.empty()) {
                REQUIRE(a.empty());
                continue;
            }
            a.pop_back();
            b.pop_back();
        } else if (op <= 90) {
            a.shrink_to_fit();
            b.shrink_to_fit();
        } else if (op <= 98) {
            auto sz = 1 + rnd()%100;
            a.resize(sz);
            b.resize(sz);
        } else if (op == 99) {
            a.clear();
            b.clear();
        }
        Check(a, b);
    }
}

void Iterators() {

    Vector<int> a{1, 2, 3, 4};
    {
        auto it = a.begin();
        REQUIRE(*it == 1);
        ++it;
        REQUIRE(*it == 2);
        it += 2;
        REQUIRE(*it == 4);

        REQUIRE(a.begin() + 4 == a.end());
    }

    {
        auto it = a.end();
        --it;
        REQUIRE(*it == 4);
    }

    {
        std::vector<int> b;
        for (auto elem : a) {
            b.push_back(elem);
        }
        Check(a, b);

        for (auto& elem : a) {
            elem += 1;
        }

        Check(a, std::vector<int>{2, 3, 4, 5});
    }
}

void Assign() {
    Vector<int> a{1, 2, 3, 4};
    a.assign(10, 1);
    Check(a, std::vector<int>(10, 1));
}

void StressWithInsert() {
    const int iterations = 1e5;
    Vector<int> a;
    std::vector<int> b;
    std::mt19937 rnd(4);
    for (size_t it = 0; it < iterations; ++it) {
        int op = rnd()%100;
        if (op <= 10) {
            auto elem = rnd();
            a.push_back(elem);
            b.push_back(elem);
        } else if (op <= 20) {
            auto elem = rnd();
            a.emplace_back(elem);
            b.emplace_back(elem);
        } else if (op <= 40) {
            if (b.empty()) {
                continue;
            }
            auto elem = rnd();
            auto pos = rnd()%a.size();
            a.insert(a.begin() + pos, elem);
            b.insert(b.begin() + pos, elem);
        } else if (op <= 50) {
            if (b.empty()) {
                continue;
            }
            auto elem = rnd();
            auto id = rnd()%a.size();
            a[id] = elem;
            b[id] = elem;
        } else if(op <= 70) {
            if (b.empty()) {
                continue;
            }
            auto pos = rnd()%a.size();
            a.erase(a.begin() + pos);
            b.erase(b.begin() + pos);
        } else if (op <= 80) {
            if (b.empty()) {
                continue;
            }
            a.pop_back();
            b.pop_back();
        } else if (op <= 98) {
            auto sz = 1 + rnd()%100;
            a.resize(sz);
            b.resize(sz);
        } else if (op == 99) {
            a.clear();
            b.clear();
        }
        Check(a, b);
    }
}

void Equal() {
    Vector<int> a{1, 2, 3, 4};
    Vector<int> b{1, 2, 3, 4};
    REQUIRE(a == b);
    a.push_back(4);
    REQUIRE(a != b);
}

// g++ -fsanitize=address test.cpp -o test -O2 -std=c++20 && ./test
// g++ -fsanitize=undefined test_.cpp -o test -O2 -std=c++20 && ./test
int main() {
    Constructors();
    Basic();
    ElementAccess();
    MemoryLayout();
    Copy();
    Empty();
    Stress();
    Iterators();
    Assign();
    StressWithInsert();
    Equal();
}