#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <iostream>
#include <array>
#include <vector>
#include <sstream>

constexpr int g_tableCapacity{10000};

template <typename T>
class HashTable {
  private:
    std::vector<std::vector<std::pair<size_t, T>>> m_table;
  public:
    HashTable() { m_table.resize(g_tableCapacity); }
    // call before getItem() to avoid an exception or just to check
    bool contains(size_t key);
    // throws std::invalid_argument exception if the item is not found
    T& get(size_t key);
    void insert(size_t key, const T& value);
    void remove(size_t key);
    // have to inline this because it doesn't like template classes
    friend std::ostream& operator<<(std::ostream& out, const HashTable& table) {
      out << "Hash Table:\n-------------------\n";
      for (int i = 0; i < g_tableCapacity; i++)
      {
        std::vector<std::pair<size_t, T>> bucket{table.m_table[i]};
        if (bucket.empty()) continue;
        
        out << "Index: " << i << "\n";
        auto itr{bucket.begin()};
        for (; itr != bucket.end(); itr++)
        {
          std::pair<size_t, T> item{*itr};
          out << "  Key: " << item.first << "\n";
        }
      }
      out << "-------------------\n";
      return out;
    }
};

// due to how the compiler instantiates template classes while compiling individual files, the old hash-table.cpp was removed so that all the definitions in it can be exposed to any other file using these functions directly (so we might as well copy the definitions in here...)

static size_t hash(size_t key) { return key % g_tableCapacity; }

template <typename T>
bool HashTable<T>::contains(size_t key) {
  int bucket = hash(key);
  if (bucket < 0 || bucket >= g_tableCapacity)
    return false;
  
  for (int i = 0; i < m_table[bucket].size(); i++) {
    if (m_table[bucket][i].first == key)
      return true;
  }
  return false;
}

template <typename T>
T& HashTable<T>::get(size_t key) {
  int bucket = hash(key);
  
  for (int i = 0; i < m_table[bucket].size(); i++) {
    if (m_table[bucket][i].first == key)
      return m_table[bucket][i].second;
  }

  std::ostringstream err;
  err << "Key " << key << " not found in table!";
  throw std::invalid_argument(err.str());
}

template <typename T>
void HashTable<T>::insert(size_t key, const T& value) {
  int index = hash(key);
  m_table[index].push_back({key, value});
}

template <typename T>
void HashTable<T>::remove(size_t key) {
  int index = hash(key);

  auto itr{m_table[index].begin()};
  for (; itr != m_table[index].end(); itr++) {
    if ((*itr).first == key) {
      m_table[index].erase(itr);
      break;
    }
  }
}

#endif