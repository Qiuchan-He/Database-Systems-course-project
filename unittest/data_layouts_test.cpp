#include <catch2/catch.hpp>

#include "data_layouts.hpp"


using namespace m;
using namespace m::storage;


TEST_CASE("NaiveRowLayout", "[milestone1]")
{
    Catalog::Clear(); // drop all data
    auto &C = m::Catalog::Get();

    try {
        C.register_data_layout("row_naive", std::make_unique<MyNaiveRowLayoutFactory>(), "row layout (naïve)");
    } catch (std::invalid_argument) { }

    auto &DB = C.add_database(C.pool("test_db"));
    auto &table = DB.add_table(C.pool("test"));

    SECTION("INT(4)")
    {
        /* Fill table with attributes. */
        table.push_back(C.pool("a"), m::Type::Get_Integer(m::Type::TY_Vector, 4));

        /* Create store and data layout. */
        table.store(C.create_store(table));
        table.layout(C.data_layout("row_naive"));
        const auto &layout = table.layout();

        /* Root must be an indefinite sequence of rows. */
        CHECK(not layout.is_finite());

        /* Check stride of row. */
        CHECK(layout.stride_in_bits() == 64);

        auto &child_node = layout.child();

        /* Check that the child INode models a single row. */
        CHECK(child_node.num_tuples() == 1);

        /* Check that child node is an INode. */
        auto inode = cast<const DataLayout::INode>(&child_node);
        REQUIRE(inode);

        /* Validate the INode. */
        CHECK(inode->num_children() == 2);
        CHECK(inode->at(0).offset_in_bits == 0);
        CHECK(inode->at(0).stride_in_bits == 0);
        CHECK(inode->at(1).offset_in_bits == 32);
        CHECK(inode->at(1).stride_in_bits == 0);

        /* Validate the data leaf. */
        auto attr_leaf = cast<const DataLayout::Leaf>(inode->at(0).ptr.get());
        REQUIRE(attr_leaf);
        CHECK(attr_leaf->num_tuples() == 1);
        CHECK(attr_leaf->index() == 0);
        CHECK(attr_leaf->type()->is_integral());
        CHECK(attr_leaf->type()->size() == 32);

        /* Validate the NULL bitmap. */
        auto null_bitmap = cast<const DataLayout::Leaf>(inode->at(1).ptr.get());
        REQUIRE(null_bitmap);
        CHECK(null_bitmap->num_tuples() == 1);
        CHECK(null_bitmap->index() == 1);
        CHECK(null_bitmap->type()->is_bitmap());
        CHECK(null_bitmap->type()->size() == 1);
    }

    SECTION("DOUBLE")
    {
        /* Fill table with attributes. */
        table.push_back(C.pool("a"), m::Type::Get_Double(m::Type::TY_Vector));

        /* Create store and data layout. */
        table.store(C.create_store(table));
        table.layout(C.data_layout("row_naive"));
        const auto &layout = table.layout();

        /* Root must be an indefinite sequence of rows. */
        CHECK(not layout.is_finite());

        /* Check stride of row. */
        CHECK(layout.stride_in_bits() == 128);

        auto &child_node = layout.child();

        /* Check that the child INode models a single row. */
        CHECK(child_node.num_tuples() == 1);

        /* Check that child node is an INode. */
        auto inode = cast<const DataLayout::INode>(&child_node);
        REQUIRE(inode);

        /* Validate the INode. */
        CHECK(inode->num_children() == 2);
        CHECK(inode->at(0).offset_in_bits == 0);
        CHECK(inode->at(0).stride_in_bits == 0);
        CHECK(inode->at(1).offset_in_bits == 64);
        CHECK(inode->at(1).stride_in_bits == 0);

        /* Validate the data leaf. */
        auto attr_leaf = cast<const DataLayout::Leaf>(inode->at(0).ptr.get());
        REQUIRE(attr_leaf);
        CHECK(attr_leaf->num_tuples() == 1);
        CHECK(attr_leaf->index() == 0);
        CHECK(attr_leaf->type()->is_double());
        CHECK(attr_leaf->type()->size() == 64);

        /* Validate the NULL bitmap. */
        auto null_bitmap = cast<const DataLayout::Leaf>(inode->at(1).ptr.get());
        REQUIRE(null_bitmap);
        CHECK(null_bitmap->num_tuples() == 1);
        CHECK(null_bitmap->index() == 1);
        CHECK(null_bitmap->type()->is_bitmap());
        CHECK(null_bitmap->type()->size() == 1);
    }

    SECTION("INT(2)")
    {
        /* Fill table with attributes. */
        table.push_back(C.pool("a"), m::Type::Get_Integer(m::Type::TY_Vector, 2));

        /* Create store and data layout. */
        table.store(C.create_store(table));
        table.layout(C.data_layout("row_naive"));
        const auto &layout = table.layout();

        /* Root must be an indefinite sequence of rows. */
        CHECK(not layout.is_finite());

        /* Check stride of row. */
        CHECK(layout.stride_in_bits() == 32);

        auto &child_node = layout.child();

        /* Check that the child INode models a single row. */
        CHECK(child_node.num_tuples() == 1);

        /* Check that child node is an INode. */
        auto inode = cast<const DataLayout::INode>(&child_node);
        REQUIRE(inode);

        /* Validate the INode. */
        CHECK(inode->num_children() == 2);
        CHECK(inode->at(0).offset_in_bits == 0);
        CHECK(inode->at(0).stride_in_bits == 0);
        CHECK(inode->at(1).offset_in_bits == 16);
        CHECK(inode->at(1).stride_in_bits == 0);

        /* Validate the data leaf. */
        auto attr_leaf = cast<const DataLayout::Leaf>(inode->at(0).ptr.get());
        REQUIRE(attr_leaf);
        CHECK(attr_leaf->num_tuples() == 1);
        CHECK(attr_leaf->index() == 0);
        CHECK(attr_leaf->type()->is_integral());
        CHECK(attr_leaf->type()->size() == 16);

        /* Validate the NULL bitmap. */
        auto null_bitmap = cast<const DataLayout::Leaf>(inode->at(1).ptr.get());
        REQUIRE(null_bitmap);
        CHECK(null_bitmap->num_tuples() == 1);
        CHECK(null_bitmap->index() == 1);
        CHECK(null_bitmap->type()->is_bitmap());
        CHECK(null_bitmap->type()->size() == 1);
    }

    SECTION("CHAR(3)")
    {
        /* Fill table with attributes. */
        table.push_back(C.pool("a"), m::Type::Get_Char(m::Type::TY_Vector, 3));

        /* Create store and data layout. */
        table.store(C.create_store(table));
        table.layout(C.data_layout("row_naive"));
        const auto &layout = table.layout();

        /* Root must be an indefinite sequence of rows. */
        CHECK(not layout.is_finite());

        /* Check stride of row. */
        CHECK(layout.stride_in_bits() == 32);

        auto &child_node = layout.child();

        /* Check that the child INode models a single row. */
        CHECK(child_node.num_tuples() == 1);

        /* Check that child node is an INode. */
        auto inode = cast<const DataLayout::INode>(&child_node);
        REQUIRE(inode);

        /* Validate the INode. */
        CHECK(inode->num_children() == 2);
        CHECK(inode->at(0).offset_in_bits == 0);
        CHECK(inode->at(0).stride_in_bits == 0);
        CHECK(inode->at(1).offset_in_bits == 24);
        CHECK(inode->at(1).stride_in_bits == 0);

        /* Validate the data leaf. */
        auto attr_leaf = cast<const DataLayout::Leaf>(inode->at(0).ptr.get());
        REQUIRE(attr_leaf);
        CHECK(attr_leaf->num_tuples() == 1);
        CHECK(attr_leaf->index() == 0);
        CHECK(attr_leaf->type()->is_character_sequence());
        CHECK(attr_leaf->type()->size() == 24);

        /* Validate the NULL bitmap. */
        auto null_bitmap = cast<const DataLayout::Leaf>(inode->at(1).ptr.get());
        REQUIRE(null_bitmap);
        CHECK(null_bitmap->num_tuples() == 1);
        CHECK(null_bitmap->index() == 1);
        CHECK(null_bitmap->type()->is_bitmap());
        CHECK(null_bitmap->type()->size() == 1);
    }

    SECTION("BOOL")
    {
        /* Fill table with attributes. */
        table.push_back(C.pool("a"), m::Type::Get_Boolean(m::Type::TY_Vector));

        /* Create store and data layout. */
        table.store(C.create_store(table));
        table.layout(C.data_layout("row_naive"));
        const auto &layout = table.layout();

        /* Root must be an indefinite sequence of rows. */
        CHECK(not layout.is_finite());

        /* Check stride of row. */
        CHECK(layout.stride_in_bits() == 8);

        auto &child_node = layout.child();

        /* Check that the child INode models a single row. */
        CHECK(child_node.num_tuples() == 1);

        /* Check that child node is an INode. */
        auto inode = cast<const DataLayout::INode>(&child_node);
        REQUIRE(inode);

        /* Validate the INode. */
        CHECK(inode->num_children() == 2);
        CHECK(inode->at(0).offset_in_bits == 0);
        CHECK(inode->at(0).stride_in_bits == 0);
        CHECK(inode->at(1).offset_in_bits == 1);
        CHECK(inode->at(1).stride_in_bits == 0);

        /* Validate the data leaf. */
        auto attr_leaf = cast<const DataLayout::Leaf>(inode->at(0).ptr.get());
        REQUIRE(attr_leaf);
        CHECK(attr_leaf->num_tuples() == 1);
        CHECK(attr_leaf->index() == 0);
        CHECK(attr_leaf->type()->is_boolean());
        CHECK(attr_leaf->type()->size() == 1);

        /* Validate the NULL bitmap. */
        auto null_bitmap = cast<const DataLayout::Leaf>(inode->at(1).ptr.get());
        REQUIRE(null_bitmap);
        CHECK(null_bitmap->num_tuples() == 1);
        CHECK(null_bitmap->index() == 1);
        CHECK(null_bitmap->type()->is_bitmap());
        CHECK(null_bitmap->type()->size() == 1);
    }

    SECTION("five booleans")
    {
        /* Fill table with attributes. */
        table.push_back(C.pool("a"), m::Type::Get_Boolean(m::Type::TY_Vector));
        table.push_back(C.pool("b"), m::Type::Get_Boolean(m::Type::TY_Vector));
        table.push_back(C.pool("c"), m::Type::Get_Boolean(m::Type::TY_Vector));
        table.push_back(C.pool("d"), m::Type::Get_Boolean(m::Type::TY_Vector));
        table.push_back(C.pool("e"), m::Type::Get_Boolean(m::Type::TY_Vector));

        /* Create store and data layout. */
        table.store(C.create_store(table));
        table.layout(C.data_layout("row_naive"));
        const auto &layout = table.layout();

        /* Root must be an indefinite sequence of rows. */
        CHECK(not layout.is_finite());

        /* Check stride of row. */
        CHECK(layout.stride_in_bits() == 16);

        auto &child_node = layout.child();

        /* Check that the child INode models a single row. */
        CHECK(child_node.num_tuples() == 1);

        /* Check that child node is an INode. */
        auto inode = cast<const DataLayout::INode>(&child_node);
        REQUIRE(inode);

        /* Validate the INode. */
        CHECK(inode->num_children() == 6);
        CHECK(inode->at(0).offset_in_bits == 0);
        CHECK(inode->at(0).stride_in_bits == 0);
        CHECK(inode->at(1).offset_in_bits == 1);
        CHECK(inode->at(1).stride_in_bits == 0);
        CHECK(inode->at(2).offset_in_bits == 2);
        CHECK(inode->at(2).stride_in_bits == 0);
        CHECK(inode->at(3).offset_in_bits == 3);
        CHECK(inode->at(3).stride_in_bits == 0);
        CHECK(inode->at(4).offset_in_bits == 4);
        CHECK(inode->at(4).stride_in_bits == 0);
        CHECK(inode->at(5).offset_in_bits == 5);
        CHECK(inode->at(5).stride_in_bits == 0);

        /* Validate the first boolean. */
        auto b0 = cast<const DataLayout::Leaf>(inode->at(0).ptr.get());
        REQUIRE(b0);
        CHECK(b0->num_tuples() == 1);
        CHECK(b0->index() == 0);
        CHECK(b0->type()->is_boolean());
        CHECK(b0->type()->size() == 1);

        /* Validate the first boolean. */
        auto b1 = cast<const DataLayout::Leaf>(inode->at(1).ptr.get());
        REQUIRE(b1);
        CHECK(b1->num_tuples() == 1);
        CHECK(b1->index() == 1);
        CHECK(b1->type()->is_boolean());
        CHECK(b1->type()->size() == 1);

        /* Validate the second boolean. */
        auto b2 = cast<const DataLayout::Leaf>(inode->at(2).ptr.get());
        REQUIRE(b2);
        CHECK(b2->num_tuples() == 1);
        CHECK(b2->index() == 2);
        CHECK(b2->type()->is_boolean());
        CHECK(b2->type()->size() == 1);

        /* Validate the third boolean. */
        auto b3 = cast<const DataLayout::Leaf>(inode->at(3).ptr.get());
        REQUIRE(b3);
        CHECK(b3->num_tuples() == 1);
        CHECK(b3->index() == 3);
        CHECK(b3->type()->is_boolean());
        CHECK(b3->type()->size() == 1);

        /* Validate the fourth boolean. */
        auto b4 = cast<const DataLayout::Leaf>(inode->at(4).ptr.get());
        REQUIRE(b4);
        CHECK(b4->num_tuples() == 1);
        CHECK(b4->index() == 4);
        CHECK(b4->type()->is_boolean());
        CHECK(b4->type()->size() == 1);

        /* Validate the NULL bitmap. */
        auto null_bitmap = cast<const DataLayout::Leaf>(inode->at(5).ptr.get());
        REQUIRE(null_bitmap);
        CHECK(null_bitmap->num_tuples() == 1);
        CHECK(null_bitmap->index() == 5);
        CHECK(null_bitmap->type()->is_bitmap());
        CHECK(null_bitmap->type()->size() == 5);
    }

    SECTION("simple table")
    {
        /* Fill table with attributes. */
        table.push_back(C.pool("id"), m::Type::Get_Integer(m::Type::TY_Vector, 4));
        table.push_back(C.pool("name"), m::Type::Get_Char(m::Type::TY_Vector, 20));
        table.push_back(C.pool("cakeday"), m::Type::Get_Date(m::Type::TY_Vector));
        table.push_back(C.pool("in_assessment"), m::Type::Get_Boolean(m::Type::TY_Vector));
        table.push_back(C.pool("salary"), m::Type::Get_Double(m::Type::TY_Vector));

        /* Create store and data layout. */
        table.store(C.create_store(table));
        table.layout(C.data_layout("row_naive"));
        const auto &layout = table.layout();

        /* Root must be an indefinite sequence of rows. */
        CHECK(not layout.is_finite());

        /* Check stride of row. */
        CHECK(layout.stride_in_bits() == 384);

        auto &child_node = layout.child();

        /* Check that the child INode models a single row. */
        CHECK(child_node.num_tuples() == 1);

        /* Check that child node is an INode. */
        auto inode = cast<const DataLayout::INode>(&child_node);
        REQUIRE(inode);

        /* Validate the INode. */
        CHECK(inode->num_children() == 6);
        /* id */
        CHECK(inode->at(0).offset_in_bits == 0);
        CHECK(inode->at(0).stride_in_bits == 0);
        /* name */
        CHECK(inode->at(1).offset_in_bits == 32);
        CHECK(inode->at(1).stride_in_bits == 0);
        /* cakeday */
        CHECK(inode->at(2).offset_in_bits == 192);
        CHECK(inode->at(2).stride_in_bits == 0);
        /* in_assessment */
        CHECK(inode->at(3).offset_in_bits == 224);
        CHECK(inode->at(3).stride_in_bits == 0);
        /* salary */
        CHECK(inode->at(4).offset_in_bits == 256);
        CHECK(inode->at(4).stride_in_bits == 0);
        /* NULL bitmap */
        CHECK(inode->at(5).offset_in_bits == 320);
        CHECK(inode->at(5).stride_in_bits == 0);

        /* Validate `id`. */
        auto attr_id = cast<const DataLayout::Leaf>(inode->at(0).ptr.get());
        REQUIRE(attr_id);
        CHECK(attr_id->num_tuples() == 1);
        CHECK(attr_id->index() == 0);
        CHECK(attr_id->type()->is_integral());
        CHECK(attr_id->type()->size() == 32);

        /* Validate `name`. */
        auto attr_name = cast<const DataLayout::Leaf>(inode->at(1).ptr.get());
        REQUIRE(attr_name);
        CHECK(attr_name->num_tuples() == 1);
        CHECK(attr_name->index() == 1);
        CHECK(attr_name->type()->is_character_sequence());
        CHECK(attr_name->type()->size() == 160);

        /* Validate `cakeday`. */
        auto attr_cakeday = cast<const DataLayout::Leaf>(inode->at(2).ptr.get());
        REQUIRE(attr_cakeday);
        CHECK(attr_cakeday->num_tuples() == 1);
        CHECK(attr_cakeday->index() == 2);
        CHECK(attr_cakeday->type()->is_date());
        CHECK(attr_cakeday->type()->size() == 32);

        /* Validate `in_assessment`. */
        auto attr_in_assessment = cast<const DataLayout::Leaf>(inode->at(3).ptr.get());
        REQUIRE(attr_in_assessment);
        CHECK(attr_in_assessment->num_tuples() == 1);
        CHECK(attr_in_assessment->index() == 3);
        CHECK(attr_in_assessment->type()->is_boolean());
        CHECK(attr_in_assessment->type()->size() == 1);

        /* Validate `salary`. */
        auto attr_salary = cast<const DataLayout::Leaf>(inode->at(4).ptr.get());
        REQUIRE(attr_salary);
        CHECK(attr_salary->num_tuples() == 1);
        CHECK(attr_salary->index() == 4);
        CHECK(attr_salary->type()->is_double());
        CHECK(attr_salary->type()->size() == 64);

        /* Validate the NULL bitmap. */
        auto null_bitmap = cast<const DataLayout::Leaf>(inode->at(5).ptr.get());
        REQUIRE(null_bitmap);
        CHECK(null_bitmap->num_tuples() == 1);
        CHECK(null_bitmap->index() == 5);
        CHECK(null_bitmap->type()->is_bitmap());
        CHECK(null_bitmap->type()->size() == 5);
    }

    SECTION("wide table")
    {
        table.push_back(C.pool("a_i4"), m::Type::Get_Integer(m::Type::TY_Vector, 4));   // 0:32
        table.push_back(C.pool("b_b"),  m::Type::Get_Boolean(m::Type::TY_Vector));      // 32:33
        table.push_back(C.pool("c_c3"), m::Type::Get_Char(m::Type::TY_Vector, 3));      // 40:64
        table.push_back(C.pool("d_b"),  m::Type::Get_Boolean(m::Type::TY_Vector));      // 64:65
        table.push_back(C.pool("e_d"),  m::Type::Get_Double(m::Type::TY_Vector));       // 128:192
        table.push_back(C.pool("f_i1"), m::Type::Get_Integer(m::Type::TY_Vector, 1));   // 192:200
        table.push_back(C.pool("g_f"),  m::Type::Get_Float(m::Type::TY_Vector));        // 224:256
        table.push_back(C.pool("h_c5"), m::Type::Get_Char(m::Type::TY_Vector, 5));      // 256:296
        table.push_back(C.pool("i_b"),  m::Type::Get_Boolean(m::Type::TY_Vector));      // 296:297
        table.push_back(C.pool("j_i2"), m::Type::Get_Integer(m::Type::TY_Vector, 2));   // 304:320
        table.push_back(C.pool("k_b"),  m::Type::Get_Boolean(m::Type::TY_Vector));      // 320:321
        table.push_back(C.pool("l_i2"), m::Type::Get_Integer(m::Type::TY_Vector, 2));   // 336:352
        // NULL bitmap: 352:364

        /* Create store and data layout. */
        table.store(C.create_store(table));
        table.layout(C.data_layout("row_naive"));
        const auto &layout = table.layout();

        /* Root must be an indefinite sequence of rows. */
        CHECK(not layout.is_finite());

        /* Check stride of row. */
        CHECK(layout.stride_in_bits() == 384);

        auto &child_node = layout.child();

        /* Check that the child INode models a single row. */
        CHECK(child_node.num_tuples() == 1);

        /* Check that child node is an INode. */
        auto inode = cast<const DataLayout::INode>(&child_node);
        REQUIRE(inode);

        /* Validate the INode. */
        CHECK(inode->num_children() == 13);
        /* a_i4 */
        CHECK(inode->at(0).offset_in_bits == 0);
        CHECK(inode->at(0).stride_in_bits == 0);
        /* b_b */
        CHECK(inode->at(1).offset_in_bits == 32);
        CHECK(inode->at(1).stride_in_bits == 0);
        /* c_c3 */
        CHECK(inode->at(2).offset_in_bits == 40);
        CHECK(inode->at(2).stride_in_bits == 0);
        /* d_b */
        CHECK(inode->at(3).offset_in_bits == 64);
        CHECK(inode->at(3).stride_in_bits == 0);
        /* e_d */
        CHECK(inode->at(4).offset_in_bits == 128);
        CHECK(inode->at(4).stride_in_bits == 0);
        /* f_i1 */
        CHECK(inode->at(5).offset_in_bits == 192);
        CHECK(inode->at(5).stride_in_bits == 0);
        /* g_f */
        CHECK(inode->at(6).offset_in_bits == 224);
        CHECK(inode->at(6).stride_in_bits == 0);
        /* h_c5 */
        CHECK(inode->at(7).offset_in_bits == 256);
        CHECK(inode->at(7).stride_in_bits == 0);
        /* i_b */
        CHECK(inode->at(8).offset_in_bits == 296);
        CHECK(inode->at(8).stride_in_bits == 0);
        /* j_i2 */
        CHECK(inode->at(9).offset_in_bits == 304);
        CHECK(inode->at(9).stride_in_bits == 0);
        /* k_b */
        CHECK(inode->at(10).offset_in_bits == 320);
        CHECK(inode->at(10).stride_in_bits == 0);
        /* l_i2 */
        CHECK(inode->at(11).offset_in_bits == 336);
        CHECK(inode->at(11).stride_in_bits == 0);

        auto attr_a_i4 = cast<const DataLayout::Leaf>(inode->at(0).ptr.get());
        REQUIRE(attr_a_i4);
        CHECK(attr_a_i4->num_tuples() == 1);
        CHECK(attr_a_i4->index() == 0);
        CHECK(attr_a_i4->type()->is_integral());
        CHECK(attr_a_i4->type()->size() == 32);

        auto attr_b_b = cast<const DataLayout::Leaf>(inode->at(1).ptr.get());
        REQUIRE(attr_b_b);
        CHECK(attr_b_b->num_tuples() == 1);
        CHECK(attr_b_b->index() == 1);
        CHECK(attr_b_b->type()->is_boolean());
        CHECK(attr_b_b->type()->size() == 1);

        auto attr_c_c3 = cast<const DataLayout::Leaf>(inode->at(2).ptr.get());
        REQUIRE(attr_c_c3);
        CHECK(attr_c_c3->num_tuples() == 1);
        CHECK(attr_c_c3->index() == 2);
        CHECK(attr_c_c3->type()->is_character_sequence());
        CHECK(attr_c_c3->type()->size() == 24);

        auto attr_d_b = cast<const DataLayout::Leaf>(inode->at(3).ptr.get());
        REQUIRE(attr_d_b);
        CHECK(attr_d_b->num_tuples() == 1);
        CHECK(attr_d_b->index() == 3);
        CHECK(attr_d_b->type()->is_boolean());
        CHECK(attr_d_b->type()->size() == 1);

        auto attr_e_d = cast<const DataLayout::Leaf>(inode->at(4).ptr.get());
        REQUIRE(attr_e_d);
        CHECK(attr_e_d->num_tuples() == 1);
        CHECK(attr_e_d->index() == 4);
        CHECK(attr_e_d->type()->is_double());
        CHECK(attr_e_d->type()->size() == 64);

        auto attr_f_i1 = cast<const DataLayout::Leaf>(inode->at(5).ptr.get());
        REQUIRE(attr_f_i1);
        CHECK(attr_f_i1->num_tuples() == 1);
        CHECK(attr_f_i1->index() == 5);
        CHECK(attr_f_i1->type()->is_integral());
        CHECK(attr_f_i1->type()->size() == 8);

        auto attr_g_f = cast<const DataLayout::Leaf>(inode->at(6).ptr.get());
        REQUIRE(attr_g_f);
        CHECK(attr_g_f->num_tuples() == 1);
        CHECK(attr_g_f->index() == 6);
        CHECK(attr_g_f->type()->is_float());
        CHECK(attr_g_f->type()->size() == 32);

        auto attr_h_c5 = cast<const DataLayout::Leaf>(inode->at(7).ptr.get());
        REQUIRE(attr_h_c5);
        CHECK(attr_h_c5->num_tuples() == 1);
        CHECK(attr_h_c5->index() == 7);
        CHECK(attr_h_c5->type()->is_character_sequence());
        CHECK(attr_h_c5->type()->size() == 40);

        auto attr_i_b = cast<const DataLayout::Leaf>(inode->at(8).ptr.get());
        REQUIRE(attr_i_b);
        CHECK(attr_i_b->num_tuples() == 1);
        CHECK(attr_i_b->index() == 8);
        CHECK(attr_i_b->type()->is_boolean());
        CHECK(attr_i_b->type()->size() == 1);

        auto attr_j_i2 = cast<const DataLayout::Leaf>(inode->at(9).ptr.get());
        REQUIRE(attr_j_i2);
        CHECK(attr_j_i2->num_tuples() == 1);
        CHECK(attr_j_i2->index() == 9);
        CHECK(attr_j_i2->type()->is_integral());
        CHECK(attr_j_i2->type()->size() == 16);

        auto attr_k_b = cast<const DataLayout::Leaf>(inode->at(10).ptr.get());
        REQUIRE(attr_k_b);
        CHECK(attr_k_b->num_tuples() == 1);
        CHECK(attr_k_b->index() == 10);
        CHECK(attr_k_b->type()->is_boolean());
        CHECK(attr_k_b->type()->size() == 1);

        auto attr_l_i2 = cast<const DataLayout::Leaf>(inode->at(11).ptr.get());
        REQUIRE(attr_l_i2);
        CHECK(attr_l_i2->num_tuples() == 1);
        CHECK(attr_l_i2->index() == 11);
        CHECK(attr_l_i2->type()->is_integral());
        CHECK(attr_l_i2->type()->size() == 16);
    }
}

