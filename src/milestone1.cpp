#include "data_layouts.hpp"
#include <cerrno>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <mutable/mutable.hpp>


int main(int argc, const char **argv)
{
    /* Check the number of parameters. */
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <Layout> <CSV-File> <SQL-File>" << std::endl;
        exit(EXIT_FAILURE);
    }

    /* Get a handle on the catalog. */
    auto &C = m::Catalog::Get();

    /* Create a `m::Diagnostic` object. */
    m::Diagnostic diag(true, std::cout, std::cerr);

    /* Register our store(s) and set the default store. */
    C.register_data_layout("row_naive", std::make_unique<MyNaiveRowLayoutFactory>(), "row layout (naïve)");
    C.register_data_layout("row_optimized", std::make_unique<MyOptimizedRowLayoutFactory>(), "row layout (optimized)");
    C.register_data_layout("PAX4k", std::make_unique<MyPAX4kLayoutFactory>(), "PAX layout with 4KiB blocks");

    /* Set default data layout. */
    C.default_data_layout(argv[1]);

    /* Create database 'dbsys' and select it. */
    auto &DB = C.add_database(C.pool("dbsys"));
    C.set_database_in_use(DB);

    /* Create table 'packages'. */
    auto &T = DB.add_table(C.pool("packages"));
    T.push_back(C.pool("id"),           m::Type::Get_Integer(m::Type::TY_Vector, 4));
    T.push_back(C.pool("repo"),         m::Type::Get_Char(m::Type::TY_Vector, 10));
    T.push_back(C.pool("pkg_name"),     m::Type::Get_Char(m::Type::TY_Vector, 32));
    T.push_back(C.pool("pkg_ver"),      m::Type::Get_Char(m::Type::TY_Vector, 20));
    T.push_back(C.pool("description"),  m::Type::Get_Char(m::Type::TY_Vector, 80));
    T.push_back(C.pool("licenses"),     m::Type::Get_Char(m::Type::TY_Vector, 32));
    T.push_back(C.pool("size"),         m::Type::Get_Integer(m::Type::TY_Vector, 8));
    T.push_back(C.pool("packager"),     m::Type::Get_Char(m::Type::TY_Vector, 32));

    /* Back the table with a store and set the data layout. */
    T.store(C.create_store(T));
    T.layout(C.data_layout());

    /* Load CSV file into table 'T'. */
    m::load_from_CSV(diag, T, argv[2], std::numeric_limits<std::size_t>::max(), true, false);

    if (diag.num_errors())
        exit(EXIT_FAILURE);

    /* Process the SQL file. */
    m::execute_file(diag, argv[3]);

    m::Catalog::Destroy();
    exit(EXIT_SUCCESS);
}
