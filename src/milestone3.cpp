#include "milestone3_utils.hpp"
#include "MyPlanEnumerator.hpp"
#include <cmath>
#include <iostream>
#include <memory>
#include <mutable/mutable.hpp>
#include <mutable/Options.hpp>
#include <random>
#include <sstream>
#include <utility>


using namespace m;

using PlanTable = m::PlanTableLargeAndSparse;


void usage(std::ostream &out, const char *name)
{
    out << "USAGE:\n    " << name << " <SCHEMA.sql> <QUERY.sql> <CARDINALITIES.json>" << std::endl;
}

int main(int argc, char *argv[])
{
    /* Check the number of parameters. */
    if (argc != 4) {
        usage(std::cerr, argv[0]);
        exit(EXIT_FAILURE);
    }

    Catalog &C = Catalog::Get();
    Diagnostic diag(true, std::cout, std::cerr);

    C.default_cardinality_estimator("Injected");
    const char *mutable_args[] = { argv[0], "--use-cardinality-file", argv[3], nullptr };
    C.arg_parser().parse_args(4, mutable_args);
    M_insist(C.arg_parser().args().empty(), "not all arguments were handled by mutable");

    /*----- Process SCHEMA.sql -----*/
    m::execute_file(diag, argv[1]);

    /*----- Read in QUERY.sql -----*/
    std::ifstream in(argv[2]);
    std::stringstream ss;
    ss << in.rdbuf();

    /* Convert input to query. */
    auto stmt = statement_from_string(diag, ss.str());
    auto G = QueryGraph::Build(*stmt);

    /*----- Show query graph. -----*/
    {
        DotTool dot(diag);
        G->dot(dot.stream());
        dot.show("graph", true, "fdp");
    }

    MyPlanEnumerator PE;
    auto &CF = C.cost_function(); // get default cost function (C_out)
    Optimizer O(PE, CF);

    /* Create base plans. */
    auto PT_in = get_plan_table<PlanTable>(*G);

    std::cout << "Initial plan table:\n" << PT_in << std::endl;

    /* Perform optimization. */
    auto [plan, PT_out] = O.optimize_with_plantable<PlanTable>(*G);

    std::cout << "Final plan table:\n" << PT_out << std::endl;

    /* Show the plan and plan table. */
    plan->minimize_schema();
    plan->dump(std::cout);

    {
        DotTool dot(diag);
        plan->dot(dot.stream());
        dot.show("plan", true);
    }
}
