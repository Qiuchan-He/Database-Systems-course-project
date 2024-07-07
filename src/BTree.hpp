#pragma once

#include "mutable/util/macro.hpp"
#include <algorithm>
#include <array>
#include <vector>
#include <bit>
#include <cassert>
#include <concepts>
#include <cstdint>
#include <cmath>
#include <queue>
#include <deque>
#include <utility>

/** Require that \tparam T is an *orderable* type, i.e. that two instances of \tparam T can be compared less than and
 * equals. */
template <typename T>
concept orderable = requires(T a, T b) {
                        {
                            a < b
                            } -> std::same_as<bool>;
                        {
                            a == b
                            } -> std::same_as<bool>;
                    };

/** Require that \tparam T is sortable, i.e. that it is `orderable`, movable, and swappable. */
template <typename T>
concept sortable = std::movable<T> and std::swappable<T> and orderable<T>;

/** A replacement for `std::pair`, that does not leak/enforce a particular data layout. */
template <typename First, typename Second>
struct ref_pair
{
private:
    // std::reference_wrapper<First> first_;
    // std::reference_wrapper<Second> second_;
    std::pair<First, Second> pair;

public:
    ref_pair() {}
    ref_pair(const First &first, const Second &second) : pair{first, second} {}
    ~ref_pair() {}

    First &first() { return pair.first; }
    Second &second() { return pair.second; }
};

/** Implements a B+-tree of \tparam Key - \tparam Value pairs.  The exact size of a tree node is given as \tparam
 * NodeSizeInBytes and the exact node alignment is given as \tparam NodeAlignmentInBytes.  The implementation must
 * guarantee that nodes are properly allocated to satisfy the alignment. */
template <
    typename Key,
    std::movable Value,
    std::size_t NodeSizeInBytes,
    std::size_t NodeAlignmentInBytes = NodeSizeInBytes>
    requires sortable<Key> and std::copyable<Key>
struct BTree
{
    using key_type = Key;
    using mapped_type = Value;
    using size_type = std::size_t;

    ///> the size of BTree nodes (both `INode` and `Leaf`)
    static constexpr size_type NODE_SIZE_IN_BYTES = NodeSizeInBytes;
    ///> the aignment of BTree nodes (both `INode` and `Leaf`)
    static constexpr size_type NODE_ALIGNMENT_IN_BYTES = NodeAlignmentInBytes;

private:
    /** Computes the number of key-value pairs per `Leaf`, considering the specified `NodeSizeInBytes`. */
    static constexpr size_type compute_num_keys_per_leaf()
    {
        // return 2;
        return (NodeSizeInBytes - (sizeof(void *) + sizeof(int) + sizeof(void *))) / sizeof(ref_pair<key_type, mapped_type>);
    };

    /** Computes the number of keys per `INode`, considering the specified `NodeSizeInBytes`. */
    static constexpr size_type compute_num_keys_per_inode()
    {
        // return 2;
        return (NodeSizeInBytes - sizeof(void *)) / sizeof(ref_pair<key_type, Node *>);
    };

public:
    ///> the number of key-value pairs per `Leaf`
    static constexpr size_type NUM_KEYS_PER_LEAF = compute_num_keys_per_leaf();
    ///> the number of keys per `INode`
    static constexpr size_type NUM_KEYS_PER_INODE = compute_num_keys_per_inode();

    struct Node
    {
        virtual void append_child(ref_pair<key_type, mapped_type> child) {}
        virtual void append_child(key_type key, Node *child) {}


        virtual ref_pair<key_type, mapped_type> find_smallest_pair() {}
        virtual Node *find(key_type key) {}
        virtual void print_contents() {}

        virtual bool is_full() {}
        virtual void set_next_leaf(Node *next_leaf) {}
        virtual Node *get_next_leaf() {}

        virtual ref_pair<const key_type, mapped_type> *get_start_iterator() {}
        virtual ref_pair<const key_type, mapped_type> *get_last_iterator() {}

