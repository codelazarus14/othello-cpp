#include "othello.h"
#include "othello-rules.h"
#include "hash-table.h"

int main() {
  HashTable<Othello> hashTable;

  std::cout << hashTable;

  // testing insertion
  Othello o1;
  hashTable.insert(o1.getHashKey(), o1);

  std::cout << hashTable;

  // test contains/get and references
  if (hashTable.contains(o1.getHashKey())) {
    Othello o2 = hashTable.get(o1.getHashKey());
    std::cout << "\nO2:" << o2;
    o2 = doMove(o2, true, 5, 4);
    std::cout << "\nO2 after:" << o2;
    std::cout << "\nO1" << o1;
    
    // testing deletion
    hashTable.remove(o1.getHashKey());
    std::cout << (hashTable.contains(o1.getHashKey()) ? "\nTrue\n" : "\nFalse\n");
    std::cout << hashTable;

    // just making sure the hash function is deterministic
    Othello o3;
    hashTable.insert(o3.getHashKey(), o3);
    std::cout << hashTable;
    hashTable.insert(o2.getHashKey(), o2);
    std::cout << hashTable;
    // testing collision
    hashTable.insert(o2.getHashKey() + 400000, o2);
    std::cout << hashTable;
  }
}