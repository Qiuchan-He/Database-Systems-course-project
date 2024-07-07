#pragma once

#include <cstdint>
#include <mutable/mutable.hpp>


template<typename PlanTable>
PlanTable get_plan_table(const m::QueryGraph &G)
{
    m::Catalog &C = m::Catalog::Get();
    auto &DB = C.get_database_in_use();
    auto &CE = DB.cardinality_estimator();

    PlanTable PT(G.num_sources());
    for (auto &ds : G.sources()) {
        m::QueryGraph::Subproblem s;
        s.at(ds->id()) = true;
        PT[s].cost = 0;
        PT[s].model = CE.estimate_scan(G, s);
    }
    return PT;
}
