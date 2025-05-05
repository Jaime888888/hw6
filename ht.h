#ifndef HT_H
#define HT_H
#include <vector>
#include <iostream>
#include <cmath>

typedef size_t HASH_INDEX_T;


// Complete - Base Prober class
template <typename KeyType>
struct Prober {
    // Data members
    HASH_INDEX_T start_;    // initial hash location, h(k)
    HASH_INDEX_T m_;        // table size
    size_t numProbes_;      // probe attempts for statistic tracking
    static const HASH_INDEX_T npos = (HASH_INDEX_T)-1; // used to indicate probing failed
    void init(HASH_INDEX_T start, HASH_INDEX_T m, const KeyType& key) 
    {
        (void) key;  // avoid unused argument warnings since base class doesn't use key
        start_ = start;
        m_ = m;
        numProbes_ = 0;
    }
    HASH_INDEX_T next() {
        throw std::logic_error("Not implemented...should use derived class");
    }
};

// Almost Complete - Fill in the if statement below.
template <typename KeyType>
struct LinearProber : public Prober<KeyType> {

    HASH_INDEX_T next() 
    {
        // Complete the condition below that indicates failure
        // to find the key or an empty slot
        if(this->numProbes_ == this->m_) {
            return this->npos; 
        }
        HASH_INDEX_T loc = (this->start_ + this->numProbes_) % this->m_;
        this->numProbes_++;
        return loc;
    }
};

// To be completed
template <typename KeyType, typename Hash2>
struct DoubleHashProber : public Prober<KeyType> 
{
    Hash2 h2_;              /// h2(k)
    HASH_INDEX_T dhstep_;   /// Stepsize to use for double hash probing

    /// Moduli to use for double hashing as table increases (resizes)
    static const HASH_INDEX_T DOUBLE_HASH_MOD_VALUES[]; 
    /// The number of elements in the array above
    static const int DOUBLE_HASH_MOD_SIZE;

    //==================================
    // Add data members, as desired
    //==================================
    HASH_INDEX_T modulus_;  // Stores modulus used in current init()

private:
    // Complete
    HASH_INDEX_T findModulusToUseFromTableSize(HASH_INDEX_T currTableSize)
    {
        HASH_INDEX_T modulus = DOUBLE_HASH_MOD_VALUES[0];
        // find the modulus that is just smaller than the table size
        for(int i=0; i < DOUBLE_HASH_MOD_SIZE && DOUBLE_HASH_MOD_VALUES[i] < currTableSize; i++)
        {
            modulus = DOUBLE_HASH_MOD_VALUES[i];
        }
        return modulus;
    }
public:
    /**
     * @brief Construct a new Double Hash Prober
     */
    DoubleHashProber(const Hash2& h2 = Hash2()) : h2_(h2) {}
    
    /**
     * @brief Supplies info the hash table must provide
     */
    void init(HASH_INDEX_T start, HASH_INDEX_T m, const KeyType& key) 
    {
        Prober<KeyType>::init(start, m, key);
        modulus_ = findModulusToUseFromTableSize(m);
        dhstep_ = modulus_ - h2_(key) % modulus_;
    }

    // To be completed
    HASH_INDEX_T next() 
    {
        if(this->numProbes_ == this->m_) {
            return this->npos;
        }
        HASH_INDEX_T loc = (this->start_ + this->numProbes_ * dhstep_) % this->m_;
        this->numProbes_++;
        return loc;
    }
};

// Initialization of static array (do not alter)
template <typename KeyType, typename Hash2>
const HASH_INDEX_T DoubleHashProber<KeyType, Hash2>::DOUBLE_HASH_MOD_VALUES[] =
{
    7, 19, 43, 89, 193, 389, 787, 1583, 3191, 6397, 12841, 25703, 51431, 102871,
    205721, 411503, 823051, 1646221, 3292463, 6584957, 13169963, 26339921, 52679927,
    105359939, 210719881, 421439749, 842879563, 1685759113
};

// Initialization of static array size (do not alter)
template <typename KeyType, typename Hash2>
const int DoubleHashProber<KeyType, Hash2>::DOUBLE_HASH_MOD_SIZE = 
    sizeof(DoubleHashProber<KeyType, Hash2>::DOUBLE_HASH_MOD_VALUES)/sizeof(HASH_INDEX_T);
















template<
    typename K, 
    typename V, 
    typename Prober = LinearProber<K>,
    typename Hash = std::hash<K>, 
    typename KEqual = std::equal_to<K> >
