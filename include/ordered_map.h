#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <initializer_list>
#include <utility>
#include <cstdint>
#include <functional>

namespace vertrag
{
#define VERT_CHECK(c, s) \
    if (c)               \
    {                    \
        std::cout << s;  \
        exit(-1);        \
    }

    template <typename Key, typename Value>
    class ordered_map
    {
    public:
        // <key, value> pair
        class Item;
        using Iterator = typename std::vector<Item>::iterator;
        using ConstIterator = typename std::vector<Item>::const_iterator;

        // default constructor
        ordered_map() = default;

        // copy constructor
        ordered_map(const ordered_map &other);
        ordered_map &operator=(const ordered_map &other);

        // move constructor
        ordered_map(ordered_map &&other) = default;
        ordered_map &operator=(ordered_map &&other) = default;

        ~ordered_map() = default;

        // Constructs a new ordered_map and pre-populate it with given Items
        ordered_map(std::initializer_list<Item> initializer_list);

        // Element Access functions

        // Return first item in ordered_map
        Item &front();
        const Item &front() const;

        // Return last item in ordered_map
        Item &back();
        const Item &back() const;

        // Returns the item at index in the ordered_map
        Item &operator[](size_t index);
        const Item &operator[](size_t index) const;

        // Returns the value associated with the given key.
        Value &operator[](const Key &key);
        const Value &operator[](const Key &key) const;

        // Lookup functions

        // Returns a pointer to the value associated with the given key or nullptr if it doesn't exist
        Value *find(const Key &key) noexcept;
        const Value *find(const Key &key) const noexcept;

        // Returns true if the key is present in ordered_map
        bool contains(const Key &key) const noexcept;

        // Iterator functions

        // Returns an iterator to the first item in the ordered_map
        Iterator begin();
        ConstIterator begin() const;

        // Returns an iterator one past the last item in the ordered_map
        Iterator end();
        ConstIterator end() const;

        // Capacity functions

        // Returns the number of items currently stored in the ordered_map
        size_t size() const noexcept;

        // Returns true if ordered_map has no items
        bool is_empty() const noexcept;

        // Resizes ordered_map to fit at least requested_capacity items without requiring reallocation
        void reserve(size_t requested_capacity);

        // Modifier functions

        // Insert <key, value> pair into the ordered_map.
        template <typename K, typename V>
        Value &insert(K &&key, V &&value);

        Value &insert(Key key, Value &&value);

        // Insert all items from other in this ordered_map
        void update(const ordered_map &other);
        void update(ordered_map &&other);

        // Removes item with given key
        void erase(const Key &key);

        // Removes all elements in ordered_map
        void clear();

        // Observer Functions

        // Return all Items in ordered_map
        const std::vector<Item> &items() const noexcept;

        // Return a newly allocated vector and copy all keys from this ordered_map into the vector
        std::vector<Key> keys() const;

        std::vector<Value> values() const;

        std::vector<std::pair<Key, Value>> pairs() const;

        template <typename K, typename V>
        friend bool operator==(const ordered_map<K, V> &a, const ordered_map<K, V> &b);

    private:
        // a mapping from a key to index into the _items vector
        std::unordered_map<Key, size_t> _index;

        // The items stored in ordered_map. Being a vector preserves insertion order
        std::vector<Item> _items;
    };

    /*------------------------------------ordered_map::Item-------------------------------------*/
    template <typename Key, typename Value>
    class ordered_map<Key, Value>::Item
    {
    public:
        Item(Key key, Value value) : _pair(std::move(key), std::move(value)){};

        Item &operator=(const Item &other)
        {
            _pair = other._pair;
            return *this;
        }

        // Observer functions

        Key &first() noexcept
        {
            return _pair.first;
        }

        const Key &first() const noexcept
        {
            return _pair.first;
        }

        Key &key() noexcept
        {
            return _pair.first;
        }

        const Key &key() const noexcept
        {
            return _pair.first;
        }

        Value &second() noexcept
        {
            return _pair.second;
        }

