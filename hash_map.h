#include <vector>
#include <stdexcept>
#include <list>
#include <algorithm>
#include <utility>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType> >
class HashMap {
 public:
    using T = std::pair <const KeyType, ValueType>;
    using iterator = typename std::list <T>::iterator;
    using const_iterator = typename std::list <T>::const_iterator;

 private:
    std::list <T> keys;
    std::vector <std::vector <iterator> > mas;
    int mod, n;
    Hash hasher;

    void reallocate() {
        mas.assign(mod, {});
        for (auto it = keys.begin(); it != keys.end(); it++) {
            mas[hasher(it->first) % mod].push_back(it);
        }
    }
    size_t get_hash(KeyType const &key) const {
        return hasher(key) % mod;
    }

 public:
    iterator begin() {
        return keys.begin();
    }
    const_iterator begin() const {
        return keys.begin();
    }
    iterator end() {
        return keys.end();
    }
    const_iterator end() const {
        return keys.end();
    }
    void insert(T const &p) {
        size_t hash_key = get_hash(p.first);
        for (auto it : mas[hash_key]) {
            if (it->first == p.first) {
                return;
            }
        }
        mas[hash_key].push_back(keys.insert(keys.end(), p));
        if (++n >= mod) {
            mod *= 2;
            reallocate();
        }
    }
    void erase(KeyType const &key) {
        size_t hash_key = get_hash(key);
        for (auto &it : mas[hash_key]) {
            if (it->first == key) {
                keys.erase(it);
                swap(it, *prev(mas[hash_key].end()));
                mas[hash_key].pop_back();
                --n;
                if (n * 4 < mod) {
                    mod /= 2;
                    reallocate();
                }
                return;
            }
        }
    }

    HashMap& operator = (HashMap const &other) {
        if (&other != this)   {
            clear();
            for (auto &temp : other) {
                insert(temp);
            }
        }
        return *this;
    }
    HashMap& operator = (HashMap &&other) {
        std::swap(mas, other.mas);
        std::swap(keys, other.keys);
        std::swap(n, other.n);
        std::swap(mod, other.mod);
        return *this;
    }

    HashMap(Hash const &hasher = Hash()) : hasher(hasher) {
        clear();
    }
    template <typename Iterator>
    HashMap(Iterator first, Iterator last, Hash const &hasher = Hash()) :
    hasher(hasher) {
        clear();
        for (auto cur = first; cur != last; cur++) {
            insert(*cur);
        }
    }
    HashMap(std::initializer_list <T> const &l, Hash const &hasher = Hash()) :
    hasher(hasher) {
        clear();
        for (auto &p : l) {
            insert(p);
        }
    }
    explicit HashMap(HashMap const &other) {
        *this = other;
    }
    explicit HashMap(HashMap &&other) {
        *this = other;
    }

    ValueType & operator[](KeyType const &key) {
        insert(make_pair(key, ValueType()));
        size_t hash_key = get_hash(key);
        for (auto it : mas[hash_key]) {
            if (it->first == key) {
                return it->second;
            }
        }
        return keys.begin()->second;
    }
    const ValueType& at(KeyType const &key) const {
        size_t hash_key = get_hash(key);
        for (auto it : mas[hash_key]) {
            if (it->first == key) {
                return it->second;
            }
        }
        throw std::out_of_range({"out_of_range"});
    }

    size_t size() const {return n;}
    bool empty() const {return n == 0;}
    Hash hash_function() const {return hasher;}

    iterator find(KeyType const &key) {
        size_t hash_key = get_hash(key);
        for (auto it : mas[hash_key]) {
            if (it->first == key) {
                return it;
            }
        }
        return keys.end();
    }
    const_iterator find(KeyType const &key) const {
        size_t hash_key = get_hash(key);
        for (auto it : mas[hash_key]) {
            if (it->first == key) {
                return it;
            }
        }
        return keys.end();
    }

    void clear() {
        n = 0, mod = 1;
        keys.clear();
        mas.assign(mod, {});
    }
};