class HashTable
{
public:
    typedef K KeyType;
    typedef V ValueType;
    typedef std::pair<KeyType, ValueType> ItemType;
    typedef Hash Hasher;
    struct HashItem {
        ItemType item;
        bool deleted;
        HashItem(const ItemType& newItem){
            item = newItem;
            deleted = false;
        }
    };

    HashTable(double resizeAlpha = 0.4, 
              const Prober& prober = Prober(),
              const Hasher& hash = Hasher(), 
              const KEqual& kequal = KEqual());
    ~HashTable();
    bool empty() const;
    size_t size() const;
    void insert(const ItemType& p);
    void remove(const KeyType& key);
    ItemType const * find(const KeyType& key) const;
    ItemType * find(const KeyType& key);
    const ValueType& at(const KeyType& key) const;
    ValueType& at(const KeyType& key);
    const ValueType& operator[](const KeyType& key) const;
    ValueType& operator[](const KeyType& key);

    void reportAll(std::ostream& out) const;
    void clearTotalProbes() { totalProbes_ = 0; }
    size_t totalProbes() const { return totalProbes_; }

private:
    HashItem * internalFind(const KeyType& key) const;
    HASH_INDEX_T probe(const KeyType& key) const;
    void resize();

    std::vector<HashItem*> table_;
    Hasher hash_;
    KEqual kequal_;
    mutable Prober prober_;
    mutable size_t totalProbes_;
    static const HASH_INDEX_T CAPACITIES[];
    HASH_INDEX_T mIndex_;

    // Missing data members you used but didn't declare:
    double resizeAlpha_;
    size_t numItems_;
    size_t numDeleted_;
};






// ----------------------------------------------------------------------------
//                           Hash Table Implementation
// ----------------------------------------------------------------------------

// Static array of prime table sizes
template<typename K, typename V, typename Prober, typename Hash, typename KEqual>
const HASH_INDEX_T HashTable<K,V,Prober,Hash,KEqual>::CAPACITIES[] =
    {
        11, 23, 47, 97, 197, 397, 797, 1597, 3203, 6421, 12853, 25717, 51437, 102877,
        205759, 411527, 823117, 1646237, 3292489, 6584983, 13169977, 26339969, 52679969,
        105359969, 210719881, 421439783, 842879579, 1685759167
    };

// To be completed
template<typename K, typename V, typename Prober, typename Hash, typename KEqual>
HashTable<K,V,Prober,Hash,KEqual>::HashTable(
    double resizeAlpha, const Prober& prober, const Hasher& hash, const KEqual& kequal)
       :  hash_(hash), kequal_(kequal), prober_(prober)
{
    mIndex_ = 0;
    totalProbes_ = 0;
    table_.resize(CAPACITIES[mIndex_], nullptr);
    resizeAlpha_ = resizeAlpha;
    numItems_ = 0;
    numDeleted_ = 0;
}

// To be completed
template<typename K, typename V, typename Prober, typename Hash, typename KEqual>
HashTable<K,V,Prober,Hash,KEqual>::~HashTable()
{
    for (size_t i = 0; i < table_.size(); ++i)
    {
        delete table_[i];
    }
}

// To be completed
template<typename K, typename V, typename Prober, typename Hash, typename KEqual>
bool HashTable<K,V,Prober,Hash,KEqual>::empty() const
{
    return numItems_ == 0;
}

// To be completed
template<typename K, typename V, typename Prober, typename Hash, typename KEqual>
size_t HashTable<K,V,Prober,Hash,KEqual>::size() const
{
    return numItems_;
}

// Almost complete
template<typename K, typename V, typename Prober, typename Hash, typename KEqual>
HASH_INDEX_T HashTable<K,V,Prober,Hash,KEqual>::probe(const KeyType& key) const
{
    HASH_INDEX_T h = hash_(key) % CAPACITIES[mIndex_];
    prober_.init(h, CAPACITIES[mIndex_], key);

    HASH_INDEX_T loc = prober_.next(); 
    totalProbes_++;
    while(Prober::npos != loc)
    {
        if(nullptr == table_[loc] ) {
            return loc;
        }
        // fill in the condition for this else if statement which should 
        // return 'loc' if the given key exists at this location
        else if(!table_[loc]->deleted && kequal_(table_[loc]->item.first, key)) {
            return loc;
        }
        loc = prober_.next();
        totalProbes_++;
    }

  return Prober::npos;

}


