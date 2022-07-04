// File WL_Data.hh
#ifndef WL_DATA_HH
#define WL_DATA_HH
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

class WL_Input 
{
  friend ostream& operator<<(ostream& os, const WL_Input& in);
public:
  WL_Input(string file_name);
  unsigned Stores() const { return stores; }
  unsigned Warehouses() const { return warehouses; }
  unsigned Capacity(unsigned w) const { return capacity[w]; }
  unsigned FixedCost(unsigned w) const { return fixed_cost[w]; }
  unsigned AmountOfGoods(unsigned s) const { return amount_of_goods[s]; }
  double SupplyCost(unsigned s, unsigned w) const { return supply_cost[s][w]; }
  unsigned Incompatibilities() const { return incompatibilities.size(); }
  pair<unsigned, unsigned> Incompatibility(unsigned i) const { return incompatibilities[i]; }
  unsigned StoreIncompatibilities(unsigned s) const { return incompatibility_list[s].size(); }
  unsigned StoreIncompatibility(unsigned s, unsigned i) const { return incompatibility_list[s][i]; }
  double Occupation();
  
  void ReadDZNFormat(istream& is);
 private:
  unsigned stores, warehouses;
  vector<unsigned> capacity;
  vector<unsigned> fixed_cost;
  vector<unsigned> amount_of_goods;
  vector<vector<double>> supply_cost;
  vector<pair<unsigned, unsigned>> incompatibilities; // list of incompatible pairs of stores
  vector<vector<unsigned>> incompatibility_list;
};

class WL_Output 
{
  friend ostream& operator<<(ostream& os, const WL_Output& out);
  friend istream& operator>>(istream& is, WL_Output& out);
  friend bool operator==(const WL_Output& out1, const WL_Output& out2);
public:
  WL_Output(const WL_Input& i);
  WL_Output& operator=(const WL_Output& out);
  void Reset();
  unsigned Supply(unsigned s, unsigned w) const { return supply[s][w]; }
  unsigned Load(unsigned w) const { return load[w]; }
  unsigned ResidualCapacity(unsigned w) const { return in.Capacity(w) - load[w]; }
  unsigned AssignedGoods(unsigned s) const { return assigned_goods[s]; }
  unsigned ResidualAmount(unsigned s) const { return in.AmountOfGoods(s) - assigned_goods[s]; }
  bool Compatible(unsigned s, unsigned w) const { return compatible[s][w]; } // for greedy
  void FullAssign(unsigned s, unsigned w); // assign completely s to w
  void Assign(unsigned s, unsigned w, unsigned q); // assign q goods of s to w

  double Cost() const { return supply_cost + opening_cost; }
  double SupplyCost() const { return supply_cost; }
  unsigned LocationCost() const { return opening_cost; }

 private:
  const WL_Input& in;
  vector<vector<unsigned>> supply;
  vector<unsigned> assigned_goods;   // quantity of goods of each store already assigned to warehouses
  vector<unsigned> load;   // quantity of goods of each warehouse assigned to stores
  vector<vector<bool>> compatible;  // store x warehouse: compatibility of the assigement of w to s (due to stores assigned to w that are incompatible with s)
  unsigned opening_cost;
  double supply_cost;
};

inline void RemoveElement(vector<unsigned>& v, unsigned e)
{ v.erase(remove(v.begin(), v.end(), e), v.end()); }

inline bool Member(const vector<unsigned>& v, unsigned e)
{ return find(v.begin(), v.end(), e) != v.end(); }
#endif
