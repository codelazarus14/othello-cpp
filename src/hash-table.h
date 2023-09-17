#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <iostream>
#include <array>
#include <vector>
#include <sstream>

constexpr int g_tableCapacity = 10000;

template <typename T>
class HashTable {
  private:
    std::array<std::vector<std::pair<int, T>>, g_tableCapacity> m_table;
    int hash(int key) { return key % g_tableCapacity; }
  public:
    // call before getItem() to avoid an exception or just to check
    bool contains(int key);
    // throws std::invalid_argument exception if the item is not found
    const T& get(int key);
    void insert(int key, const T& value);
    void remove(int key);
    // have to inline this because it doesn't like template classes
    friend std::ostream& operator<<(std::ostream& out, const HashTable& table) {
      out << "Hash Table:\n-------------------\n";
      for (int i = 0; i < g_tableCapacity; i++)
      {
        std::vector<std::pair<int, T>> bucket = table.m_table[i];
        if (bucket.empty()) continue;
        
        out << "Index: " << i << "\n";
        std::vector<std::pair<int, T>>::iterator itr;
        for (itr = bucket.begin(); itr != bucket.end(); itr++)
        {
          std::pair<int, T> item = *itr;
          out << "  Key: " << item.first << "\n";
        }
      }
      out << "-------------------\n";
      return out;
    }
};

// due to how the compiler instantiates template classes while compiling individual files, the old hash-table.cpp was removed so that all the definitions in it can be exposed to any other file using these functions directly (so we might as well copy the definitions in here...)

template <typename T>
bool HashTable<T>::contains(int key) {
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
const T& HashTable<T>::get(int key) {
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
void HashTable<T>::insert(int key, const T& value) {
  int index = hash(key);
  m_table[index].push_back({key, value});
}

template <typename T>
void HashTable<T>::remove(int key) {
  int index = hash(key);

  std::vector<std::pair<int, T>>::iterator itr;
  for (itr = m_table[index].begin(); itr != m_table[index].end(); itr++) {
    if ((*itr).first == key) {
      m_table[index].erase(itr);
      break;
    }
  }
}

#endif