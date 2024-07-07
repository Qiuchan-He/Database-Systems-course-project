#include <mutable/mutable.hpp>


struct MyPlanEnumerator final : m::PlanEnumeratorCRTP<MyPlanEnumerator>
{
    using base_type = m::PlanEnumeratorCRTP<MyPlanEnumerator>;
    using base_type::operator();

    template<typename PlanTable>
    void operator()(m::enumerate_tag, PlanTable &PT, const m::QueryGraph &G, const m::CostFunction &CF) const;
};
