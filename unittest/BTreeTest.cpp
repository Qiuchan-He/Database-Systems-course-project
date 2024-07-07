#include "catch2/catch.hpp"

#include "BTree.hpp"
#include <array>
#include <typeinfo>
#include <vector>


namespace {

template<typename key_type, typename value_type, std::size_t node_size>
void __test_bulkload()
{
    using tree_type = BTree<key_type, value_type, node_size>;
    using pair_type = std::pair<key_type, value_type>;

    SECTION("empty")
    {
        std::array<pair_type, 0> data;
        auto tree = tree_type::Bulkload(data.cbegin(), data.cend());

        CHECK(tree.size() == 0);
        CHECK(tree.height() == 0);

        auto it = tree.begin();
        CHECK(it == tree.end());
    }

    SECTION("N = 1")
    {
        std::array<pair_type, 1> data = { {
            { 42, 13 },
        } };
        auto tree = tree_type::Bulkload(data.cbegin(), data.cend());

        CHECK(tree.size() == 1);
        CHECK(tree.height() == 0);

        auto it = tree.begin();

        REQUIRE(it != tree.end());
        CHECK((*it).first()  == 42);
        CHECK((*it).second() == 13);
        ++it;

        CHECK(it == tree.end());
    }

    SECTION("N = 2")
    {
        std::array<pair_type, 2> data = { {
            { 7, 137 },
            { 42, 13 },
        } };
        auto tree = tree_type::Bulkload(data.cbegin(), data.cend());

        CHECK(tree.size() == 2);
        CHECK(tree.height() == 0);

        auto it = tree.begin();

        REQUIRE(it != tree.end());
        CHECK((*it).first()  ==   7);
        CHECK((*it).second() == 137);
        ++it;

        REQUIRE(it != tree.end());
        CHECK((*it).first()  == 42);
        CHECK((*it).second() == 13);
        ++it;

        CHECK(it == tree.end());
    }

    SECTION("N = 2133")
    {
        constexpr key_type N = 2133;
        std::vector<pair_type> data;
        data.reserve(N);
        for (key_type key = 0; key != N; ++key) {
            value_type val = 2 * key + 13;
            data.emplace_back(key, val);
        }

        auto tree = tree_type::Bulkload(data.cbegin(), data.cend());

        CHECK(tree.size() == N);
        // std::cout << "size check finished..." << std::endl;

        auto it = tree.cbegin();
        for (key_type key = 0; key != N; ++key, ++it) {
            REQUIRE(it != tree.cend());
            CHECK((*it).first() == key);
            CHECK((*it).second() == 2 * key + 13);
            // std::cout << "checked: " << (*it).first() <<", " << (*it).second() << std::endl;
        }
        // std::cout << "check done..." << std::endl;
        CHECK(it == tree.cend());
    }

    SECTION("N = 1e6")
    {
        constexpr key_type N = 1e6;
        std::vector<pair_type> data;
        data.reserve(N);
        for (key_type key = 0; key != N; ++key) {
            value_type val = 2 * key + 13;
            data.emplace_back(key, val);
        }
        // std::cout << "data creation complete..." << std::endl;


        auto tree = tree_type::Bulkload(data.cbegin(), data.cend());
        // std::cout << "bulkloading finished..." << std::endl;


        CHECK(tree.size() == N);
        // std::cout << "size check finished..." << std::endl;

        auto it = tree.cbegin();
        for (key_type key = 0; key != N; ++key, ++it) {
            REQUIRE(it != tree.cend());
            CHECK((*it).first() == key);
            CHECK((*it).second() == 2 * key + 13);
        }
        CHECK(it == tree.cend());
        // std::cout << "check done..." << std::endl;
    }
}

template<typename key_type, typename value_type, std::size_t node_size>
void __test_find()
{
    using tree_type = BTree<key_type, value_type, node_size>;
    using pair_type = std::pair<key_type, value_type>;

    SECTION("empty")
    {
        std::array<pair_type, 0> data;
        auto tree = tree_type::Bulkload(data.cbegin(), data.cend());

        auto it = tree.find(42);
        CHECK(it == tree.end());
    }

    SECTION("N = 1")
    {
        std::array<pair_type, 1> data = { {
            { 42, 13 },
        } };

        auto tree = tree_type::Bulkload(data.cbegin(), data.cend());

        {
            auto it = tree.find(42);
            REQUIRE(it != tree.end());
            CHECK((*it).first()  == 42);
            CHECK((*it).second() == 13);
            ++it;
            CHECK(it == tree.end());
        }

        {
            auto it = tree.find(0);
            CHECK(it == tree.end());
        }

        {
            auto it = tree.find(137);
            CHECK(it == tree.end());
        }
    }

    SECTION("N = 2")
    {
        std::array<pair_type, 2> data = { {
            {  42, 13 },
            { 137, 16 },
        } };

        auto tree = tree_type::Bulkload(data.cbegin(), data.cend());

        {
            auto it = tree.find(0);
            CHECK(it == tree.end());
        }

        {
            auto it = tree.find(42);
            REQUIRE(it != tree.end());
            CHECK((*it).first()  == 42);
            CHECK((*it).second() == 13);
        }

        {
            auto it = tree.find(64);
            CHECK(it == tree.end());
        }

        {
            auto it = tree.find(137);
            REQUIRE(it != tree.end());
            CHECK((*it).first()  == 137);
            CHECK((*it).second() ==  16);
        }

        {
            auto it = tree.find(1024);
            CHECK(it == tree.end());
        }
    }

    SECTION("N = 100")
    {
        constexpr key_type N = 100;
        std::vector<pair_type> data;
        for (key_type key = 0; key != N; ++key) {
            value_type val = 2 * key + 13;
            data.emplace_back(key, val);
        }

        auto tree = tree_type::Bulkload(data.cbegin(), data.cend());

        for (key_type key = 0; key != N; ++key) {
            auto it = tree.find(key);
            REQUIRE(it != tree.end());
            CHECK((*it).first() == key);
            CHECK((*it).second() == 2 * key + 13);
        }

        {
            auto it = tree.find(-1);
            CHECK(it == tree.end());
        }

        {
            auto it = tree.find(N);
            CHECK(it == tree.end());
        }
    }

    SECTION("N = 1e6")
    {
        constexpr key_type N = 1e6;

        std::vector<pair_type> data;
        for (key_type key = 0; key != N; ++key) {
            value_type val = 2 * key + 13;
            data.emplace_back(key, val);
        }

        auto tree = tree_type::Bulkload(data.cbegin(), data.cend());

        for (key_type key = N / 2, end = key + 1000; key != end; ++key) {
            auto it = tree.find(key);
            REQUIRE(it != tree.end());
            CHECK((*it).first() == key);
            CHECK((*it).second() == 2 * key + 13);
        }

        {
            auto it = tree.find(-1);
            CHECK(it == tree.end());
        }

        {
            auto it = tree.find(N);
            CHECK(it == tree.end());
        }
    }
}

template<typename key_type, typename value_type, std::size_t node_size>
void __test_find_range()
{
    using tree_type = BTree<key_type, value_type, node_size>;
    using pair_type = std::pair<key_type, value_type>;

    SECTION("empty")
    {
        std::array<pair_type, 0> data;

        auto tree = tree_type::Bulkload(data.cbegin(), data.cend());

        auto range = tree.find_range(0, 42);
        CHECK(range.empty());
    }

    SECTION("N = 1")
    {
        std::array<pair_type, 1> data = { {
            { 42, 13 },
        } };
        auto tree = tree_type::Bulkload(data.cbegin(), data.cend());

        {
            auto range = tree.find_range(0, 42);
            CHECK(range.empty());
        }

        {
            auto range = tree.find_range(42, 43);
            REQUIRE_FALSE(range.empty());
            auto it = range.begin();
            CHECK((*it).first()  == 42);
            CHECK((*it).second() == 13);
            ++it;
            CHECK(it == range.end());
            CHECK(it == tree.cend());
        }

        {
            auto range = tree.find_range(43, 100);
            REQUIRE(range.empty());
        }
    }

    SECTION("N = 2")
    {
        std::array<pair_type, 2> data = { {
            {  42, 13 },
            { 137, 16 },
        } };
        auto tree = tree_type::Bulkload(data.cbegin(), data.cend());

        {
            auto range = tree.find_range(0, 42);
            CHECK(range.empty());
        }

        {
            auto range = tree.find_range(42, 43);
            REQUIRE_FALSE(range.empty());
            auto it = range.begin();
            CHECK((*it).first()  == 42);
            CHECK((*it).second() == 13);
            ++it;
            CHECK(it == range.end());
            CHECK(it != tree.end());
        }

        {
            auto range = tree.find_range(43, 137);
            CHECK(range.empty());
        }

        {
            auto range = tree.find_range(137, 138);
            REQUIRE_FALSE(range.empty());
            auto it = range.begin();
            CHECK((*it).first()  == 137);
            CHECK((*it).second() ==  16);
            ++it;
            CHECK(it == range.end());
            CHECK(it == tree.end());
        }

        {
            auto range = tree.find_range(138, 200);
            CHECK(range.empty());
        }

        {
            auto range = tree.find_range(42, 138);
            REQUIRE_FALSE(range.empty());
            auto it = range.begin();
            CHECK((*it).first()  == 42);
            CHECK((*it).second() == 13);
            ++it;
            REQUIRE(it != range.end());
            CHECK((*it).first()  == 137);
            CHECK((*it).second() ==  16);
            ++it;
            CHECK(it == range.end());
            CHECK(it == tree.end());
        }
    }

    SECTION("N = 100")
    {
        constexpr key_type N = 100;

        std::vector<pair_type> data;
        for (key_type key = 0; key != N; ++key) {
            data.emplace_back(key, 2 * key + 13);
        }
        auto tree = tree_type::Bulkload(data.cbegin(), data.cend());

        for (key_type key = 0; key != N; ++key) {
            auto range = tree.find_range(key, key+1);
            REQUIRE_FALSE(range.empty());
            auto it = range.begin();
            REQUIRE(it != range.end());
            CHECK((*it).first()  == key);
            CHECK((*it).second() == 2 * key + 13);
            ++it;
            CHECK(it == range.end());
        }

        {
            auto range = tree.find_range(-100, 0);
            REQUIRE(range.empty());
        }

        {
            auto range = tree.find_range(100, 200);
            REQUIRE(range.empty());
        }

        {
            auto range = tree.find_range(0, 100);
            REQUIRE_FALSE(range.empty());

            auto it = range.begin();
            for (key_type key = 0; key != N; ++key, ++it) {
                REQUIRE(it != range.end());
                CHECK((*it).first()  == key);
                CHECK((*it).second() == 2 * key + 13);
            }
            CHECK(it == range.end());
            CHECK(it == tree.end());
        }
    }

    SECTION("N = 1e6")
    {
        constexpr key_type N = 1e6;

        std::vector<pair_type> data;
        for (key_type key = 0; key != N; ++key)
            data.emplace_back(key, 2 * key + 13);

        auto tree = tree_type::Bulkload(data.cbegin(), data.cend());

        for (key_type key = N/2, end = key + 1000; key != end; ++key) {
            auto range = tree.find_range(key, key+2);
            REQUIRE_FALSE(range.empty());
            auto it = range.begin();

            REQUIRE(it != range.end());
            CHECK((*it).first()  == key);
            CHECK((*it).second() == 2 * key + 13);
            ++it;

            REQUIRE(it != range.end());
            CHECK((*it).first()  == (key + 1));
            CHECK((*it).second() == 2 * (key + 1) + 13);
            ++it;

            CHECK(it == range.end());
        }
    }
}

template<typename key_type, typename value_type, std::size_t node_size>
void __test_equal_range()
{
    using tree_type = BTree<key_type, value_type, node_size>;
    using pair_type = std::pair<key_type, value_type>;

    SECTION("empty")
    {
        std::array<pair_type, 0> data;

        auto tree = tree_type::Bulkload(data.cbegin(), data.cend());

        auto range = tree.equal_range(42);
        CHECK(range.empty());
    }

    SECTION("N = 1")
    {
        std::array<pair_type, 1> data = { {
            { 42, 13 },
        } };
        auto tree = tree_type::Bulkload(data.cbegin(), data.cend());

        {
            auto range = tree.equal_range(41);
            CHECK(range.empty());
        }

        {
            auto range = tree.equal_range(42);
            REQUIRE_FALSE(range.empty());
            auto it = range.begin();
            CHECK((*it).first()  == 42);
            CHECK((*it).second() == 13);
            ++it;
            CHECK(it == range.end());
            CHECK(it == tree.cend());
        }

        {
            auto range = tree.equal_range(43);
            REQUIRE(range.empty());
        }
    }

    SECTION("N = 2")
    {
        std::array<pair_type, 2> data = { {
            {  42, 13 },
            { 137, 16 },
        } };
        auto tree = tree_type::Bulkload(data.cbegin(), data.cend());

        {
            auto range = tree.equal_range(41);
            CHECK(range.empty());
        }

        {
            auto range = tree.equal_range(42);
            REQUIRE_FALSE(range.empty());
            auto it = range.begin();
            CHECK((*it).first()  == 42);
            CHECK((*it).second() == 13);
            ++it;
            CHECK(it == range.end());
            CHECK(it != tree.end());
        }

        {
            auto range = tree.equal_range(43);
            CHECK(range.empty());
        }

        {
            auto range = tree.equal_range(137);
            REQUIRE_FALSE(range.empty());
            auto it = range.begin();
            CHECK((*it).first()  == 137);
            CHECK((*it).second() ==  16);
            ++it;
            CHECK(it == range.end());
            CHECK(it == tree.end());
        }

        {
            auto range = tree.equal_range(138);
            CHECK(range.empty());
        }
    }

    SECTION("N = 10")
    {
        std::array<pair_type, 10> data = { {
            { 1, 1 },
            { 1, 2 },
            { 1, 3 },
            { 2, 1 },
            { 2, 2 },
            { 3, 1 },
            { 4, 1 },
            { 4, 2 },
            { 5, 1 },
            { 8, 1 },
        } };

        auto tree = tree_type::Bulkload(data.cbegin(), data.cend());

        {
            auto range = tree.equal_range(0);
            REQUIRE(range.empty());
        }

        {
            auto range = tree.equal_range(1);
            REQUIRE_FALSE(range.empty());
            auto it = range.begin();

            REQUIRE(it != range.end());
            CHECK((*it).first() == 1);
            CHECK((*it).second() == 1);
            ++it;

            REQUIRE(it != range.end());
            CHECK((*it).first() == 1);
            CHECK((*it).second() == 2);
            ++it;

            REQUIRE(it != range.end());
            CHECK((*it).first() == 1);
            CHECK((*it).second() == 3);
            ++it;

            CHECK(it == range.end());
            CHECK(it != tree.end());
        }

        {
            auto range = tree.equal_range(2);
            REQUIRE_FALSE(range.empty());
            auto it = range.begin();

            REQUIRE(it != range.end());
            CHECK((*it).first() == 2);
            CHECK((*it).second() == 1);
            ++it;

            REQUIRE(it != range.end());
            CHECK((*it).first() == 2);
            CHECK((*it).second() == 2);
            ++it;

            CHECK(it == range.end());
            CHECK(it != tree.end());
        }

        {
            auto range = tree.equal_range(3);
            REQUIRE_FALSE(range.empty());
            auto it = range.begin();

            REQUIRE(it != range.end());
            CHECK((*it).first() == 3);
            CHECK((*it).second() == 1);
            ++it;

            CHECK(it == range.end());
            CHECK(it != tree.end());
        }

        {
            auto range = tree.equal_range(6);
            REQUIRE(range.empty());
        }

        {
            auto range = tree.equal_range(8);
            REQUIRE_FALSE(range.empty());
            auto it = range.begin();

            REQUIRE(it != range.end());
            CHECK((*it).first() == 8);
            CHECK((*it).second() == 1);
            ++it;

            CHECK(it == range.end());
            CHECK(it == tree.end());
        }
    }

    SECTION("N = 2'000")
    {
        constexpr key_type N = 2'000;
        constexpr unsigned REP_COUNT = 4;
        constexpr key_type n = N / REP_COUNT;

        std::vector<pair_type> data;
        data.reserve(N);
        for (key_type key = 0; key != n; ++key) {
            for (key_type v = 0; v != REP_COUNT; ++v)
                data.emplace_back(key, v);
        }

        auto tree = tree_type::Bulkload(data.cbegin(), data.cend());

        for (key_type key = n / 2, end = key + 10; key != end; ++key) {
            DYNAMIC_SECTION("key = " << key) {
                auto range = tree.equal_range(key);
                REQUIRE_FALSE(range.empty());

                auto it = range.begin();

                REQUIRE(it != range.end());
                CHECK((*it).first() == key);
                CHECK((*it).second() == 0);
                ++it;

                REQUIRE(it != range.end());
                CHECK((*it).first() == key);
                CHECK((*it).second() == 1);
                ++it;

                REQUIRE(it != range.end());
                CHECK((*it).first() == key);
                CHECK((*it).second() == 2);
                ++it;

                REQUIRE(it != range.end());
                CHECK((*it).first() == key);
                CHECK((*it).second() == 3);
                ++it;

                CHECK(it == range.end());
            }
        }
    }
}

}


