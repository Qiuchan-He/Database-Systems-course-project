#include "BTree.hpp"
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <vector>


#ifndef NDEBUG
constexpr std::size_t num_entries = 1e6;
constexpr std::size_t num_point_lookups = 1e4;
#else
constexpr std::size_t num_entries = 1e8;
constexpr std::size_t num_point_lookups = 1e6;
#endif


template<typename Key, typename Generator>
std::vector<Key> gen_data(std::size_t count, Generator &&g)
{
    std::vector<Key> vec;
    vec.reserve(count);

    std::uniform_int_distribution<> dist_repetition(1, 10); // how often to repeat the same value

    int32_t current = 0;
    for (;;) {
        std::uniform_int_distribution<> dist_value(current + 1, current + 10); // the next value
        current = dist_value(g);

        std::size_t n = dist_repetition(g);
        while (n--) {
            vec.emplace_back(current);
            if (vec.size() == count)
                goto exit;
        }
    }
exit:

    assert(vec.size() == count);
    return vec;
};

template<typename Key, typename Generator>
std::vector<Key> gen_misses(std::size_t count, const std::vector<Key> &keys, Generator &&g)
{
    using std::begin, std::end;

    std::vector<Key> misses;
    misses.reserve(count);

    std::uniform_int_distribution<> dist_value(keys.front() - 10, keys.back() + 10);
    std::uniform_int_distribution<> dist_repetition(1, 10);

    for (;;) {
        /* Find a key that is not in `keys`. */
        Key key;
        do
            key = dist_value(g); // roll a key
        while (std::binary_search(begin(keys), end(keys), key)); // key exists?

        /* Append multiple repetitions of that key. */
        std::size_t n = dist_repetition(g);
        while (n--) {
            misses.emplace_back(key);
            if (misses.size() == count)
                goto exit;
        }
    }
exit:

    assert(misses.size() == count);
    std::shuffle(misses.begin(), misses.end(), g);
    return misses;
}

template<typename Key, typename Generator>
std::vector<Key> draw_lookup_keys(const std::vector<Key> &keys, const std::vector<Key> &misses,
                                  const float hit_ratio, const std::size_t count,
                                  Generator &&g)
{
    assert(0.f <= hit_ratio and hit_ratio <= 1.f);
    assert(misses.size() >= count);

    std::vector<Key> lookup_keys;
    lookup_keys.reserve(count);

    const std::size_t num_hits   = count * hit_ratio;
    const std::size_t num_misses = count - num_hits;

    /* Draw the keys that hit. */
    std::sample(keys.cbegin(), keys.cend(), std::back_inserter(lookup_keys), num_hits, g);

    /* Draw the keys that miss. */
    lookup_keys.insert(lookup_keys.cend(), misses.cbegin(), std::next(misses.cbegin(), num_misses));
    assert(lookup_keys.size() == count);

    std::shuffle(lookup_keys.begin(), lookup_keys.end(), g);

    return lookup_keys;
}

template<typename Key, typename Value, std::size_t NODE_SIZE, typename Generator>
void benchmark(
    const char *name,
    const std::vector<Key> &keys,
    const std::vector<std::pair<Key, Value>> &data,
    const std::vector<Key> &misses,
    Generator g
) {
    using tree_type = BTree<Key, Value, NODE_SIZE>;
    using namespace std::chrono;

    /*----- Bulkload data. -----*/
    const auto t_bulkload_begin = steady_clock::now();
    const auto tree = tree_type::Bulkload(data.cbegin(), data.cend());
    const auto t_bulkload_end = steady_clock::now();

    std::cout << "milestone2,bulkload_" << name << ','
              << duration_cast<milliseconds>(t_bulkload_end - t_bulkload_begin).count()
              << '\n';

    /*----- Benchmark `find()`. -----*/
    uint64_t checksum;
    for (const float hit_ratio : {.05f, .95f,}) {
        const auto lookup_keys = draw_lookup_keys(keys, misses, hit_ratio, num_point_lookups, g);
        checksum = 0;

        const auto t_lookup_begin = steady_clock::now();
        for (auto k : lookup_keys) {
            const auto it = tree.find(k);
            const uint64_t v = (it == tree.cend()) ? 1UL : (*it).second();
            checksum = (checksum << 3UL) ^ v;

        }
        const auto t_lookup_end = steady_clock::now();

        const auto ns = duration_cast<nanoseconds>(t_lookup_end - t_lookup_begin).count();
        std::cout << "milestone2,find_" << name << '_' << unsigned(100 * hit_ratio) << ','
                  << std::round(ns / double(num_point_lookups)) << ','
                  << std::hex << checksum << std::dec
                  << '\n';
    }
}

template<typename Key, typename Value, typename Generator>
void benchmark_all_node_sizes(const char *name, Generator g)
{
    std::ostringstream oss;

    /*----- Generate keys. -----*/
    const auto keys = gen_data<Key>(num_entries, g);
    assert(std::is_sorted(keys.begin(), keys.end()));

    /*----- Generate key-value pairs. -----*/
    std::vector<std::pair<Key, Value>> data;
    data.resize(keys.size());
    for (std::size_t i = 0; i != keys.size(); ++i)
        data.emplace_back(keys[i], Value(i));

    /*----- Generate misses. -----*/
    const auto misses = gen_misses(num_point_lookups, keys, g);

#define BENCHMARK(NODE_SIZE) { \
    oss.str(""); \
    oss << name << '_' << NODE_SIZE; \
    benchmark<Key, Value, NODE_SIZE>(oss.str().c_str(), keys, data, misses, g); \
}
    BENCHMARK(64);
    BENCHMARK(512);
    BENCHMARK(4096);
#undef BENCHMARK
}


int main()
{
#define BENCHMARK(KEY, VALUE) \
    benchmark_all_node_sizes<KEY, VALUE>(#KEY "__" #VALUE, std::mt19937(0))
    BENCHMARK(int32_t, int32_t);
#undef BENCHMARK
}