template<typename K, typename V, typename Prober, typename Hash, typename KEqual>
typename HashTable<K,V,Prober,Hash,KEqual>::HashItem* 
HashTable<K,V,Prober,Hash,KEqual>::internalFind(const KeyType& key) const
{
    HASH_INDEX_T loc = probe(key);
    if(loc == Prober::npos || table_[loc] == nullptr || table_[loc]->deleted) {
        return nullptr;
    }
    return table_[loc];
}

template<typename K, typename V, typename Prober, typename Hash, typename KEqual>
typename HashTable<K,V,Prober,Hash,KEqual>::ItemType const *
HashTable<K,V,Prober,Hash,KEqual>::find(const KeyType& key) const
{
    HashItem* item = internalFind(key);
    if(item == nullptr) return nullptr;
    return &item->item;
}

template<typename K, typename V, typename Prober, typename Hash, typename KEqual>
typename HashTable<K,V,Prober,Hash,KEqual>::ItemType *
HashTable<K,V,Prober,Hash,KEqual>::find(const KeyType& key)
{
    return const_cast<ItemType*>(static_cast<const HashTable<K,V,Prober,Hash,KEqual>*>(this)->find(key));
}

template<typename K, typename V, typename Prober, typename Hash, typename KEqual>
const V& HashTable<K,V,Prober,Hash,KEqual>::at(const KeyType& key) const
{
    ItemType const* item = find(key);
    if(item == nullptr) {
        throw std::out_of_range("Key not found");
    }
    return item->second;
}

template<typename K, typename V, typename Prober, typename Hash, typename KEqual>
V& HashTable<K,V,Prober,Hash,KEqual>::at(const KeyType& key)
{
    return const_cast<V&>(static_cast<const HashTable<K,V,Prober,Hash,KEqual>*>(this)->at(key));
}

template<typename K, typename V, typename Prober, typename Hash, typename KEqual>
const V& HashTable<K,V,Prober,Hash,KEqual>::operator[](const KeyType& key) const
{
    return at(key);
}

template<typename K, typename V, typename Prober, typename Hash, typename KEqual>
V& HashTable<K,V,Prober,Hash,KEqual>::operator[](const KeyType& key)
{
    ItemType* item = find(key);
    if(item != nullptr) {
        return item->second;
    }
    insert(std::make_pair(key, V()));
    return find(key)->second;
}

template<typename K, typename V, typename Prober, typename Hash, typename KEqual>
void HashTable<K,V,Prober,Hash,KEqual>::insert(const ItemType& p)
{
    if(static_cast<double>(numItems_ + numDeleted_) / CAPACITIES[mIndex_] >= resizeAlpha_) {
        resize();
    }

    HASH_INDEX_T loc = probe(p.first);
    if(loc == Prober::npos) {
        throw std::logic_error("No available slot found");
    }

    if(table_[loc] == nullptr) {
        table_[loc] = new HashItem(p);
        numItems_++;
    } 
    else if(table_[loc]->deleted) {
        table_[loc]->item = p;
        table_[loc]->deleted = false;
        numItems_++;
        numDeleted_--;
    } 
    else {
        table_[loc]->item.second = p.second;
    }
}

template<typename K, typename V, typename Prober, typename Hash, typename KEqual>
void HashTable<K,V,Prober,Hash,KEqual>::remove(const KeyType& key)
{
    HashItem* item = internalFind(key);
    if(item != nullptr && !item->deleted) {
        item->deleted = true;
        numItems_--;
        numDeleted_++;
    }
}

template<typename K, typename V, typename Prober, typename Hash, typename KEqual>
void HashTable<K,V,Prober,Hash,KEqual>::resize()
{
    size_t oldSize = table_.size();
    mIndex_++;
    if(mIndex_ >= sizeof(CAPACITIES)/sizeof(HASH_INDEX_T)) {
        throw std::logic_error("No more sizes available");
    }

    std::vector<HashItem*> oldTable = table_;
    table_.clear();
    table_.resize(CAPACITIES[mIndex_], nullptr);
    numItems_ = 0;
    numDeleted_ = 0;

    for(size_t i = 0; i < oldSize; ++i) {
        if(oldTable[i] != nullptr && !oldTable[i]->deleted) {
            insert(oldTable[i]->item);
        }
        delete oldTable[i];
    }
}

template<typename K, typename V, typename Prober, typename Hash, typename KEqual>
void HashTable<K,V,Prober,Hash,KEqual>::reportAll(std::ostream& out) const
{
    for(size_t i = 0; i < table_.size(); ++i) {
        if(table_[i] != nullptr && !table_[i]->deleted) {
            out << i << ": " << table_[i]->item.first << " " << table_[i]->item.second << std::endl;
        }
    }
}



#endif