        virtual Node *get_first_leaf() {}
        virtual Node *get_last_leaf() {}

        virtual int calculate_height() {}
        virtual int get_size() {}
        virtual ref_pair<key_type, mapped_type> *get_child(int index) {}
    };

    /** This class implements leaves of the B+-tree. */
    struct alignas(NODE_ALIGNMENT_IN_BYTES) Leaf : public Node
    {
        // TODO: make fields private
        ref_pair<key_type, mapped_type> children[NUM_KEYS_PER_LEAF];
        int size = 0;
        Node *next_leaf = nullptr;

        ~Leaf()
        {
            delete[] children;
            if (next_leaf != nullptr)
                delete next_leaf;
        }

        int get_size() override { return size; }

        ref_pair<key_type, mapped_type> *get_child(int index) override
        {
            if (index >= size)
                return children + index;
            return children + index;
        }

        void append_child(ref_pair<key_type, mapped_type> child) override
        {
            children[size] = child;
            ++size;
        }

        ref_pair<key_type, mapped_type> find_smallest_pair() override
        {
            return children[size - 1];
        }

        void print_contents() override
        {
            std::cout << "LEAF: ";
            for (auto child : children)
                std::cout << child.first() << ", ";
            std::cout << std::endl;
        }

        bool is_full() override
        {
            return size >= NUM_KEYS_PER_LEAF;
        }

        void set_next_leaf(Node *next_leaf) override
        {
            this->next_leaf = next_leaf;
        }

        Node *get_next_leaf() override
        {
            return next_leaf;
        }

        ref_pair<const key_type, mapped_type> *get_start_iterator()
        {
            return reinterpret_cast<ref_pair<const key_type, mapped_type> *>(children);
        }

        ref_pair<const key_type, mapped_type> *get_last_iterator()
        {
            return reinterpret_cast<ref_pair<const key_type, mapped_type> *>(children + size);
        }

        Node *get_first_leaf() override
        {
            return this;
        }

        Node *get_last_leaf() override
        {
            return this;
        }

        int calculate_height() override
        {
            return 0;
        }

        Node *find(key_type key) override
        {
            return this;
        }
    };
    static_assert(sizeof(Leaf) <= NODE_SIZE_IN_BYTES, "Leaf exceeds its size limit");

    /** This class implements inner nodes of the B+-tree. */
    struct alignas(NODE_ALIGNMENT_IN_BYTES) INode : public Node
    {
        // TODO: make fields private
        ref_pair<key_type, Node *> children[NUM_KEYS_PER_INODE];
        int size = 0;

        ~INode()
        {
            delete[] children;
        }

        int get_size() override { return size; }

        void append_child(key_type key, Node *child) override
        {
            // ref_pair<key_type, mapped_type> smallest_pair = child->find_smallest_pair();
            // key_type key = smallest_pair.first();

            ref_pair<key_type, Node *> new_pair = ref_pair(key, child);
            children[size] = new_pair;
            ++size;
        }

        ref_pair<key_type, mapped_type> find_smallest_pair() override
        {
            Node *node = children[size - 1].second();
            return node->find_smallest_pair();
        }

        void print_contents() override
        {
            std::cout << "INODE: ";
            for (auto child : children)
                std::cout << child.first() << ", ";
            std::cout << std::endl;
        }

        bool is_full() override
        {
            return size >= NUM_KEYS_PER_INODE;
        }

        Node *get_first_leaf() override
        {
            Node *node = children[0].second();
            return node->get_first_leaf();
        }

        Node *get_last_leaf() override
        {
            Node *node = children[size - 1].second();
            return node->get_last_leaf();
        }

        int calculate_height() override
        {
            Node *node = children[size - 1].second();
            return 1 + node->calculate_height();
        }

        Node *find(key_type key) override
        {
            int left = 0;
            int right = size - 1;
            int mid;
            Node *child = nullptr;

            while (left <= right)
            {
                mid = left + (right - left) / 2;
                if (children[mid].first() >= key)
                {
                    child = children[mid].second();
                    right = mid - 1;
                }
                else
                {
                    left = mid + 1;
                }
            }

            if (child == nullptr)
                child = children[size-1].second();

            return child->find(key);
        }
    };

