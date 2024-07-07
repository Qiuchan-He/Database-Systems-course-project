#include "milestone3_utils.hpp"
#include "MyPlanEnumerator.hpp"
#include "nullstream.hpp"
#include <cmath>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutable/mutable.hpp>
#include <mutable/Options.hpp>
#include <random>
#include <sstream>
#include <utility>


using namespace m;

using PlanTable = m::PlanTableLargeAndSparse;


void run_benchmark(const char *name,
                   std::filesystem::path schema,
                   std::filesystem::path query,
                   std::filesystem::path cardinalities)
{
    using namespace std::chrono;

    Catalog::Clear();
    Catalog &C = Catalog::Get();
    NullStream devnull;
    Diagnostic diag(true, devnull, std::cerr);

    C.default_cardinality_estimator("Injected");
    const char *mutable_args[] = { "", "--use-cardinality-file", cardinalities.c_str(), nullptr };
    C.arg_parser().parse_args(4, mutable_args);
    M_insist(C.arg_parser().args().empty(), "not all arguments were handled by mutable");

    /*----- Process SCHEMA.sql -----*/
    m::execute_file(diag, schema);
    if (diag.num_errors())
        return;

    /*----- Read in QUERY.sql -----*/
    std::ifstream in(query);
    std::stringstream ss;
    ss << in.rdbuf();

    /* Convert input to query. */
    auto stmt = statement_from_string(diag, ss.str());
    if (diag.num_errors())
        return;

    auto G = QueryGraph::Build(*stmt);
    MyPlanEnumerator PE;
    auto &CF = C.cost_function(); // get default cost function (C_out)
    Optimizer O(PE, CF);

    /* Perform optimization. */
    const auto t_begin = steady_clock::now();
    auto [plan, PT] = O.optimize_with_plantable<PlanTable>(*G);
    const auto t_end = steady_clock::now();

    // PT.dump();

    const std::size_t cost = PT.get_final().cost;
    const auto ns = duration_cast<nanoseconds>(t_end - t_begin).count();
    std::cout << "milestone3," << name << ','
              << ns / 1e3 << ',' // µs
              << std::hex << cost << std::dec
              << '\n';
}

int main(int argc, char**)
{
    /* Check the number of parameters. */
    if (argc != 1)
        exit(EXIT_FAILURE);

    std::filesystem::path schema("resource/schema.sql");

#define RUN(NAME) \
    run_benchmark(NAME, schema, "resource/" NAME ".query.sql", "resource/" NAME ".cardinalities.json");
    RUN("chain-12");
    RUN("cycle-12");
    RUN("star-10");
    RUN("clique-10");
    RUN("clique-10_thinned-32");
#undef RUN
}
