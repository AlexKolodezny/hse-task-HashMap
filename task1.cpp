#include <functional>
#include <vector>
#include <list>
#include <stdexcept>

template <class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
    using Item = std::pair<const KeyType, ValueType>;
    struct Node {
        Item data;
        typename std::list<typename std::list<Node>::iterator>::iterator iter;

        Node(const Item other_data)
                : data(other_data), iter() {}
    };

    const size_t kSizeArray = 500'000;
    std::vector<std::list<typename std::list<Node>::iterator>> store_;
    Hash hasher_;
    size_t size_;
    std::list<Node> list_for_iter_;

    template <class TypeIter, class TypeData>
    class Iterator {
        TypeIter iter_;

    public:
        Iterator() {}

        Iterator(TypeIter other_iter): iter_(other_iter) {}

        Iterator& operator++() {
            ++iter_;
            return *this;
        }

        Iterator operator++(int) {
            auto tmp = *this;
            ++iter_;
            return tmp;
        }

        Iterator operator--() {
            --iter_;
            return *this;
        }

        Iterator operator--(int) {
            auto tmp = *this;
            --iter_;
            return tmp;
        }

        TypeData& operator*() const {
            return iter_->data;
        }

        TypeData* operator->() const {
            return &(iter_->data);
        }

        bool operator==(const Iterator& other) const {
            return iter_ == other.iter_;
        }

        bool operator!=(const Iterator& other) const {
            return iter_ != other.iter_;
        }
    };

public:
    using iterator = Iterator<typename std::list<Node>::iterator, Item>;
    using const_iterator = Iterator<typename std::list<Node>::const_iterator,
            const Item>;

    HashMap(Hash hasher = Hash())
            : store_(kSizeArray), hasher_(hasher), size_(0), list_for_iter_() {}

    template <class Iter>
    HashMap(Iter first, Iter last, Hash hasher = Hash())
            : store_(kSizeArray), hasher_(hasher), size_(0), list_for_iter_() {
        while (first != last) {
            insert(*first);
            ++first;
        }
    }

    HashMap(std::initializer_list<Item> data,
            Hash hasher = Hash())
            : store_(kSizeArray), hasher_(hasher), size_(0), list_for_iter_() {
        for (auto it = data.begin(); it != data.end(); ++it) {
            insert(*it);
        }
    }

    HashMap(const HashMap& other):
            store_(kSizeArray), hasher_(other.hasher_), size_(0), list_for_iter_() {
        for (const auto& x : other) {
            insert(x);
        }
    }

    HashMap& operator=(const HashMap& other) {
        if (&other == this) {
            return *this;
        }
        clear();
        for (const auto& x : other) {
            insert(x);
        }
        return *this;
    }

    size_t size() const {
        return size_;
    }

    bool empty() const {
        return size_ == 0;
    }

    Hash hash_function() const {
        return hasher_;
    }

    void insert(const Item& other_item) {
        size_t pos = hasher_(other_item.first) % kSizeArray;
        for (const auto& item : store_[pos]) {
            if (item->data.first == other_item.first) {
                return;
            }
        }
        list_for_iter_.emplace_front(other_item);
        store_[pos].push_front(list_for_iter_.begin());
        list_for_iter_.front().iter = store_[pos].begin();
        ++size_;
    }

    void erase(const KeyType& key) {
        size_t pos = hasher_(key) % kSizeArray;
        for (auto it = store_[pos].begin(); it != store_[pos].end(); ++it) {
            if ((*it)->data.first == key) {
                list_for_iter_.erase(*it);
                store_[pos].erase(it);
                --size_;
                return;
            }
        }
        return;
    }

    const_iterator begin() const {
        return const_iterator(list_for_iter_.begin());
    }

    iterator begin() {
        return iterator(list_for_iter_.begin());
    }

    const_iterator end() const {
        return const_iterator(list_for_iter_.end());
    }

    iterator end() {
        return iterator(list_for_iter_.end());
    }

    iterator find(const KeyType& key) {
        size_t pos = hasher_(key) % kSizeArray;
        for (const auto& item : store_[pos]) {
            if (item->data.first == key) {
                return iterator(item);
            }
        }
        return end();
    }

    const_iterator find(const KeyType& key) const {
        size_t pos = hasher_(key) % kSizeArray;
        for (const auto& item : store_[pos]) {
            if (item->data.first == key) {
                return const_iterator(item);
            }
        }
        return end();
    }

    ValueType& operator[](const KeyType& key) {
        size_t pos = hasher_(key) % kSizeArray;
        for (const auto& item : store_[pos]) {
            if (item->data.first == key) {
                return item->data.second;
            }
        }
        insert({key, ValueType()});
        return operator[](key);
    }

    const ValueType& at(const KeyType& key) const {
        size_t pos = hasher_(key) % kSizeArray;
        for (const auto& item : store_[pos]) {
            if (item->data.first == key) {
                return item->data.second;
            }
        }
        throw std::out_of_range("invalid key");
    }

    void clear() {
        while (!list_for_iter_.empty()) {
            store_[hasher_(list_for_iter_.back().data.first)].erase(list_for_iter_.back().iter);
            list_for_iter_.pop_back();
        }
        size_ = 0;
        return;
    }
};