    static_assert(sizeof(INode) <= NODE_SIZE_IN_BYTES, "INode exceeds its size limit");

private:
    template <bool IsConst>
    struct the_iterator
    {
        friend struct BTree;

        static constexpr bool is_const = IsConst;
        using value_type = std::conditional_t<is_const, const mapped_type, mapped_type>;

    private:
        using leaf_type = std::conditional_t<is_const, const Leaf, Leaf>;

        Node *current_leaf = nullptr;
        ref_pair<const key_type, mapped_type> *current_element = nullptr;
        ref_pair<const key_type, mapped_type> *last_element = nullptr;

    public:
        the_iterator() {}

        the_iterator(
            Node *_current_leaf,
            ref_pair<const key_type, mapped_type> *_current_element,
            ref_pair<const key_type, mapped_type> *_last_element) : current_leaf{_current_leaf}, current_element(_current_element), last_element{_last_element}
        {
        }

        the_iterator(the_iterator<false> other)
            requires is_const
        {
            current_leaf = other.current_leaf;
            current_element = other.current_element;
            last_element = other.last_element;
        }

        bool operator==(the_iterator other) const
        {
            return current_leaf == other.current_leaf &&
                   current_element == other.current_element &&
                   last_element == other.last_element;
        }
        bool operator!=(the_iterator other) const { return not operator==(other); }

        the_iterator &operator++()
        {
            current_element++;
            if (current_element == last_element)
            {

                if (current_leaf->get_next_leaf() == nullptr)
                    return *this;

                current_leaf = current_leaf->get_next_leaf();
                current_element = current_leaf->get_start_iterator();
                last_element = current_leaf->get_last_iterator();
            }
            return *this;
        }

        the_iterator operator++(int)
        {
            the_iterator copy(this);
            operator++();
            return copy;
        }

        ref_pair<const key_type, mapped_type> operator*() const
        {
            return *current_element;
        }
    };

    template <bool IsConst>
    struct the_range
    {
        static constexpr bool is_const = IsConst;
        using iter_t = the_iterator<is_const>;

    private:
        iter_t begin_, end_;

    public:
        the_range(iter_t begin, iter_t end) : begin_(begin), end_(end) {}

        bool empty() const { return begin() == end(); }

        iter_t begin() const { return begin_; }
        iter_t end() const { return end_; }
    };

public:
    using iterator = the_iterator<false>;
    using const_iterator = the_iterator<true>;

    using range = the_range<false>;
    using const_range = the_range<true>;

private:
    /* TODO 1.4.1 define fields */
    Node *root;
    int64_t size_ = 0;
    int64_t height_ = 0;

public:
    BTree(Node *_root, int64_t _size, int64_t _height) : root{_root}, size_{_size}, height_{_height}
    {
    }

    ~BTree() {}

    static Node *bulkload_helper(std::vector<Node *> &children, int64_t &height)
    {
        // for (auto child : children)
        //     child->print_contents();

        if (children.size() == 1)
        {
            // std::cout << "bulkloading: done..." << std::endl;
            return children.at(0);
        }

        std::vector<Node *> parents;

        auto child_it = children.begin();
        while (child_it != children.end())
        {
            Node *parent = new INode();
            while (child_it != children.end() && !parent->is_full())
            {
                parent->append_child(*child_it);
                child_it++;
            }
            parents.push_back(parent);
        }

        height++;
        return bulkload_helper(parents, height);
    }

