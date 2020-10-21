/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ==" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashSet<Data>;

   public:
      iterator(vector<Data>* n, size_t num, size_t r, typename vector<Data>::iterator itr): 
               _buckets(n), _numBuckets(num), _row(r), itr(itr) {}
      iterator(const iterator& i): 
               _buckets(i._buckets), _numBuckets(i._numBuckets), _row(i._row), itr(i.itr) {}
      ~iterator() {} 

      const Data& operator * () const { return *itr; }
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
      vector<Data>*     _buckets;
      size_t            _numBuckets;
      size_t            _row;
      typename vector<Data>::iterator itr;
      
   };

   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

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
   bool empty() const { if(begin() == end()) return true; else return false; }
   // number of valid data
   size_t size() const {
      size_t s = 0;
      for(iterator i=begin(); i != end(); i++) s++;
      return s; 
   }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const {
      size_t buc = bucketNum(d); 
      for(size_t i = 0; i < _buckets[buc].size(); i++){
        if(_buckets[buc][i] == d)
          return true;
      }
      return false; 
   }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const {
      size_t buc = bucketNum(d); 
      for(size_t i = 0; i < _buckets[buc].size(); i++){
        if(_buckets[buc][i] == d){
          d = _buckets[buc][i];
          return true;
        }
      }
      return false; 
   }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) {
      size_t buc = bucketNum(d); 
      for(size_t i = 0; i < _buckets[buc].size(); i++){
        if(_buckets[buc][i] == d){
          _buckets[buc][i] = d;
          return true;
        }
      }
      _buckets[buc].push_back(d);
      return false; 
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) {
      size_t buc = bucketNum(d); 
      for(size_t i = 0; i < _buckets[buc].size(); i++){
        if(_buckets[buc][i] == d){
            return false;
        }
      }
      _buckets[buc].push_back(d);
      return true; 
   }

   // return true if removed successfully (i.e. d is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const Data& d) { 
      size_t buc = bucketNum(d); 
      for(size_t i = 0; i < _buckets[buc].size(); i++){
        if(_buckets[buc][i] == d){
            _buckets[buc][i] = _buckets[buc].back();
            _buckets[buc].pop_back();
            return true;
        }
      }
      return false; 
   }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