TEST_CASE("OptimizedRowLayout", "[milestone1]")
{
    Catalog::Clear(); // drop all data
    auto &C = m::Catalog::Get();

    try {
        C.register_data_layout("row_optimized", std::make_unique<MyOptimizedRowLayoutFactory>(), "row layout (optimized)");
    } catch (std::invalid_argument) { }

    auto &DB = C.add_database(C.pool("test_db"));
    auto &table = DB.add_table(C.pool("test"));

    SECTION("simple table")
    {
        /* Fill table with attributes. */
        table.push_back(C.pool("id"), m::Type::Get_Integer(m::Type::TY_Vector, 4));
        table.push_back(C.pool("name"), m::Type::Get_Char(m::Type::TY_Vector, 20));
        table.push_back(C.pool("cakeday"), m::Type::Get_Date(m::Type::TY_Vector));
        table.push_back(C.pool("in_assessment"), m::Type::Get_Boolean(m::Type::TY_Vector));
        table.push_back(C.pool("salary"), m::Type::Get_Double(m::Type::TY_Vector));

        /* Create store and data layout. */
        table.store(C.create_store(table));
        table.layout(C.data_layout("row_optimized"));
        const auto &layout = table.layout();

        /* Root must be an indefinite sequence of rows. */
        CHECK(not layout.is_finite());

        /* Check stride of row. */
        CHECK(layout.stride_in_bits() == 320);

        auto &child_node = layout.child();

        /* Check that the child INode models a single row. */
        CHECK(child_node.num_tuples() == 1);

        /* Check that child node is an INode. */
        auto inode = cast<const DataLayout::INode>(&child_node);
        REQUIRE(inode);

        /* Validate the INode. */
        CHECK(inode->num_children() == 6);
        /* id */
        CHECK(inode->at(0).offset_in_bits == 64);
        CHECK(inode->at(0).stride_in_bits == 0);
        /* name */
        CHECK(inode->at(1).offset_in_bits == 128);
        CHECK(inode->at(1).stride_in_bits == 0);
        /* cakeday */
        CHECK(inode->at(2).offset_in_bits == 96);
        CHECK(inode->at(2).stride_in_bits == 0);
        /* in_assessment */
        CHECK(inode->at(3).offset_in_bits == 288);
        CHECK(inode->at(3).stride_in_bits == 0);
        /* salary */
        CHECK(inode->at(4).offset_in_bits == 0);
        CHECK(inode->at(4).stride_in_bits == 0);
        /* NULL bitmap */
        CHECK(inode->at(5).offset_in_bits == 289);
        CHECK(inode->at(5).stride_in_bits == 0);

        /* Validate `id`. */
        auto attr_id = cast<const DataLayout::Leaf>(inode->at(0).ptr.get());
        REQUIRE(attr_id);
        CHECK(attr_id->num_tuples() == 1);
        CHECK(attr_id->index() == 0);
        CHECK(attr_id->type()->is_integral());
        CHECK(attr_id->type()->size() == 32);

        /* Validate `name`. */
        auto attr_name = cast<const DataLayout::Leaf>(inode->at(1).ptr.get());
        REQUIRE(attr_name);
        CHECK(attr_name->num_tuples() == 1);
        CHECK(attr_name->index() == 1);
        CHECK(attr_name->type()->is_character_sequence());
        CHECK(attr_name->type()->size() == 160);

        /* Validate `cakeday`. */
        auto attr_cakeday = cast<const DataLayout::Leaf>(inode->at(2).ptr.get());
        REQUIRE(attr_cakeday);
        CHECK(attr_cakeday->num_tuples() == 1);
        CHECK(attr_cakeday->index() == 2);
        CHECK(attr_cakeday->type()->is_date());
        CHECK(attr_cakeday->type()->size() == 32);

        /* Validate `in_assessment`. */
        auto attr_in_assessment = cast<const DataLayout::Leaf>(inode->at(3).ptr.get());
        REQUIRE(attr_in_assessment);
        CHECK(attr_in_assessment->num_tuples() == 1);
        CHECK(attr_in_assessment->index() == 3);
        CHECK(attr_in_assessment->type()->is_boolean());
        CHECK(attr_in_assessment->type()->size() == 1);

        /* Validate `salary`. */
        auto attr_salary = cast<const DataLayout::Leaf>(inode->at(4).ptr.get());
        REQUIRE(attr_salary);
        CHECK(attr_salary->num_tuples() == 1);
        CHECK(attr_salary->index() == 4);
        CHECK(attr_salary->type()->is_double());
        CHECK(attr_salary->type()->size() == 64);

        /* Validate the NULL bitmap. */
        auto null_bitmap = cast<const DataLayout::Leaf>(inode->at(5).ptr.get());
        REQUIRE(null_bitmap);
        CHECK(null_bitmap->num_tuples() == 1);
        CHECK(null_bitmap->index() == 5);
        CHECK(null_bitmap->type()->is_bitmap());
        CHECK(null_bitmap->type()->size() == 5);
    }

    SECTION("four booleans")
    {
        /* Fill table with attributes. */
        table.push_back(C.pool("a"), m::Type::Get_Boolean(m::Type::TY_Vector));
        table.push_back(C.pool("b"), m::Type::Get_Boolean(m::Type::TY_Vector));
        table.push_back(C.pool("c"), m::Type::Get_Boolean(m::Type::TY_Vector));
        table.push_back(C.pool("d"), m::Type::Get_Boolean(m::Type::TY_Vector));

        /* Create store and data layout. */
        table.store(C.create_store(table));
        table.layout(C.data_layout("row_optimized"));
        const auto &layout = table.layout();

        /* Root must be an indefinite sequence of rows. */
        CHECK(not layout.is_finite());

        /* Check stride of row. */
        CHECK(layout.stride_in_bits() == 8);

        auto &child_node = layout.child();

        /* Check that the child INode models a single row. */
        CHECK(child_node.num_tuples() == 1);

        /* Check that child node is an INode. */
        auto inode = cast<const DataLayout::INode>(&child_node);
        REQUIRE(inode);

        /* Validate the INode. */
        CHECK(inode->num_children() == 5);
        CHECK(inode->at(0).offset_in_bits == 0);
        CHECK(inode->at(0).stride_in_bits == 0);
        CHECK(inode->at(1).offset_in_bits == 1);
        CHECK(inode->at(1).stride_in_bits == 0);
        CHECK(inode->at(2).offset_in_bits == 2);
        CHECK(inode->at(2).stride_in_bits == 0);
        CHECK(inode->at(3).offset_in_bits == 3);
        CHECK(inode->at(3).stride_in_bits == 0);
        CHECK(inode->at(4).offset_in_bits == 4);
        CHECK(inode->at(4).stride_in_bits == 0);

        auto attr_a = cast<const DataLayout::Leaf>(inode->at(0).ptr.get());
        REQUIRE(attr_a);
        CHECK(attr_a->num_tuples() == 1);
        CHECK(attr_a->index() == 0);
        CHECK(attr_a->type()->is_boolean());
        CHECK(attr_a->type()->size() == 1);

        auto attr_b = cast<const DataLayout::Leaf>(inode->at(1).ptr.get());
        REQUIRE(attr_b);
        CHECK(attr_b->num_tuples() == 1);
        CHECK(attr_b->index() == 1);
        CHECK(attr_b->type()->is_boolean());
        CHECK(attr_b->type()->size() == 1);

        auto attr_c = cast<const DataLayout::Leaf>(inode->at(2).ptr.get());
        REQUIRE(attr_c);
        CHECK(attr_c->num_tuples() == 1);
        CHECK(attr_c->index() == 2);
        CHECK(attr_c->type()->is_boolean());
        CHECK(attr_c->type()->size() == 1);

        auto attr_d = cast<const DataLayout::Leaf>(inode->at(3).ptr.get());
        REQUIRE(attr_d);
        CHECK(attr_d->num_tuples() == 1);
        CHECK(attr_d->index() == 3);
        CHECK(attr_d->type()->is_boolean());
        CHECK(attr_d->type()->size() == 1);

        /* Validate the NULL bitmap. */
        auto null_bitmap = cast<const DataLayout::Leaf>(inode->at(4).ptr.get());
        REQUIRE(null_bitmap);
        CHECK(null_bitmap->num_tuples() == 1);
        CHECK(null_bitmap->index() == 4);
        CHECK(null_bitmap->type()->is_bitmap());
        CHECK(null_bitmap->type()->size() == 4);
    }
}

