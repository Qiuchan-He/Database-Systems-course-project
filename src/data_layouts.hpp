#pragma once


#include <mutable/mutable.hpp>
#include <mutable/storage/DataLayoutFactory.hpp>


struct MyNaiveRowLayoutFactory : m::storage::DataLayoutFactory
{
    m::storage::DataLayout make(std::vector<const m::Type*> types, std::size_t num_tuples = 0) const override;
};

struct MyOptimizedRowLayoutFactory : m::storage::DataLayoutFactory
{
    m::storage::DataLayout make(std::vector<const m::Type*> types, std::size_t num_tuples = 0) const override;
};

struct MyPAX4kLayoutFactory : m::storage::DataLayoutFactory
{
    m::storage::DataLayout make(std::vector<const m::Type*> types, std::size_t num_tuples = 0) const override;
};