        const Value &second() const noexcept
        {
            return _pair.second;
        }

        Value &value() noexcept
        {
            return _pair.second;
        }

        const Value &value() const noexcept
        {
            return _pair.second;
        }

        const std::pair<Key, Value> &pair() const noexcept
        {
            return _pair;
        }

        // Operators

        Value &operator*()
        {
            return value();
        }

        const Value &operator*() const
        {
            return value();
        }

        Value *operator->()
        {
            return &value();
        }

        const Value *operator->() const
        {
            return &value();
        }

    private:
        std::pair<Key, Value> _pair;
    };

    /*----------------------------------------ordered_map-----------------------------------*/
    template <typename Key, typename Value>
    ordered_map<Key, Value>::ordered_map(const ordered_map &other)
        : _index(other._index), _items(other._items) {}

    template <typename Key, typename Value>
    ordered_map<Key, Value> &ordered_map<Key, Value>::operator=(const ordered_map &other)
    {
        _index = other._index;
        _items.clear();
        for (auto &i : other._items)
            _items.push_back(i);
        return *this;
    }

    template <typename Key, typename Value>
    ordered_map<Key, Value>::ordered_map(std::initializer_list<Item> initializer_list)
    {
        _items.reserve(initializer_list.size());
        for (auto &i : initializer_list)
        {
            _items.emplace_back(i.key(), std::move(i.value()));
            _index.emplace(std::move(i.key()), size() - 1);
        }
    }

    template <typename Key, typename Value>
    typename ordered_map<Key, Value>::Iterator ordered_map<Key, Value>::begin()
    {
        return _items.begin();
    }

    template <typename Key, typename Value>
    typename ordered_map<Key, Value>::ConstIterator ordered_map<Key, Value>::begin() const
    {
        return _items.begin();
    }

    template <typename Key, typename Value>
    typename ordered_map<Key, Value>::Iterator ordered_map<Key, Value>::end()
    {
        return _items.end();
    }

    template <typename Key, typename Value>
    typename ordered_map<Key, Value>::ConstIterator ordered_map<Key, Value>::end() const
    {
        return _items.end();
    }

    template <typename Key, typename Value>
    size_t ordered_map<Key, Value>::size() const noexcept
    {
        return _items.size();
    }

    template <typename Key, typename Value>
    bool ordered_map<Key, Value>::is_empty() const noexcept
    {
        return _items.empty();
    }

    template <typename Key, typename Value>
    void ordered_map<Key, Value>::clear()
    {
        _items.clear();
        _index.clear();
    }

    template <typename Key, typename Value>
    typename ordered_map<Key, Value>::Item &ordered_map<Key, Value>::front()
    {
        VERT_CHECK(_items.empty(), "Called front() on empty map");
        return _items.front();
    }

    template <typename Key, typename Value>
    const typename ordered_map<Key, Value>::Item &ordered_map<Key, Value>::front() const
    {
        VERT_CHECK(_items.empty(), "Called front() on empty map");
        return _items.front();
    }

    template <typename Key, typename Value>
    typename ordered_map<Key, Value>::Item &ordered_map<Key, Value>::back()
    {
        VERT_CHECK(_items.empty(), "Called back() on empty map");
        return _items.back();
    }

    template <typename Key, typename Value>
    const typename ordered_map<Key, Value>::Item &ordered_map<Key, Value>::back() const
    {
        VERT_CHECK(_items.empty(), "Called back() on empty map");
        return _items.back();
    }

    template <typename Key, typename Value>
    typename ordered_map<Key, Value>::Item &ordered_map<Key, Value>::operator[](size_t index)
    {
        VERT_CHECK(index > _items.size(), "index out of bounds");
        return _items[index];
    }

    template <typename Key, typename Value>
    const typename ordered_map<Key, Value>::Item &ordered_map<Key, Value>::operator[](size_t index) const
    {
        VERT_CHECK(index > _items.size(), "index out of bounds");
        return _items[index];
    }

