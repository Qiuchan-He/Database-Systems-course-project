#include "data_layouts.hpp"
#include <cassert>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <mutable/util/macro.hpp>
#include <sstream>


#ifndef NDEBUG
constexpr int32_t NUM_TUPLES_RW = 5e5;
#else
constexpr int32_t NUM_TUPLES_RW = 5e6;
#endif


template<typename Layout>
void benchmark_store(const char *name)
{
    /* Clear the catalog before starting a new benchmark. */
    m::Catalog::Clear();

    /* Get a handle on the catalog. */
    auto &C = m::Catalog::Get();

    /* Create a `m::Diagnostic` object. */
    m::Diagnostic diag(true, std::cout, std::cerr);

    /* Use the Wasm backend for benchmarking. */
    C.default_backend("WasmV8");

    /* Register our store and set as default store. */
    C.register_data_layout(name, std::make_unique<Layout>(), name);
    C.default_data_layout(name);

    /* Create database 'dbsys' and select it. */
    auto &DB = C.add_database(C.pool("dbsys"));
    C.set_database_in_use(DB);

    /* Evaluate memory layout. */
    {
        /* Create a wide table to evaluate padding and alignment. */
        auto &tbl_wide = DB.add_table(C.pool("wide"));
        tbl_wide.push_back(C.pool("a_i4"), m::Type::Get_Integer(m::Type::TY_Vector, 4));
        tbl_wide.push_back(C.pool("b_b"),  m::Type::Get_Boolean(m::Type::TY_Vector));
        tbl_wide.push_back(C.pool("c_c3"), m::Type::Get_Char(m::Type::TY_Vector, 3));
        tbl_wide.push_back(C.pool("d_b"),  m::Type::Get_Boolean(m::Type::TY_Vector));
        tbl_wide.push_back(C.pool("e_d"),  m::Type::Get_Double(m::Type::TY_Vector));
        tbl_wide.push_back(C.pool("f_i1"), m::Type::Get_Integer(m::Type::TY_Vector, 1));
        tbl_wide.push_back(C.pool("g_f"),  m::Type::Get_Float(m::Type::TY_Vector));
        tbl_wide.push_back(C.pool("h_c5"), m::Type::Get_Char(m::Type::TY_Vector, 5));
        tbl_wide.push_back(C.pool("i_b"),  m::Type::Get_Boolean(m::Type::TY_Vector));
        tbl_wide.push_back(C.pool("j_i2"), m::Type::Get_Integer(m::Type::TY_Vector, 2));
        tbl_wide.push_back(C.pool("k_b"),  m::Type::Get_Boolean(m::Type::TY_Vector));
        tbl_wide.push_back(C.pool("l_i2"), m::Type::Get_Integer(m::Type::TY_Vector, 2));
        tbl_wide.store(C.create_store(tbl_wide));

        tbl_wide.store(C.create_store(tbl_wide));
        tbl_wide.layout(C.data_layout().make(tbl_wide.schema()));
        auto &layout = tbl_wide.layout();

        std::cout << "milestone1,size," << name << ',' << (layout.stride_in_bits() / layout.child().num_tuples()) << '\n';
    }

    /* Evaluate read/write performance - full table scan. */
    {
        /* Create a simple table to evaluate read performance. */
        auto &table = DB.add_table(C.pool("full_scan"));
        table.push_back(C.pool("key"),    m::Type::Get_Integer(m::Type::TY_Vector, 4));
        table.push_back(C.pool("value0"), m::Type::Get_Integer(m::Type::TY_Vector, 4));
        table.push_back(C.pool("value1"), m::Type::Get_Integer(m::Type::TY_Vector, 4));
        table.push_back(C.pool("value2"), m::Type::Get_Integer(m::Type::TY_Vector, 4));
        table.store(C.create_store(table));
        table.layout(C.data_layout().make(table.schema()));

        /* Get a handle on the backing store, create a writer, and an I/O tuple. */
        auto &store = table.store();
        m::StoreWriter W(store);
        m::Tuple tup(table.schema());

        for (int32_t i = 0; i != NUM_TUPLES_RW; ++i) {
            /* Set tuple data (i, 2*i). */
            tup.set(0, i);
            tup.set(1, i<<1);
            tup.set(2, i<<1);
            tup.set(3, i<<1);
            W.append(tup);
        }

        auto stmt = m::statement_from_string(diag, "SELECT key, value0, value1, value2 FROM full_scan;");
        auto query = m::as<m::ast::SelectStmt>(std::move(stmt));

        uint64_t checksum = 0;
        auto op = std::make_unique<m::CallbackOperator>([&checksum](const m::Schema&, const m::Tuple &T) {
                checksum += T.get(0).as_i() * 3;
                checksum += T.get(1).as_i() * 5;
                checksum += T.get(2).as_i() * 7;
                checksum += T.get(3).as_i() * 11;
        });

        using namespace std::chrono;
        auto t_read_begin = steady_clock::now();
        m::execute_query(diag, *query, std::move(op));
        auto t_read_end = steady_clock::now();

        std::cout << "milestone1,full_scan," << name << ','
                  << duration_cast<milliseconds>(t_read_end - t_read_begin).count() << ','
                  << std::hex << checksum << std::dec
                  << '\n';
    }

    /* Evaluate read/write performance - partial table scan. */
    {
        /* Create a simple table to evaluate read performance. */
        auto &table = DB.add_table(C.pool("partial_scan"));
        table.push_back(C.pool("key"),    m::Type::Get_Integer(m::Type::TY_Vector, 4));
        table.push_back(C.pool("value0"), m::Type::Get_Integer(m::Type::TY_Vector, 4));
        table.push_back(C.pool("value1"), m::Type::Get_Integer(m::Type::TY_Vector, 4));
        table.push_back(C.pool("value2"), m::Type::Get_Integer(m::Type::TY_Vector, 4));
        table.store(C.create_store(table));
        table.layout(C.data_layout().make(table.schema()));

        /* Get a handle on the backing store, create a writer, and an I/O tuple. */
        auto &store = table.store();
        m::StoreWriter W(store);
        m::Tuple tup(W.schema());

        for (int32_t i = 0; i != NUM_TUPLES_RW; ++i) {
            /* Set tuple data (i, 2*i). */
            tup.set(0, i);
            tup.set(1, i<<1);
            tup.set(2, i<<1);
            tup.set(3, i<<1);
            W.append(tup);
        }

        auto stmt = m::statement_from_string(diag, "SELECT key, value2 FROM partial_scan;");
        auto query = m::as<m::ast::SelectStmt>(std::move(stmt));

        uint64_t checksum = 0;
        auto op = std::make_unique<m::CallbackOperator>([&checksum](const m::Schema&, const m::Tuple &T) {
                checksum += T.get(0).as_i() * 3;
                checksum += T.get(1).as_i() * 5;
        });

        using namespace std::chrono;
        auto t_read_begin = steady_clock::now();
        m::execute_query(diag, *query, std::move(op));
        auto t_read_end = steady_clock::now();

        std::cout << "milestone1,partial_scan," << name << ','
                  << duration_cast<milliseconds>(t_read_end - t_read_begin).count() << ','
                  << std::hex << checksum << std::dec
                  << '\n';
    }
}

int main()
{
    benchmark_store<MyNaiveRowLayoutFactory>("row_naive");
    benchmark_store<MyOptimizedRowLayoutFactory>("row_optimized");
    benchmark_store<MyPAX4kLayoutFactory>("pax");
    m::Catalog::Destroy();
}
