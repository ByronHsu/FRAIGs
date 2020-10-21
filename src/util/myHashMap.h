/****************************************************************************
  FileName     [ myHashMap.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashMap and Cache ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2009-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_MAP_H
#define MY_HASH_MAP_H

#include <vector>

using namespace std;

// TODO: (Optionally) Implement your own HashMap and Cache classes.

//-----------------------
// Define HashMap classes
//-----------------------
// To use HashMap ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//
// class HashKey
// {
// public:
//    HashKey() {}
// 
//    size_t operator() () const { return 0; }
// 
//    bool operator == (const HashKey& k) const { return true; }
// 
// private:
// };
//
template <class HashKey, class HashData>
class HashMap
{
typedef pair<HashKey, HashData> HashNode;

public:
   HashMap(size_t b=0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashMap() { reset(); }

   // [Optional] TODO: implement the HashMap<HashKey, HashData>::iterator
   // o An iterator should be able to go through all the valid HashNodes
   //   in the HashMap
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashMap<HashKey, HashData>;

   public:
      iterator(vector<HashNode>* n, size_t num, size_t r, typename vector<HashNode>::iterator itr): 
               _buckets(n), _numBuckets(num), _row(r), itr(itr) {}
      iterator(const iterator& i): 
               _buckets(i._buckets), _numBuckets(i._numBuckets), _row(i._row), itr(i.itr) {}
      ~iterator() {} 
      const HashNode& operator * () const { return *itr; }
      iterator& operator ++ () { 
         if(itr+1 != _buckets[_row].end()) itr++;
         else{
            _row = _row + 1;
            while(_row < _numBuckets && _buckets[_row].empty()){
               _row++;
            }
            if(_row == _numBuckets) {itr = _buckets[_numBuckets-1].end(); _row = _numBuckets-1;}
            else itr = _buckets[_row].begin();
         }
         return (*this); 
      }
      iterator operator ++ (int) { iterator temp = (*this); ++(*this); return temp;}
      iterator& operator = (const iterator& i) { 
         itr = i.itr;
         _buckets = i._buckets; 
         _numBuckets = i._numBuckets;
         _row = i._row; 
         return (*this); 
      }
      bool operator == (const iterator& i) const {
         if(itr == i.itr && _buckets == i._buckets 
            && _numBuckets == i._numBuckets && _row == i._row) return true;
         else return false;
      }
      bool operator != (const iterator& i) const { if(*this == i) return false; else return true; }
   private:
      vector<HashNode>*     _buckets;
      size_t            _numBuckets;
      size_t            _row;
      typename vector<HashNode>::iterator itr;
   };

   void init(size_t b) {
      reset(); _numBuckets = b; _buckets = new vector<HashNode>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<HashNode>& operator [] (size_t i) { return _buckets[i]; }
   const vector<HashNode>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const {
      size_t now = 0;
      while(now<_numBuckets && _buckets[now].empty()){ now++; }
      if(now == _numBuckets) return iterator(_buckets, _numBuckets, _numBuckets-1, _buckets[_numBuckets-1].end());
      return iterator(_buckets, _numBuckets, now, _buckets[now].begin()); 
   }
   // Pass the end
   iterator end() const { 
      return iterator(_buckets, _numBuckets, _numBuckets-1, _buckets[_numBuckets-1].end()); 
   }
   // return true if no valid data
   bool empty() const { if(begin() == end()) return true; else return false;  }
   // number of valid data
   size_t size() const {
      size_t s = 0;
      // for(iterator i=begin(); i != end(); i++) s++;
      for(size_t i = 0; i < _buckets.size(); i++) s += _buckets[i].size();
      return s; 
   }

   // check if k is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const HashKey& k) const {
      size_t buc = bucketNum(k); 
      for(size_t i = 0; i < _buckets[buc].size(); i++){
        if(_buckets[buc][i].first == k)
          return true;
      }
      return false; 
   }

   // query if k is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(const HashKey& k, HashData& d) const {
      size_t buc = bucketNum(k); 
      for(size_t i = 0; i < _buckets[buc].size(); i++){
        if(_buckets[buc][i].first == k){
          d = _buckets[buc][i].second;
          return true;
        }
      }
      return false; 
   }

   // update the entry in hash that is equal to k (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const HashKey& k, HashData& d) {
      size_t buc = bucketNum(k); 
      for(size_t i = 0; i < _buckets[buc].size(); i++){
        if(_buckets[buc][i].first == k){
          _buckets[buc][i].second = d;
          return true;
        }
      }
      _buckets[buc].push_back(HashNode(k,d));
      return false; 
   }

   // return true if inserted d successfully (i.e. k is not in the hash)
   // return false is k is already in the hash ==> will not insert
   bool insert(const HashKey& k, const HashData& d) {
      size_t buc = bucketNum(k); 
      for(size_t i = 0; i < _buckets[buc].size(); i++){
        if(_buckets[buc][i].first == k){
            return false;
        }
      }
      _buckets[buc].push_back(HashNode(k,d));
      return true; 
   }

   // return true if removed successfully (i.e. k is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const HashKey& k) {
      size_t buc = bucketNum(k); 
      for(size_t i = 0; i < _buckets[buc].size(); i++){
        if(_buckets[buc][i].first == k){
            _buckets[buc][i] = _buckets[buc].back();
            _buckets[buc].pop_back();
            return true;
        }
      }
      return false; 
   }

private:
   // Do not add any extra data member
   size_t                   _numBuckets;
   vector<HashNode>*        _buckets;

   size_t bucketNum(const HashKey& k) const {
      return (k() % _numBuckets); }

};


//---------------------
// Define Cache classes
//---------------------
// To use Cache ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//
// class CacheKey
// {
// public:
//    CacheKey() {}
//    
//    size_t operator() () const { return 0; }
//   
//    bool operator == (const CacheKey&) const { return true; }
//       
// private:
// }; 
// 
template <class CacheKey, class CacheData>
class Cache
{
typedef pair<CacheKey, CacheData> CacheNode;

public:
   Cache() : _size(0), _cache(0) {}
   Cache(size_t s) : _size(0), _cache(0) { init(s); }
   ~Cache() { reset(); }

   // NO NEED to implement Cache::iterator class

   // TODO: implement these functions
   //
   // Initialize _cache with size s
   void init(size_t s) { reset(); _size = s; _cache = new CacheNode[s]; }
   void reset() {  _size = 0; if (_cache) { delete [] _cache; _cache = 0; } }

   size_t size() const { return _size; }

   CacheNode& operator [] (size_t i) { return _cache[i]; }
   const CacheNode& operator [](size_t i) const { return _cache[i]; }

   // return false if cache miss
   bool read(const CacheKey& k, CacheData& d) const {
      size_t i = k() % _size;
      if (k == _cache[i].first) {
         d = _cache[i].second;
         return true;
      }
      return false;
   }
   // If k is already in the Cache, overwrite the CacheData
   void write(const CacheKey& k, const CacheData& d) {
      size_t i = k() % _size;
      _cache[i].first = k;
      _cache[i].second = d;
   }

private:
   // Do not add any extra data member
   size_t         _size;
   CacheNode*     _cache;
};


#endif // MY_HASH_H