    template <typename Key, typename Value>
    Value &ordered_map<Key, Value>::operator[](const Key &key)
    {
        // VERT_CHECK((_index.find(key) == _index.end()), "Key not found");
        if (_index.find(key) == _index.end())
        {
            insert(key, std::move(Value{}));
        }
        return _items[_index[key]].value();
    }

    template <typename Key, typename Value>
    const Value &ordered_map<Key, Value>::operator[](const Key &key) const
    {
        // VERT_CHECK((_index.find(key) == _index.end()), "Key not found");
        if (_index.find(key) == _index.end())
        {
            insert(key, std::move(Value{}));
        }
        return _items[_index[key]].value();
    }

    template <typename Key, typename Value>
    Value *ordered_map<Key, Value>::find(const Key &key) noexcept
    {
        auto iterator = _index.find(key);
        if (iterator == _index.end())
        {
            return nullptr;
        }
        return &_items[iterator->second].value();
    }

    template <typename Key, typename Value>
    const Value *ordered_map<Key, Value>::find(const Key &key) const noexcept
    {
        auto iterator = _index.find(key);
        if (iterator == _index.end())
        {
            return nullptr;
        }
        return &_items[iterator->second].value();
    }

    template <typename Key, typename Value>
    void ordered_map<Key, Value>::erase(const Key &key)
    {
        auto it = _index.find(key);
        VERT_CHECK(it == _index.end(), "Key not found");
        auto index = it->second();
        _index.erase(it);
        _items.erase(_items.begin() + index);
        for (auto &pair : _index)
        {
            if (pair.second > index)
                --pair.second;
        }
    }

    template <typename Key, typename Value>
    bool ordered_map<Key, Value>::contains(const Key &key) const noexcept
    {
        return find(key) != nullptr;
    }

    template <typename Key, typename Value>
    template <typename K, typename V>
    Value &ordered_map<Key, Value>::insert(K &&key, V &&value)
    {
        VERT_CHECK(_index.find(key) != _index.end(), "Key trying to be inserted already exists")
        _items.emplace_back(key, std::forward<V>(value));
        _index.emplace(std::forward<K>(key), size() - 1);
        return _items.back().value();
    }

    template <typename Key, typename Value>
    Value &ordered_map<Key, Value>::insert(Key key, Value &&value)
    {
        return insert<Key, Value>(std::move(key), std::move(value));
    }

    template <typename Key, typename Value>
    void ordered_map<Key, Value>::reserve(size_t requested_capacity)
    {
        _items.reserve(requested_capacity);
        _index.reserve(requested_capacity);
    }

    template <typename Key, typename Value>
    void ordered_map<Key, Value>::update(const ordered_map &other)
    {
        reserve(size() + other.size());
        for (auto &i : other)
        {
            insert(i.key(), i.value());
        }
    }

    template <typename Key, typename Value>
    void ordered_map<Key, Value>::update(ordered_map &&other)
    {
        reserve(size() + other.size());
        for (auto &i : other)
        {
            insert(std::move(i.key()), std::move(i.value()));
        }
    }

    template <typename Key, typename Value>
    std::vector<Key> ordered_map<Key, Value>::keys() const
    {
        std::vector<Key> keys;
        keys.reserve(size());
        for (const auto &i : _items)
        {
            keys.push_back(i.key());
        }
        return keys;
    }

    template <typename Key, typename Value>
    std::vector<Value> ordered_map<Key, Value>::values() const
    {
        std::vector<Value> values;
        values.reserve(size());
        for (const auto &i : _items)
        {
            values.push_back(i.value());
        }
        return values;
    }

    template <typename Key, typename Value>
    std::vector<std::pair<Key, Value>> ordered_map<Key, Value>::pairs() const
    {
        std::vector<std::pair<Key, Value>> pairs;
        pairs.reserve(size());
        for (const auto &i : _items)
        {
            pairs.push_back(i.pair());
        }
        return pairs;
    }
} // namespace vertrag