TEST_CASE("BTree/node size", "[milestone2]")
{
    auto test = []<typename key_type, typename value_type, std::size_t node_size>() {
        using tree_type = BTree<key_type, value_type, node_size>;
        using INode = typename tree_type::INode;
        using Leaf = typename tree_type::Leaf;
        CHECK(sizeof(INode) <= node_size);
        CHECK(sizeof(Leaf) <= node_size);
        CHECK(alignof(INode) >= node_size);
        CHECK(alignof(Leaf) >= node_size);
    };
#define TEST(KEY, VALUE, NODE_SIZE) \
    DYNAMIC_SECTION((#KEY " -> " #VALUE ", " #NODE_SIZE "B")) { test.template operator()<KEY, VALUE, NODE_SIZE>(); }
    TEST(int32_t, int32_t, 4096);
    TEST(int64_t, int32_t, 4096);
    TEST(int32_t, int64_t, 4096);
    TEST(int64_t, int64_t, 4096);

    TEST(int32_t, int32_t, 64);
    TEST(int64_t, int32_t, 64);
    TEST(int32_t, int64_t, 64);
    TEST(int64_t, int64_t, 64);

    TEST(int32_t, int32_t, 512);
    TEST(int64_t, int32_t, 512);
    TEST(int32_t, int64_t, 512);
    TEST(int64_t, int64_t, 512);
#undef TEST
}


TEST_CASE("BTree/Bulkload", "[milestone2]")
{
#define TEST(KEY, VALUE, NODE_SIZE) \
    DYNAMIC_SECTION((#KEY " -> " #VALUE ", " #NODE_SIZE "B")) \
    { __test_bulkload<KEY, VALUE, NODE_SIZE>(); }

    TEST(int32_t, int32_t, 4096);
    TEST(int64_t, int64_t, 4096);

    TEST(int32_t, int32_t, 64);
    TEST(int64_t, int64_t, 64);

#undef TEST
}

TEST_CASE("BTree/find", "[milestone2]")
{
#define TEST(KEY, VALUE, NODE_SIZE) \
    DYNAMIC_SECTION((#KEY " -> " #VALUE ", " #NODE_SIZE "B")) \
    { __test_find<KEY, VALUE, NODE_SIZE>(); }

    TEST(int32_t, int32_t, 4096);
    TEST(int64_t, int64_t, 4096);

    TEST(int32_t, int32_t, 64);
    TEST(int64_t, int64_t, 64);

#undef TEST
}

TEST_CASE("BTree/find_range", "[milestone2]")
{
#define TEST(KEY, VALUE, NODE_SIZE) \
    DYNAMIC_SECTION((#KEY " -> " #VALUE ", " #NODE_SIZE "B")) \
    { __test_find_range<KEY, VALUE, NODE_SIZE>(); }

    TEST(int32_t, int32_t, 4096);
    TEST(int64_t, int64_t, 4096);

    TEST(int32_t, int32_t, 64);
    TEST(int64_t, int64_t, 64);

#undef TEST
}

TEST_CASE("BTree/equal_range", "[milestone2]")
{
#define TEST(KEY, VALUE, NODE_SIZE) \
    DYNAMIC_SECTION((#KEY " -> " #VALUE ", " #NODE_SIZE "B")) \
    { __test_equal_range<KEY, VALUE, NODE_SIZE>(); }

    TEST(int32_t, int32_t, 4096);
    TEST(int64_t, int64_t, 4096);

    TEST(int32_t, int32_t, 64);
    TEST(int64_t, int64_t, 64);

#undef TEST
}