    /** Bulkloads the data in the range from `begin` (inclusive) to `end` (exclusive) into a fresh `BTree` and returns
     * it. */
    template <typename It>
    static BTree Bulkload(It begin, It end)
        requires requires(It it) {
                     key_type(std::move(it->first));
                     mapped_type(std::move(it->second));
                 }
    {
        if (begin == end)
            return BTree(nullptr, 0, 0);

        std::deque<std::pair<key_type, Node *>> children;
        int64_t size = 0;

        auto pair_it = begin;
        while (pair_it != end)
        {
            Node *child = new Leaf();
            key_type maxi = std::numeric_limits<key_type>::min();
            while (pair_it != end && !child->is_full())
            {
                ref_pair<key_type, mapped_type> pair = ref_pair(pair_it->first, pair_it->second);
                child->append_child(pair);
                size++;
                maxi = std::max(maxi, pair_it->first);
                pair_it++;
            }

            if (children.size())
                (children.back().second)->set_next_leaf(child);
            
            // child->print_contents();

            children.push_back({maxi, child});
        }

        // for (int i = 1; i < size; i++)
        //     children[i - 1]->set_next_leaf(children[i]);

        while (children.size() > 1)
        {
            std::deque<std::pair<key_type, Node *>> parents;
            while (children.size())
            {
                Node *parent = new INode();
                key_type maxi = std::numeric_limits<key_type>::min();
                while (children.size() && !parent->is_full())
                {
                    std::pair<key_type, Node *> child = children.front();
                    children.pop_front();
                    parent->append_child(child.first, child.second);
                    maxi = std::max(maxi, child.first);
                }
                parents.push_back({maxi, parent});
            }
            children = parents;
        }

        // Node *root = bulkload_helper(children, height);
        Node *root = children.front().second;
        int64_t height = BTree::calculate_height(root);
        return BTree(root, size, height);
    }

    static int calculate_height(Node *root)
    {
        if (root == nullptr)
            return 0;
        return root->calculate_height();
    }

    ///> returns the size of the tree, i.e. the number of key-value pairs
    size_type size() const
    { /* TODO 1.4.2 */
        return size_;
    }
    ///> returns the number if inner/non-leaf levels, a.k.a. the height
    size_type height() const
    { /* TODO 1.4.2 */
        return height_;
    }

    /** Returns an `iterator` to the smallest key-value pair of the tree, if any, and `end()` otherwise. */
    iterator begin()
    { /* TODO 1.4.3 */
        if (root == nullptr)
            return iterator();
        Node *first_leaf = root->get_first_leaf();
        return iterator(
            first_leaf,
            first_leaf->get_start_iterator(),
            first_leaf->get_last_iterator());
    }
    /** Returns the past-the-end `iterator`. */
    iterator end()
    { /* TODO 1.4.3 */
        if (root == nullptr)
            return iterator();
        Node *last_leaf = root->get_last_leaf();
        return iterator(
            last_leaf,
            last_leaf->get_last_iterator(),
            last_leaf->get_last_iterator());
    }
    /** Returns an `const_iterator` to the smallest key-value pair of the tree, if any, and `end()` otherwise. */
    const_iterator begin() const
    { /* TODO 1.4.3 */
        if (root == nullptr)
            return const_iterator();
        Node *first_leaf = root->get_first_leaf();
        return const_iterator(
            first_leaf,
            first_leaf->get_start_iterator(),
            first_leaf->get_last_iterator());
    }
    /** Returns the past-the-end `iterator`. */
    const_iterator end() const
    { /* TODO 1.4.3 */
        if (root == nullptr)
            return const_iterator();
        Node *last_leaf = root->get_last_leaf();
        return const_iterator(
            last_leaf,
            last_leaf->get_last_iterator(),
            last_leaf->get_last_iterator());
    }
    /** Returns an `const_iterator` to the smallest key-value pair of the tree, if any, and `end()` otherwise. */
    const_iterator cbegin() const { return begin(); }
    /** Returns the past-the-end `iterator`. */
    const_iterator cend() const { return end(); }

