#include "catch2/catch.hpp"

#include "milestone3_utils.hpp"
#include "MyPlanEnumerator.hpp"
#include "nullstream.hpp"
#include <initializer_list>
#include <memory>
#include <sstream>
#include <streambuf>
#include <string>
#include <utility>


using namespace m;


using PlanTable = m::PlanTableLargeAndSparse;
using table_list = std::initializer_list<const char*>;
using cardinality_entry = std::pair<table_list, std::size_t>;
using cardinality_list = std::initializer_list<cardinality_entry>;

struct PT_entry
{
    PlanTable::Subproblem S;
    std::size_t size;
    PlanTable::Subproblem S1;
    PlanTable::Subproblem S2;
};

struct CardinalityWriter
{
    private:
    std::stringstream &ss;
    bool empty = true;

    public:
    CardinalityWriter(std::stringstream &ss, const char *db_name)
        : ss(ss)
    {
        ss << "{\n  \"" << db_name << "\": [";
    }

    ~CardinalityWriter() {
        ss << "\n  ]\n}\n";
    }

    void operator()(std::initializer_list<const char*> tables, std::size_t cardinality) {
        if (empty) {
            empty = false;
        } else {
            ss << ',';
        }

        ss << "\n    { \"relations\": [";
        for (auto it = tables.begin(); it != tables.end(); ++it) {
            if (it != tables.begin())
                ss << ", ";
            ss << '"' << *it << '"';
        }
        ss << "], \"size\": " << cardinality << " }";
    }
};

void write_cardinalities(std::stringstream &ss, const char *db_name, cardinality_list cardinalities)
{
    ss.str(""); // clear stream
    CardinalityWriter W(ss, db_name);
    for (cardinality_entry e : cardinalities)
        W(e.first, e.second);
}

void run(Diagnostic &diag, const char *sql)
{
    auto stmt = m::statement_from_string(diag, sql);
    m::execute_statement(diag, *stmt);
}

