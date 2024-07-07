#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

std::vector<uint64_t> comb(int N, int K)
{
    std::string bitmask(K, 1); // K leading 1's
    bitmask.resize(N, 0);      // N-K trailing 0's
    std::vector<uint64_t> result;

    // print integers and permute bitmask
    do
    {
        uint64_t bitset = 0;
        for (int i = 0; i < N; ++i) // [0..N-1] integers
        {
            if (bitmask[i])
                bitset |= 1 << i;
        }
        result.push_back(bitset);
    } while (std::prev_permutation(bitmask.begin(), bitmask.end()));

    return result;
}

int main()
{
    for (int i = 1; i < 6; i++)
    {
        std::cout << "bit count: " << i << std::endl;
        comb(5, i);
        std::cout << "----------------" << std::endl;
    }
}