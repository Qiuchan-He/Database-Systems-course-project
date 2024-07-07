#include "MyPlanEnumerator.hpp"
#include<unordered_set>

using namespace m;

struct iterator {
    std::vector<uint64_t> items;
    std::string bitmask;
    int N;

    iterator(int N, int K, std::vector<uint64_t>& items): N(N), bitmask(K, 1), items(items) {
        // bitmask(K, 1); // K leading 1's
        bitmask.resize(N, 0);      // N-K trailing 0's
    }

    std::vector<uint64_t> get_subproblem() {
        std::vector<uint64_t> result;
        for(int i = 0; i < N; i++){
            if (bitmask[i]) result.push_back(items[i]);
        }
        return result;
    }

    std::vector<uint64_t> get_inv_subproblem() {
        std::vector<uint64_t> result;
        for(int i = 0; i < N; i++){
            if (!bitmask[i]) result.push_back(items[i]);
        }
        return result;
    }
    
    uint64_t get_bitset() {
        uint64_t result;
        for(int i = 0; i < N; i++){
            if (bitmask[i]) result |= 1 << i;
        }
        return result;
    }


    bool advance(){
        return std::prev_permutation(bitmask.begin(), bitmask.end());
    }
};

m::SmallBitset get_bitset(std::vector<uint64_t> relations) {
    uint64_t bitset = 0;
    for(auto rel: relations)
        bitset |= 1 << rel;
    return m::SmallBitset(bitset);
}

template <typename PlanTable>
void MyPlanEnumerator::operator()(enumerate_tag, PlanTable &PT, const QueryGraph &G, const CostFunction &CF) const
{
    const AdjacencyMatrix &M = G.adjacency_matrix();
    auto &CE = Catalog::Get().get_database_in_use().cardinality_estimator();
    cnf::CNF condition; // Use this as join condition for PT.update(); we have fake cardinalities, so the condition
                        // doesn't matter.

    uint64_t num_relations = PT.num_sources();

    // std::cout << M << std::endl;

    std::vector<uint64_t> relations;
    // std::unordered_set<m::SmallBitset> vis;
    for(int i = 0; i<num_relations; i++){
        // vis.insert(m::SmallBitset(1 << i));
        relations.push_back(i);
    } 


    for (int problem_size = 2; problem_size <= num_relations; problem_size++)
    {
        iterator it = iterator(num_relations, problem_size, relations);
        do{
            std::vector<uint64_t> S = it.get_subproblem();
            for(int sub_problem_size = 1; sub_problem_size < problem_size; sub_problem_size++){
                iterator left_it = iterator(problem_size, sub_problem_size, S);
                // std::cout << PT << std::endl;
                do {
                    m::SmallBitset left = get_bitset(left_it.get_subproblem());
                    m::SmallBitset right = get_bitset(left_it.get_inv_subproblem());

                    if(!PT.has_plan(left)) continue;
                    if(!PT.has_plan(right)) continue;

                    if(M.is_connected(left, right)){
                        // std::cout << left << "\t" << right << std::endl;

                        PT.update(G, CE, CF, left, right, condition);
                        left |= right;
                        // vis.insert(left);
                    }
                }while (left_it.advance());
            }
        }while(it.advance());
    }

    // TODO 3: Implement algorithm for plan enumeration (join ordering).
}

template void MyPlanEnumerator::operator()<PlanTableSmallOrDense &>(enumerate_tag, PlanTableSmallOrDense &, const QueryGraph &, const CostFunction &) const;
template void MyPlanEnumerator::operator()<PlanTableLargeAndSparse &>(enumerate_tag, PlanTableLargeAndSparse &, const QueryGraph &, const CostFunction &) const;