TEST_CASE("MyPlanEnumerator", "[milestone3]")
{
    /*----- Prepare database. ----------------------------------------------------------------------------------------*/
    Catalog::Clear();
    Catalog &C = Catalog::Get();
    NullStream devnull;
    m::Diagnostic diag(false, devnull, std::cerr);

    run(diag, "CREATE DATABASE test;");
    run(diag, "USE test;");
    run(diag, "\
CREATE TABLE T (\n\
    id      INT(4),\n\
    fid_T0  INT(4),\n\
    fid_T1  INT(4),\n\
    fid_T2  INT(4),\n\
    fid_T3  INT(4),\n\
    fid_T4  INT(4),\n\
    fid_T5  INT(4)\n\
);");

    const Subproblem None;
    const Subproblem T0(1UL << 0U);
    const Subproblem T1(1UL << 1U);
    const Subproblem T2(1UL << 2U);
    const Subproblem T3(1UL << 3U);
    const Subproblem T4(1UL << 4U);
    const Subproblem T5(1UL << 5U);

    std::stringstream cardinalities;


    /*----- Define test configurations. ------------------------------------------------------------------------------*/
    const char *query_str = nullptr;
    std::vector<PT_entry> expected;
    std::size_t expected_cost = 0;

    SECTION("no join")
    {
        query_str = "\
                     SELECT 1\n\
                     FROM T AS T0\n\
                     ;";

        write_cardinalities(cardinalities, "test", {
            { { "T0" }, 1337 },
        });

        expected.emplace_back(
            PT_entry { .S = T0, .size = 1337, .S1 = None, .S2 = None }
        );
    }

    SECTION("single join")
    {
        query_str = "\
                     SELECT 1\n\
                     FROM T AS T0, T AS T1\n\
                     WHERE T0.fid_T1 = T1.id\n\
                     ;";

        write_cardinalities(cardinalities, "test", {
            { { "T0" }, 10 },
            { { "T1" }, 5 },
            { { "T0", "T1" }, 20 },
        });

        expected.emplace_back(PT_entry { .S = T0, .size = 10, .S1 = None, .S2 = None });
        expected.emplace_back(PT_entry { .S = T1, .size = 5, .S1 = None, .S2 = None });
        expected.emplace_back(PT_entry { .S = T0|T1, .size = 20, .S1 = T0, .S2 = T1 });

        expected_cost = 20;
    }

    SECTION("chain-3")
    {
        query_str = "\
                     SELECT 1\n\
                     FROM T AS T0, T AS T1, T AS T2\n\
                     WHERE T0.fid_T1 = T1.id\n\
                       AND T1.fid_T2 = T2.id\n\
                     ;";

        SECTION("T0 ⋈  (T1 ⋈  T2)")
        {
            write_cardinalities(cardinalities, "test", {
                { { "T0" }, 5 },
                { { "T1" }, 20 },
                { { "T2" }, 8 },
                { { "T0", "T1" }, 90 },
                { { "T1", "T2" }, 4 },
                { { "T0", "T1", "T2" }, 7 },
            });

            expected.emplace_back(PT_entry { .S = T0, .size = 5, .S1 = None, .S2 = None });
            expected.emplace_back(PT_entry { .S = T1, .size = 20, .S1 = None, .S2 = None });
            expected.emplace_back(PT_entry { .S = T2, .size = 8, .S1 = None, .S2 = None });

            expected.emplace_back(PT_entry { .S = T1|T2, .size = 4, .S1 = T1, .S2 = T2 });
            expected.emplace_back(PT_entry { .S = T0|T1|T2, .size = 7, .S1 = T0, .S2 = T1|T2 });

            expected_cost = 11;
        }

        SECTION("(T0 ⋈  T1) ⋈  T2")
        {
            write_cardinalities(cardinalities, "test", {
                { { "T0" }, 5 },
                { { "T1" }, 20 },
                { { "T2" }, 8 },
                { { "T0", "T1" }, 7 },
                { { "T1", "T2" }, 110 },
                { { "T0", "T1", "T2" }, 7 },
            });

            expected.emplace_back(PT_entry { .S = T0, .size = 5, .S1 = None, .S2 = None });
            expected.emplace_back(PT_entry { .S = T1, .size = 20, .S1 = None, .S2 = None });
            expected.emplace_back(PT_entry { .S = T2, .size = 8, .S1 = None, .S2 = None });

            expected.emplace_back(PT_entry { .S = T0|T1, .size = 7, .S1 = T0, .S2 = T1 });
            expected.emplace_back(PT_entry { .S = T0|T1|T2, .size = 7, .S1 = T0|T1, .S2 = T2 });

            expected_cost = 14;
        }
    }

    SECTION("cycle-3")
    {
        query_str = "\
                     SELECT 1\n\
                     FROM T AS T0, T AS T1, T AS T2\n\
                     WHERE T0.fid_T1 = T1.id\n\
                       AND T1.fid_T2 = T2.id\n\
                       AND T2.fid_T0 = T0.id\n\
                     ;";

        SECTION("(T0 ⋈  T1) ⋈  T2")
        {
            write_cardinalities(cardinalities, "test", {
                { { "T0" }, 5 },
                { { "T1" }, 20 },
                { { "T2" }, 8 },
                { { "T0", "T1" }, 17 },
                { { "T1", "T2" }, 56 },
                { { "T0", "T2" }, 24 },
                { { "T0", "T1", "T2" }, 7 },
            });

            expected.emplace_back(PT_entry { .S = T0, .size = 5, .S1 = None, .S2 = None });
            expected.emplace_back(PT_entry { .S = T1, .size = 20, .S1 = None, .S2 = None });
            expected.emplace_back(PT_entry { .S = T2, .size = 8, .S1 = None, .S2 = None });

            expected.emplace_back(PT_entry { .S = T0|T1, .size = 17, .S1 = T0, .S2 = T1 });
            expected.emplace_back(PT_entry { .S = T0|T1|T2, .size = 7, .S1 = T0|T1, .S2 = T2 });

            expected_cost = 24;
        }

        SECTION("(T0 ⋈  T2) ⋈  T1")
        {
            write_cardinalities(cardinalities, "test", {
                { { "T0" }, 5 },
                { { "T1" }, 20 },
                { { "T2" }, 8 },
                { { "T0", "T1" }, 90 },
                { { "T1", "T2" }, 56 },
                { { "T0", "T2" }, 24 },
                { { "T0", "T1", "T2" }, 7 },
            });

            expected.emplace_back(PT_entry { .S = T0, .size = 5, .S1 = None, .S2 = None });
            expected.emplace_back(PT_entry { .S = T1, .size = 20, .S1 = None, .S2 = None });
            expected.emplace_back(PT_entry { .S = T2, .size = 8, .S1 = None, .S2 = None });

            expected.emplace_back(PT_entry { .S = T0|T2, .size = 24, .S1 = T0, .S2 = T2 });
            expected.emplace_back(PT_entry { .S = T0|T1|T2, .size = 7, .S1 = T0|T2, .S2 = T1 });

            expected_cost = 31;
        }
    }

    SECTION("star-5")
    {
        query_str = "\
                     SELECT 1\n\
                     FROM T AS T0, T AS T1, T AS T2, T AS T3, T AS T4\n\
                     WHERE T0.fid_T1 = T1.id\n\
                       AND T0.fid_T2 = T2.id\n\
                       AND T0.fid_T3 = T3.id\n\
                       AND T0.fid_T4 = T4.id\n\
                     ;";

        SECTION("(((T0 ⋈  T2) ⋈  T4) ⋈  T1) ⋈  T3")
        {
            write_cardinalities(cardinalities, "test", {
                { { "T0" }, 17 },
                { { "T1" }, 76 },
                { { "T2" }, 32 },
                { { "T3" }, 91 },
                { { "T4" }, 6 },

                { { "T0", "T1" }, 91 },
                { { "T0", "T2" }, 2 },
                { { "T0", "T3" }, 222 },
                { { "T0", "T4" }, 8 },

                { { "T0", "T1", "T2" }, 3 },
                { { "T0", "T1", "T3" }, 15 },
                { { "T0", "T1", "T4" }, 4 },
                { { "T0", "T2", "T3" }, 27 },
                { { "T0", "T2", "T4" }, 2 },
                { { "T0", "T3", "T4" }, 39 },

                { { "T0", "T1", "T2", "T3" }, 11 },
                { { "T0", "T1", "T2", "T4" }, 3 },
                { { "T0", "T1", "T3", "T4" }, 56 },
                { { "T0", "T2", "T3", "T4" }, 4 },

                { { "T0", "T1", "T2", "T3", "T4" }, 46 },
            });

            expected.emplace_back(PT_entry { .S = T0, .size = 17, .S1 = None, .S2 = None });
            expected.emplace_back(PT_entry { .S = T1, .size = 76, .S1 = None, .S2 = None });
            expected.emplace_back(PT_entry { .S = T2, .size = 32, .S1 = None, .S2 = None });
            expected.emplace_back(PT_entry { .S = T3, .size = 91, .S1 = None, .S2 = None });
            expected.emplace_back(PT_entry { .S = T4, .size = 6, .S1 = None, .S2 = None });

            expected.emplace_back(PT_entry { .S = T0|T2, .size = 2, .S1 = T0, .S2 = T2 });
            expected.emplace_back(PT_entry { .S = T0|T2|T4, .size = 2, .S1 = T0|T2, .S2 = T4 });
            expected.emplace_back(PT_entry { .S = T0|T1|T2|T4, .size = 3, .S1 = T0|T2|T4, .S2 = T1 });
            expected.emplace_back(PT_entry { .S = T0|T1|T2|T3|T4, .size = 46, .S1 = T0|T1|T2|T4, .S2 = T3 });

            expected_cost = 53;
        }

        SECTION("(((T0 ⋈  T4) ⋈  T1) ⋈  T3) ⋈  T2")
        {
            write_cardinalities(cardinalities, "test", {
                { { "T0" }, 90 },
                { { "T1" }, 81 },
                { { "T2" }, 5 },
                { { "T3" }, 21 },
                { { "T4" }, 2 },

                { { "T0", "T1" }, 364 },
                { { "T0", "T2" }, 10 },
                { { "T0", "T3" }, 21 },
                { { "T0", "T4" }, 3 },

                { { "T0", "T1", "T2" }, 564 },
                { { "T0", "T1", "T3" }, 60 },
                { { "T0", "T1", "T4" }, 2 },
                { { "T0", "T2", "T3" }, 14 },
                { { "T0", "T2", "T4" }, 3 },
                { { "T0", "T3", "T4" }, 4 },

                { { "T0", "T1", "T2", "T3" }, 2 },
                { { "T0", "T1", "T2", "T4" }, 9 },
                { { "T0", "T1", "T3", "T4" }, 2 },
                { { "T0", "T2", "T3", "T4" }, 2 },

                { { "T0", "T1", "T2", "T3", "T4" }, 2 },
            });

            expected.emplace_back(PT_entry { .S = T0, .size = 90, .S1 = None, .S2 = None });
            expected.emplace_back(PT_entry { .S = T1, .size = 81, .S1 = None, .S2 = None });
            expected.emplace_back(PT_entry { .S = T2, .size = 5, .S1 = None, .S2 = None });
            expected.emplace_back(PT_entry { .S = T3, .size = 21, .S1 = None, .S2 = None });
            expected.emplace_back(PT_entry { .S = T4, .size = 2, .S1 = None, .S2 = None });

            expected.emplace_back(PT_entry { .S = T0|T4, .size = 3, .S1 = T0, .S2 = T4 });
            expected.emplace_back(PT_entry { .S = T0|T1|T4, .size = 2, .S1 = T0|T4, .S2 = T1 });
            expected.emplace_back(PT_entry { .S = T0|T1|T3|T4, .size = 2, .S1 = T0|T1|T4, .S2 = T3 });
            expected.emplace_back(PT_entry { .S = T0|T1|T2|T3|T4, .size = 2, .S1 = T0|T1|T3|T4, .S2 = T2 });

            expected_cost = 9;
        }
    }

    SECTION("clique-4")
    {
        query_str = "\
                     SELECT 1\n\
                     FROM T AS T0, T AS T1, T AS T2, T AS T3\n\
                     WHERE T0.fid_T1 = T1.id\n\
                       AND T0.fid_T2 = T2.id\n\
                       AND T0.fid_T3 = T3.id\n\
                       AND T1.fid_T2 = T2.id\n\
                       AND T1.fid_T3 = T3.id\n\
                       AND T2.fid_T3 = T3.id\n\
                     ;";

        SECTION("(T1 ⋈  T2) ⋈  (T0 ⋈  T3)")
        {
            write_cardinalities(cardinalities, "test", {
                { { "T0" }, 70 },
                { { "T1" }, 46 },
                { { "T2" }, 58 },
                { { "T3" }, 52 },

                { { "T0", "T1" }, 123 },
                { { "T0", "T2" }, 3572 },
                { { "T0", "T3" }, 1521 },
                { { "T1", "T2" }, 1060 },
                { { "T1", "T3" }, 1133 },
                { { "T2", "T3" }, 2663 },

                { { "T0", "T1", "T2" }, 3897 },
                { { "T0", "T1", "T3" }, 6389 },
                { { "T0", "T2", "T3" }, 5677 },
                { { "T1", "T2", "T3" }, 8909 },

                { { "T0", "T1", "T2", "T3" }, 991 },
            });

            expected.emplace_back(PT_entry { .S = T0, .size = 70, .S1 = None, .S2 = None });
            expected.emplace_back(PT_entry { .S = T1, .size = 46, .S1 = None, .S2 = None });
            expected.emplace_back(PT_entry { .S = T2, .size = 58, .S1 = None, .S2 = None });
            expected.emplace_back(PT_entry { .S = T3, .size = 52, .S1 = None, .S2 = None });

            expected.emplace_back(PT_entry { .S = T1|T2, .size = 1060, .S1 = T1, .S2 = T2 });
            expected.emplace_back(PT_entry { .S = T0|T3, .size = 1521, .S1 = T0, .S2 = T3 });
            expected.emplace_back(PT_entry { .S = T0|T1|T2|T3, .size = 991, .S1 = T0|T3, .S2 = T1|T2 });

            expected_cost = 991 + 1060 + 1521;
        }
    }


    /*----- Perform tests. -------------------------------------------------------------------------------------------*/
    auto &DB = C.get_database_in_use();
    {
        auto CE = std::make_unique<InjectionCardinalityEstimator>(diag, C.pool("test"), cardinalities);
        DB.cardinality_estimator(std::move(CE));
    }
    auto &CE = DB.cardinality_estimator();

    auto query = m::statement_from_string(diag, query_str);
    auto G = QueryGraph::Build(*query);

    auto PT_initial = get_plan_table<PlanTable>(*G);

    MyPlanEnumerator PE;
    auto &CF = C.cost_function(); // get default cost function (C_out)
    Optimizer O(PE, CF);
    auto [_, PT_out] = O.optimize_with_plantable<PlanTable>(*G);

    // PT_out.dump();

    CHECK(PT_out.get_final().cost == expected_cost);

    for (const PT_entry &exp : expected) {
        auto &actual = PT_out[exp.S];
        REQUIRE(bool(actual.model));
        CHECK(CE.predict_cardinality(*actual.model) == exp.size);

        if (not ((actual.left == exp.S1 and actual.right == exp.S2) or
                 (actual.left == exp.S2 and actual.right == exp.S1)))
        {
            std::cerr << "actual join is ";
            actual.left.print_fixed_length(std::cerr, G->num_sources());
            std::cerr << " ⋈  ";
            actual.right.print_fixed_length(std::cerr, G->num_sources());
            std::cerr << ", expected ";
            exp.S1.print_fixed_length(std::cerr, G->num_sources());
            std::cerr << " ⋈  ";
            exp.S2.print_fixed_length(std::cerr, G->num_sources());
            CHECK(false);
        }
    }
}
