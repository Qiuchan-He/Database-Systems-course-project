#include "data_layouts.hpp"
#include <numeric>
#include <cmath>

using namespace m;
using namespace m::storage;

DataLayout MyNaiveRowLayoutFactory::make(std::vector<const Type *> types, std::size_t num_tuples) const
{
    const Bitmap *bitmap = Type::Get_Bitmap(Type::TY_Vector, types.size());
    types.push_back(bitmap);

    std::vector<std::pair<size_t, const Type *>> types_mapped;
    for (size_t index = 0; index < types.size(); index++)
    {
        types_mapped.push_back(std::pair{index, types[index]});
    }

    std::unordered_map<size_t, size_t> offset_map;
    uint64_t inode_alignment = 8;
    uint64_t offset = 0;
    uint64_t padding = 0;
    uint64_t alignment = 0;
    uint64_t inode_stride = 0;
    for (size_t i = 0; i < types_mapped.size(); i++)
    {
        size_t key = types_mapped[i].first;
        const Type *type = types_mapped[i].second;

        alignment = type->alignment();
        padding = (alignment - (offset % alignment)) % alignment;
        offset += padding;

        offset_map.insert({key, offset});

        offset += type->size();
        inode_alignment = std::max(inode_alignment, type->alignment());
    }

    inode_stride = std::ceil(offset / (float)inode_alignment) * inode_alignment;

    DataLayout layout;
    auto &row = layout.add_inode(1, inode_stride);

    uint64_t index = 0;
    for (size_t i = 0; i < types_mapped.size(); i++)
    {
        size_t key = types_mapped[i].first;
        const Type *type = types_mapped[i].second;

        row.add_leaf(
            type,
            index,
            offset_map.at(key),
            0);

        index++;
    }

    return layout;
}

bool sort_alignment_descending(const std::pair<int, const Type *> t1, const std::pair<int, const Type *> t2)
{
    return t1.second->alignment() > t2.second->alignment();
}

bool sort_index_ascending(const std::pair<int, const Type *> t1, const std::pair<int, const Type *> t2)
{
    return t1.first < t2.first;
}

DataLayout MyOptimizedRowLayoutFactory::make(std::vector<const Type *> types, std::size_t num_tuples) const
{
    const Bitmap *bitmap = Type::Get_Bitmap(Type::TY_Vector, types.size());
    types.push_back(bitmap);

    std::vector<std::pair<size_t, const Type *>> types_mapped;
    for (size_t index = 0; index < types.size(); index++)
    {
        types_mapped.push_back(std::pair{index, types[index]});
    }

    std::sort(types_mapped.begin(), types_mapped.end(), sort_alignment_descending);

    std::unordered_map<size_t, size_t> offset_map;
    uint64_t inode_alignment = 8;
    uint64_t offset = 0;
    uint64_t padding = 0;
    uint64_t alignment = 0;
    uint64_t inode_stride = 0;
    for (size_t i = 0; i < types_mapped.size(); i++)
    {
        size_t key = types_mapped[i].first;
        const Type *type = types_mapped[i].second;

        alignment = type->alignment();
        padding = (alignment - (offset % alignment)) % alignment;
        offset += padding;

        offset_map.insert({key, offset});

        offset += type->size();
        inode_alignment = std::max(inode_alignment, type->alignment());
    }

    inode_stride = std::ceil(offset / (float)inode_alignment) * inode_alignment;

    std::sort(types_mapped.begin(), types_mapped.end(), sort_index_ascending);

    DataLayout layout;
    auto &row = layout.add_inode(1, inode_stride);

    uint64_t index = 0;
    for (size_t i = 0; i < types_mapped.size(); i++)
    {
        size_t key = types_mapped[i].first;
        const Type *type = types_mapped[i].second;

        row.add_leaf(
            type,
            index,
            offset_map.at(key),
            0);

        index++;
    }

    return layout;
}

DataLayout MyPAX4kLayoutFactory::make(std::vector<const Type *> types, std::size_t num_tuples) const
{
    const Bitmap *bitmap = Type::Get_Bitmap(Type::TY_Vector, types.size());
    types.push_back(bitmap);

    uint64_t total_size = 0;
    uint64_t inode_alignment = 8;
    for (const Type *type : types)
    {
        total_size += type->size();
        inode_alignment = std::max(inode_alignment, type->alignment());
    }

    uint64_t inode_num_tuples = std::floor((4096 * 8) / total_size);

    std::vector<std::pair<size_t, const Type *>> types_mapped;
    for (size_t index = 0; index < types.size(); index++)
    {
        types_mapped.push_back(std::pair{index, types[index]});
    }

    std::sort(types_mapped.begin(), types_mapped.end(), sort_alignment_descending);

    std::unordered_map<size_t, size_t> offset_map;
    uint64_t offset = 0;
    uint64_t padding = 0;
    uint64_t alignment = 0;
    for (size_t i = 0; i < types_mapped.size(); i++)
    {
        size_t key = types_mapped[i].first;
        const Type *type = types_mapped[i].second;

        alignment = type->alignment();
        padding = (alignment - (offset % alignment)) % alignment;
        offset += padding;

        offset_map.insert({key, offset});

        offset += type->size() * inode_num_tuples;
    }

    std::sort(types_mapped.begin(), types_mapped.end(), sort_index_ascending);

    DataLayout layout;
    auto &row = layout.add_inode(inode_num_tuples, 4096 * 8);

    uint64_t index = 0;
    for (size_t i = 0; i < types_mapped.size(); i++)
    {
        size_t key = types_mapped[i].first;
        const Type *type = types_mapped[i].second;

        row.add_leaf(
            type,
            index,
            offset_map.at(key),
            type->size());

        index++;
    }

    return layout;
}
