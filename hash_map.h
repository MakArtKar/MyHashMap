#include <vector>
#include <stdexcept>
#include <list>
#include <algorithm>
#include <utility>

/*
The implementation of hash map using closed addressing and rebuilding by doubling the size of container, with linear probing.
*/
template<class KeyType, class ValueType, class Hash = std::hash<KeyType> >
class HashMap {
 public:
    using T = std::pair<const KeyType, ValueType>;
    using iterator = typename std::list<T>::iterator;
    using const_iterator = typename std::list<T>::const_iterator;

    // Returns iterator to the begin of Storage. Time: O(1).
    iterator begin() {
        return Storage.begin();
    }

    // Returns constant iterator to the begin of Storage. Time: O(1).
    const_iterator begin() const {
        return Storage.begin();
    }

    // Returns iterator to the end of Storage. Time: O(1).
    iterator end() {
        return Storage.end();
    }

    // Returns constant iterator to the end of Storage. Time: O(1).
    const_iterator end() const {
        return Storage.end();
    }

    // Inserts the element (of type std::pair<const KeyType, ValueType>) to HashMap and reallocates if need. Time: O(1).
    void insert(T const &p) {
        size_t hash_key = getHash(p.first);
        for (const auto it : LoopedContainer[hash_key]) {
            if (it->first == p.first) {
                return;
            }
        }
        LoopedContainer[hash_key].push_back(Storage.insert(Storage.end(), p));
        ++Sz;
        resizeIfNedeed();
    }

    // Erases the element (by key) from HashMap. Time: O(1).
    void erase(KeyType const &key) {
        size_t hash_key = getHash(key);
        for (auto &it : LoopedContainer[hash_key]) {
            if (it->first == key) {
                Storage.erase(it);
                swap(it, *prev(LoopedContainer[hash_key].end()));
                LoopedContainer[hash_key].pop_back();
                --Sz;
                return;
            }
        }
    }

    // Copy assignment operator. Time: O(other size).
    HashMap& operator = (HashMap const &other) {
        if (&other != this)   {
            clear();
            for (auto &temp : other) {
                insert(temp);
            }
        }
        return *this;
    }

    // Move assignment operator. Time: O(1).
    HashMap& operator = (HashMap &&other) {
        std::swap(LoopedContainer, other.LoopedContainer);
        std::swap(Storage, other.Storage);
        std::swap(Sz, other.Sz);
        std::swap(Mod, other.Mod);
        return *this;
    }

    // Empty constructor. Time: O(1).
    HashMap(Hash const &Hasher = Hash()) : Hasher(Hasher) {
        clear();
    }

    // Constructs by iterators on container's borders. Time: O(last - first).
    template <typename Iterator>
    HashMap(Iterator first, Iterator last, Hash const &Hasher = Hash()) :
    Hasher(Hasher) {
        clear();
        for (auto cur = first; cur != last; cur++) {
            insert(*cur);
        }
    }

    // Constructs by initializer_list. Time: O(list size).
    HashMap(std::initializer_list <T> const &l, Hash const &Hasher = Hash()) :
    Hasher(Hasher) {
        clear();
        for (auto &p : l) {
            insert(p);
        }
    }

    // Removes copy assignment constructor.
    explicit HashMap(HashMap const &other) {
        *this = other;
    }

    // Removes move assignment constructor.
    explicit HashMap(HashMap &&other) {
        *this = other;
    }

    // Time: O(1).
    ValueType & operator[](KeyType const &key) {
        insert(std::make_pair(key, ValueType()));
        size_t hash_key = getHash(key);
        for (auto it : LoopedContainer[hash_key]) {
            if (it->first == key) {
                return it->second;
            }
        }
        return Storage.begin()->second;
    }

    // Time: O(1).
    const ValueType& at(KeyType const &key) const {
        size_t hash_key = getHash(key);
        for (auto it : LoopedContainer[hash_key]) {
            if (it->first == key) {
                return it->second;
            }
        }
        throw std::out_of_range({"out_of_range"});
    }

    // Time: O(1).
    size_t size() const {
        return Sz;
    }

    // Time: O(1).
    bool empty() const {
        return Sz == 0;
    }

    // Time: O(1).
    Hash hash_function() const {
        return Hasher;
    }

    // Returns iterator on Storage. Time: O(1).
    iterator find(KeyType const &key) {
        size_t hash_key = getHash(key);
        for (auto it : LoopedContainer[hash_key]) {
            if (it->first == key) {
                return it;
            }
        }
        return Storage.end();
    }

    // Returns constant iterator on Storage. Time: O(1).
    const_iterator find(KeyType const &key) const {
        size_t hash_key = getHash(key);
        for (const auto it : LoopedContainer[hash_key]) {
            if (it->first == key) {
                return it;
            }
        }
        return Storage.end();
    }

    // Time: O(1).
    void clear() {
        Sz = 0, Mod = 1;
        Storage.clear();
        LoopedContainer.assign(Mod, {});
    }

  private:
    std::list<T> Storage;
    std::vector<std::vector<iterator> > LoopedContainer;
    size_t Mod;
    size_t Sz;
    Hash Hasher;

    // Resizes LoopedContainer to Mod and fills with elements from Storage. Time: O(Mod + Sz).
    void reallocate() {
        LoopedContainer.assign(Mod, {});
        for (auto it = Storage.begin(); it != Storage.end(); it++) {
            LoopedContainer[Hasher(it->first) % Mod].push_back(it);
        }
    }

    // Returns hash of key. Time: O(Hash).
    size_t getHash(KeyType const &key) const {
        return Hasher(key) % Mod;
    }

    // Doubles the size of the container if Sz >= Mod. Time: O(Sz). 
    void resizeIfNedeed() {
    	if (Sz >= Mod) {
            Mod *= 2;
            reallocate();
        }
    }
};