TEST_CASE("PAXLayout", "[milestone1]")
{
    Catalog::Clear(); // drop all data
    auto &C = m::Catalog::Get();

    try {
        C.register_data_layout("PAX4k", std::make_unique<MyPAX4kLayoutFactory>(), "PAX layout with 4KiB blocks");
    } catch (std::invalid_argument) { }

    auto &DB = C.add_database(C.pool("test_db"));
    auto &table = DB.add_table(C.pool("test"));

    SECTION("INT(4)")
    {
        /* Fill table with attributes. */
        table.push_back(C.pool("a"), m::Type::Get_Integer(m::Type::TY_Vector, 4));

        /* Create store and data layout. */
        table.store(C.create_store(table));
        table.layout(C.data_layout("PAX4k"));
        const auto &layout = table.layout();

        /* Root must be an indefinite sequence of rows. */
        CHECK(not layout.is_finite());

        /* Check stride of row. */
        CHECK(layout.stride_in_bits() == 4096 * 8);

        auto &child_node = layout.child();

        /* Check that the child INode models a single row. */
        CHECK(child_node.num_tuples() == 992);

        /* Check that child node is an INode. */
        auto inode = cast<const DataLayout::INode>(&child_node);
        REQUIRE(inode);

        /* Validate the INode. */
        CHECK(inode->num_children() == 2);
        CHECK(inode->at(0).offset_in_bits == 0);
        CHECK(inode->at(0).stride_in_bits == 32);
        CHECK(inode->at(1).offset_in_bits == 31744);
        CHECK(inode->at(1).stride_in_bits == 1);

        /* Validate the data leaf. */
        auto attr_leaf = cast<const DataLayout::Leaf>(inode->at(0).ptr.get());
        REQUIRE(attr_leaf);
        CHECK(attr_leaf->num_tuples() == 1);
        CHECK(attr_leaf->index() == 0);
        CHECK(attr_leaf->type()->is_integral());
        CHECK(attr_leaf->type()->size() == 32);

        /* Validate the NULL bitmap. */
        auto null_bitmap = cast<const DataLayout::Leaf>(inode->at(1).ptr.get());
        REQUIRE(null_bitmap);
        CHECK(null_bitmap->num_tuples() == 1);
        CHECK(null_bitmap->index() == 1);
        CHECK(null_bitmap->type()->is_bitmap());
        CHECK(null_bitmap->type()->size() == 1);
    }

    SECTION("DOUBLE")
    {
        /* Fill table with attributes. */
        table.push_back(C.pool("a"), m::Type::Get_Double(m::Type::TY_Vector));

        /* Create store and data layout. */
        table.store(C.create_store(table));
        table.layout(C.data_layout("PAX4k"));
        const auto &layout = table.layout();

        /* Root must be an indefinite sequence of rows. */
        CHECK(not layout.is_finite());

        /* Check stride of row. */
        CHECK(layout.stride_in_bits() == 4096 * 8);

        auto &child_node = layout.child();

        /* Check that the child INode models a single row. */
        CHECK(child_node.num_tuples() == 504);

        /* Check that child node is an INode. */
        auto inode = cast<const DataLayout::INode>(&child_node);
        REQUIRE(inode);

        /* Validate the INode. */
        CHECK(inode->num_children() == 2);
        CHECK(inode->at(0).offset_in_bits == 0);
        CHECK(inode->at(0).stride_in_bits == 64);
        CHECK(inode->at(1).offset_in_bits == 32256);
        CHECK(inode->at(1).stride_in_bits == 1);

        /* Validate the data leaf. */
        auto attr_leaf = cast<const DataLayout::Leaf>(inode->at(0).ptr.get());
        REQUIRE(attr_leaf);
        CHECK(attr_leaf->num_tuples() == 1);
        CHECK(attr_leaf->index() == 0);
        CHECK(attr_leaf->type()->is_double());
        CHECK(attr_leaf->type()->size() == 64);

        /* Validate the NULL bitmap. */
        auto null_bitmap = cast<const DataLayout::Leaf>(inode->at(1).ptr.get());
        REQUIRE(null_bitmap);
        CHECK(null_bitmap->num_tuples() == 1);
        CHECK(null_bitmap->index() == 1);
        CHECK(null_bitmap->type()->is_bitmap());
        CHECK(null_bitmap->type()->size() == 1);
    }

    SECTION("INT(2)")
    {
        /* Fill table with attributes. */
        table.push_back(C.pool("a"), m::Type::Get_Integer(m::Type::TY_Vector, 2));

        /* Create store and data layout. */
        table.store(C.create_store(table));
        table.layout(C.data_layout("PAX4k"));
        const auto &layout = table.layout();

        /* Root must be an indefinite sequence of rows. */
        CHECK(not layout.is_finite());

        /* Check stride of row. */
        CHECK(layout.stride_in_bits() == 4096 * 8);

        auto &child_node = layout.child();

        /* Check that the child INode models a single row. */
        CHECK(child_node.num_tuples() == 1927);

        /* Check that child node is an INode. */
        auto inode = cast<const DataLayout::INode>(&child_node);
        REQUIRE(inode);

        /* Validate the INode. */
        CHECK(inode->num_children() == 2);
        CHECK(inode->at(0).offset_in_bits == 0);
        CHECK(inode->at(0).stride_in_bits == 16);
        CHECK(inode->at(1).offset_in_bits == 30832);
        CHECK(inode->at(1).stride_in_bits == 1);

        /* Validate the data leaf. */
        auto attr_leaf = cast<const DataLayout::Leaf>(inode->at(0).ptr.get());
        REQUIRE(attr_leaf);
        CHECK(attr_leaf->num_tuples() == 1);
        CHECK(attr_leaf->index() == 0);
        CHECK(attr_leaf->type()->is_integral());
        CHECK(attr_leaf->type()->size() == 16);

        /* Validate the NULL bitmap. */
        auto null_bitmap = cast<const DataLayout::Leaf>(inode->at(1).ptr.get());
        REQUIRE(null_bitmap);
        CHECK(null_bitmap->num_tuples() == 1);
        CHECK(null_bitmap->index() == 1);
        CHECK(null_bitmap->type()->is_bitmap());
        CHECK(null_bitmap->type()->size() == 1);
    }

    SECTION("CHAR(3)")
    {
        /* Fill table with attributes. */
        table.push_back(C.pool("a"), m::Type::Get_Char(m::Type::TY_Vector, 3));

        /* Create store and data layout. */
        table.store(C.create_store(table));
        table.layout(C.data_layout("PAX4k"));
        const auto &layout = table.layout();

        /* Root must be an indefinite sequence of rows. */
        CHECK(not layout.is_finite());

        /* Check stride of row. */
        CHECK(layout.stride_in_bits() == 4096 * 8);

        auto &child_node = layout.child();

        /* Check that the child INode models a single row. */
        CHECK(child_node.num_tuples() == 1310);

        /* Check that child node is an INode. */
        auto inode = cast<const DataLayout::INode>(&child_node);
        REQUIRE(inode);

        /* Validate the INode. */
        CHECK(inode->num_children() == 2);
        CHECK(inode->at(0).offset_in_bits == 0);
        CHECK(inode->at(0).stride_in_bits == 24);
        CHECK(inode->at(1).offset_in_bits == 31440);
        CHECK(inode->at(1).stride_in_bits == 1);

        /* Validate the data leaf. */
        auto attr_leaf = cast<const DataLayout::Leaf>(inode->at(0).ptr.get());
        REQUIRE(attr_leaf);
        CHECK(attr_leaf->num_tuples() == 1);
        CHECK(attr_leaf->index() == 0);
        CHECK(attr_leaf->type()->is_character_sequence());
        CHECK(attr_leaf->type()->size() == 24);

        /* Validate the NULL bitmap. */
        auto null_bitmap = cast<const DataLayout::Leaf>(inode->at(1).ptr.get());
        REQUIRE(null_bitmap);
        CHECK(null_bitmap->num_tuples() == 1);
        CHECK(null_bitmap->index() == 1);
        CHECK(null_bitmap->type()->is_bitmap());
        CHECK(null_bitmap->type()->size() == 1);
    }

    SECTION("BOOL")
    {
        /* Fill table with attributes. */
        table.push_back(C.pool("a"), m::Type::Get_Boolean(m::Type::TY_Vector));

        /* Create store and data layout. */
        table.store(C.create_store(table));
        table.layout(C.data_layout("PAX4k"));
        const auto &layout = table.layout();

        /* Root must be an indefinite sequence of rows. */
        CHECK(not layout.is_finite());

        /* Check stride of row. */
        CHECK(layout.stride_in_bits() == 4096 * 8);

        auto &child_node = layout.child();

        /* Check that the child INode models a single row. */
        CHECK(child_node.num_tuples() >= 16377);

        /* Check that child node is an INode. */
        auto inode = cast<const DataLayout::INode>(&child_node);
        REQUIRE(inode);

        /* Validate the INode. */
        CHECK(inode->num_children() == 2);
        CHECK(inode->at(0).offset_in_bits == 0);
        CHECK(inode->at(0).stride_in_bits == 1);
        CHECK(inode->at(1).offset_in_bits >= 16377);
        CHECK(inode->at(1).stride_in_bits == 1);

        /* Validate the data leaf. */
        auto attr_leaf = cast<const DataLayout::Leaf>(inode->at(0).ptr.get());
        REQUIRE(attr_leaf);
        CHECK(attr_leaf->num_tuples() == 1);
        CHECK(attr_leaf->index() == 0);
        CHECK(attr_leaf->type()->is_boolean());
        CHECK(attr_leaf->type()->size() == 1);

        /* Validate the NULL bitmap. */
        auto null_bitmap = cast<const DataLayout::Leaf>(inode->at(1).ptr.get());
        REQUIRE(null_bitmap);
        CHECK(null_bitmap->num_tuples() == 1);
        CHECK(null_bitmap->index() == 1);
        CHECK(null_bitmap->type()->is_bitmap());
        CHECK(null_bitmap->type()->size() == 1);
    }

    SECTION("simple table")
    {
        /* Fill table with attributes. */
        table.push_back(C.pool("id"), m::Type::Get_Integer(m::Type::TY_Vector, 4));
        table.push_back(C.pool("name"), m::Type::Get_Char(m::Type::TY_Vector, 20));
        table.push_back(C.pool("cakeday"), m::Type::Get_Date(m::Type::TY_Vector));
        table.push_back(C.pool("in_assessment"), m::Type::Get_Boolean(m::Type::TY_Vector));
        table.push_back(C.pool("salary"), m::Type::Get_Double(m::Type::TY_Vector));

        /* Create store and data layout. */
        table.store(C.create_store(table));
        table.layout(C.data_layout("PAX4k"));
        const auto &layout = table.layout();

        /* Root must be an indefinite sequence of rows. */
        CHECK(not layout.is_finite());

        /* Check stride of row. */
        CHECK(layout.stride_in_bits() == 4096 * 8);

        auto &child_node = layout.child();

        /* Check that the child INode models a single row. */
        CHECK((child_node.num_tuples() == 111 or child_node.num_tuples() == 112));

        /* Check that child node is an INode. */
        auto inode = cast<const DataLayout::INode>(&child_node);
        REQUIRE(inode);

        /* Validate the INode. */
        CHECK(inode->num_children() == 6);
        CHECK((inode->at(0).offset_in_bits == 7104 or inode->at(0).offset_in_bits == 7168));
        CHECK(inode->at(0).stride_in_bits == 32);
        CHECK((inode->at(1).offset_in_bits == 14208 or inode->at(1).offset_in_bits == 14336));
        CHECK(inode->at(1).stride_in_bits == 160);
        CHECK((inode->at(2).offset_in_bits == 10656 or inode->at(2).offset_in_bits == 10752));
        CHECK(inode->at(2).stride_in_bits == 32);
        CHECK((inode->at(3).offset_in_bits == 31968 or inode->at(3).offset_in_bits == 32256));
        CHECK(inode->at(3).stride_in_bits == 1);
        CHECK(inode->at(4).offset_in_bits == 0);
        CHECK(inode->at(4).stride_in_bits == 64);

        /* Validate `id`. */
        auto attr_id = cast<const DataLayout::Leaf>(inode->at(0).ptr.get());
        REQUIRE(attr_id);
        CHECK(attr_id->num_tuples() == 1);
        CHECK(attr_id->index() == 0);
        CHECK(attr_id->type()->is_integral());
        CHECK(attr_id->type()->size() == 32);

        /* Validate `name`. */
        auto attr_name = cast<const DataLayout::Leaf>(inode->at(1).ptr.get());
        REQUIRE(attr_name);
        CHECK(attr_name->num_tuples() == 1);
        CHECK(attr_name->index() == 1);
        CHECK(attr_name->type()->is_character_sequence());
        CHECK(attr_name->type()->size() == 160);

        /* Validate `cakeday`. */
        auto attr_cakeday = cast<const DataLayout::Leaf>(inode->at(2).ptr.get());
        REQUIRE(attr_cakeday);
        CHECK(attr_cakeday->num_tuples() == 1);
        CHECK(attr_cakeday->index() == 2);
        CHECK(attr_cakeday->type()->is_date());
        CHECK(attr_cakeday->type()->size() == 32);

        /* Validate `in_assessment`. */
        auto attr_in_assessment = cast<const DataLayout::Leaf>(inode->at(3).ptr.get());
        REQUIRE(attr_in_assessment);
        CHECK(attr_in_assessment->num_tuples() == 1);
        CHECK(attr_in_assessment->index() == 3);
        CHECK(attr_in_assessment->type()->is_boolean());
        CHECK(attr_in_assessment->type()->size() == 1);

        /* Validate `salary`. */
        auto attr_salary = cast<const DataLayout::Leaf>(inode->at(4).ptr.get());
        REQUIRE(attr_salary);
        CHECK(attr_salary->num_tuples() == 1);
        CHECK(attr_salary->index() == 4);
        CHECK(attr_salary->type()->is_double());
        CHECK(attr_salary->type()->size() == 64);

        /* Validate the NULL bitmap. */
        auto null_bitmap = cast<const DataLayout::Leaf>(inode->at(5).ptr.get());
        REQUIRE(null_bitmap);
        CHECK(null_bitmap->num_tuples() == 1);
        CHECK(null_bitmap->index() == 5);
        CHECK(null_bitmap->type()->is_bitmap());
        CHECK(null_bitmap->type()->size() == 5);
    }
}