    /** Returns a `const_iterator` to the first element with the given \p key, if any, and `end()` otherwise. */
    const_iterator find(const key_type &key) const
    {
        // std::cout << "find in const_iterator" << std::endl;
        if (root == nullptr)
            return cend();

        Node *leaf = root->find(key);
        // if (leaf == nullptr)
        //     return cend();
        // leaf->print_contents();

        int left = 0, right = leaf->get_size() - 1;
        while (left <= right)
        {
            int mid = left + (right - left) / 2;
            key_type leaf_key = leaf->get_child(mid)->first();

            if (leaf_key == key)
                return const_iterator(
                    leaf,
                    leaf->get_start_iterator() + mid,
                    leaf->get_last_iterator());
            else if (leaf_key < key)
                left = mid + 1;
            else
                right = mid - 1;
        }
        return cend();
    }
    /** Returns an `iterator` to the first element with the given \p key, if any, and `end()` otherwise. */
    iterator find(const key_type &key)
    {
        // std::cout << "find in iterator: " << key << std::endl;
        if (root == nullptr)
            return end();

        Node *leaf = root->find(key);
        // if (leaf == nullptr){
        //     //std::cout << "cannot find leaf: " << key <<std::endl;
        //     return end();
        // }

        // leaf->print_contents();

        int left = 0, right = leaf->get_size() - 1;
        while (left <= right)
        {
            int mid = left + (right - left) / 2;
            key_type leaf_key = leaf->get_child(mid)->first();

            if (leaf_key == key)
                return iterator(
                    leaf,
                    leaf->get_start_iterator() + mid,
                    leaf->get_last_iterator());
            else if (leaf_key < key)
                left = mid + 1;
            else
                right = mid - 1;
        }
        // std::cout << "cannot find key: " << key <<std::endl;
        return end();
    }

    /** Returns a `const_range` of all elements with key in the interval `[lo, hi)`, i.e. `lo` including and `hi`
     * excluding. */
    const_range find_range(const key_type &lo, const key_type &hi) const
    {
        const_iterator end = BTree::end();

        if (root == nullptr)
            return const_range(end, end);

        Node *left_leaf = root->find(lo);
        Node *right_leaf = root->find(hi);

        const_iterator left = const_iterator(
            left_leaf,
            left_leaf->get_start_iterator(),
            left_leaf->get_last_iterator());

        while (left != end && left.first() < lo)
            ++left;

        const_iterator right = const_iterator(
            right_leaf,
            right_leaf->get_start_iterator(),
            right_leaf->get_last_iterator());

        while (right != end && right.first() < hi)
            ++right;

        return const_range(left, right);
    }
    /** Returns a `range` of all elements with key in the interval `[lo, hi)`, i.e. `lo` including and `hi` excluding.
     * */
    range find_range(const key_type &lo, const key_type &hi)
    {
        iterator end = BTree::end();

        if (root == nullptr)
            return range(end, end);

        Node *left_leaf = root->find(lo);
        Node *right_leaf = root->find(hi);

        iterator left = iterator(
            left_leaf,
            left_leaf->get_start_iterator(),
            left_leaf->get_last_iterator());

        while (left != end && (*left).first() < lo)
            ++left;

        iterator right = iterator(
            right_leaf,
            right_leaf->get_start_iterator(),
            right_leaf->get_last_iterator());

        while (right != end && (*right).first() < hi)
            ++right;

        return range(left, right);
    }

    /** Returns a `const_range` of all elements with key equals to \p key. */
    const_range equal_range(const key_type &key) const
    {
        // return find_range(key, key+1);
        iterator end = BTree::end();
        iterator left = BTree::begin();
        while (left != end && left < key)
            ++left;

        iterator right = left;
        while (right != end && right.first() == key)
            ++right;

        return const_range(left, right);

        M_unreachable("not implemented");
    }
    /** Returns a `range` of all elements with key equals to \p key. */
    range equal_range(const key_type &key)
    {
        /* TODO 1.4.7 */
        // return find_range(key, key+1);
        iterator end = BTree::end();
        iterator left = BTree::begin();
        while (left != end && (*left).first() < key)
            ++left;

        iterator right = left;
        while (right != end && (*right).first() == key)
            ++right;

        return range(left, right);
        M_unreachable("not implemented");
    }
};
