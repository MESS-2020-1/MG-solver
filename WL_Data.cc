// File WL_Data.cc
#include "WL_Data.hh"
#include <fstream>

WL_Input::WL_Input(string file_name)
{  
  ifstream is(file_name);
  if(!is)
  {
    cerr << "Cannot open input file " <<  file_name << endl;
    exit(1);
  }
  if (file_name.find(".dzn") != string::npos)  
	ReadDZNFormat(is);
  else
	throw invalid_argument("Unrecognized file format");
}

void WL_Input::ReadDZNFormat(istream& is)
{
  const unsigned MAX_DIM = 256;
  unsigned w, s, s2;
  char ch, buffer[MAX_DIM];

  is >> buffer >> ch >> warehouses >> ch;
  is >> buffer >> ch >> stores >> ch;
  
  capacity.resize(warehouses);
  fixed_cost.resize(warehouses);
  amount_of_goods.resize(stores);
  supply_cost.resize(stores,vector<double>(warehouses));
  incompatibility_list.resize(stores);
  
  // read capacity
  is.ignore(MAX_DIM,'['); // read "... Capacity = ["
  for (w = 0; w < warehouses; w++)
    is >> capacity[w] >> ch;
  
  // read fixed costs  
  is.ignore(MAX_DIM,'['); // read "... FixedCosts = ["
  for (w = 0; w < warehouses; w++)
    is >> fixed_cost[w] >> ch;

  // read goods
  is.ignore(MAX_DIM,'['); // read "... Goods = ["
  for (s = 0; s < stores; s++)
    is >> amount_of_goods[s] >> ch;

  // read supply costs
  is.ignore(MAX_DIM,'['); // read "... SupplyCost = ["
  is >> ch; // read first '|'
  for (s = 0; s < stores; s++)
  {	 
    for (w = 0; w < warehouses; w++)
      is >> supply_cost[s][w] >> ch;
  }
  is >> ch >> ch;

  // read store incompatibilities
  unsigned num_incompatibilities;
  is >> buffer >> ch >> num_incompatibilities >> ch;
  
  incompatibilities.resize(num_incompatibilities);
  is.ignore(MAX_DIM,'['); // read "... IncompatiblePairs = ["
  for (unsigned i = 0; i < num_incompatibilities; i++)
  {
    is >> ch >> s >> ch >> s2;	  
    incompatibilities[i].first = s - 1;
    incompatibilities[i].second = s2 - 1;
    incompatibility_list[s-1].push_back(s2-1);
    incompatibility_list[s2-1].push_back(s-1);    
  }
  is >> ch >> ch;
}

ostream& operator<<(ostream& os, const WL_Input& in)
{
  unsigned w, s, i;
  os << "Warehouses = " << in.warehouses << ";" << endl;
  os << "Stores = " << in.stores << ";" << endl;
  os << endl;
  
  os << "Capacity = [";
  for (w = 0; w < in.warehouses; w++)
    {
      os << in.capacity[w];
      if (w < in.warehouses - 1)
        os << ", ";
      else
        os << "];" << endl;
    }
  
  os << "FixedCost = [";
  for (w = 0; w < in.warehouses; w++)
    {
      os << in.fixed_cost[w];
      if (w < in.warehouses - 1)
        os << ", ";
      else
        os << "];" << endl;
    }
  
  os << "Goods = [";
  for (s = 0; s < in.stores; s++)
    {
      os << in.amount_of_goods[s];
      if (s < in.stores - 1)
        os << ", ";
      else
        os << "];" << endl;
    }
  
  os << "SupplyCost = [|";
  for (s = 0; s < in.stores; s++)
    {
      for (w = 0; w < in.warehouses; w++)
	  {
         os << static_cast<unsigned>(in.supply_cost[s][w] + 0.5);
         if (w < in.warehouses - 1)
           os << ", ";
         else
           os << "|" << endl;
	  }
    }
  os << "];" << endl << endl;
  
  os << "Incompatibilities = " << in.incompatibilities.size() << ";" << endl;
  os << "[|";
  for (i = 0; i < in.incompatibilities.size(); i++)
	  os << " " << in.incompatibilities[i].first << ", " << in.incompatibilities[i].second << " |";
  os << "]" << endl;
  return os;
}

double WL_Input::Occupation()
{
  unsigned s, w, total_goods = 0, total_capacity = 0;
  for (s = 0; s < stores; s++)
    total_goods += amount_of_goods[s];
  for (w = 0; w < warehouses; w++)
    total_capacity += capacity[w];
  return static_cast<double>(total_goods)/total_capacity;
}

WL_Output::WL_Output(const WL_Input& my_in)
  : in(my_in), supply(in.Stores(),vector<unsigned>(in.Warehouses(),0)), 
    assigned_goods(in.Stores(),0), load(in.Warehouses(),0), 
    compatible(in.Stores(),vector<bool>(in.Warehouses(),true))
{
  supply_cost = 0.0; 
  opening_cost = 0; 
}

WL_Output& WL_Output::operator=(const WL_Output& out)	
{
  supply = out.supply; 
  assigned_goods = out.assigned_goods; 
  load = out.load;
  compatible = out.compatible;
  supply_cost = out.supply_cost; 
  opening_cost = out.opening_cost; 
  return *this;
}

void WL_Output::Reset()
{
  unsigned s, w;
  for (s = 0; s < in.Stores(); s++)
    {
      for (w = 0; w < in.Warehouses(); w++)
        {
          supply[s][w] = 0;
          compatible[s][w] = true;
        }
      assigned_goods[s] = 0;
    }
  for (w = 0; w < in.Warehouses(); w++)
    load[w] = 0;
  supply_cost = 0.0; 
  opening_cost = 0; 
}   

void WL_Output::FullAssign(unsigned s, unsigned w)
{	
  Assign(s,w,in.AmountOfGoods(s) - assigned_goods[s]);
}

void WL_Output::Assign(unsigned s, unsigned w, unsigned q)
{  
  unsigned i, s1;
  supply[s][w] += q;
  assigned_goods[s] += q;
  load[w] += q;
  
  for (i = 0; i < in.StoreIncompatibilities(s); i++)
  {
    s1 = in.StoreIncompatibility(s,i);
    compatible[s1][w] = false;      
  }
  supply_cost += q * in.SupplyCost(s,w); 
  if (load[w] == q)
    opening_cost += in.FixedCost(w); 
}

ostream& operator<<(ostream& os, const WL_Output& out)
{ 
  unsigned s, w;
  const bool extended_format = false;
  if (extended_format)
    {
      os << "[";
      for (s = 0; s < out.in.Stores(); s++)
        {
          os << "(";
          for (w = 0; w < out.in.Warehouses(); w++)
            { 
              os << out.supply[s][w];
              if (w < out.in.Warehouses() - 1)
                os << ",";
              else 
                os << ")";
            }
          if (s < out.in.Stores() - 1)
	    os << endl;
	  else
	    os << "]" << endl;
        }
    }
  os << "{";
  bool first = true;
  for (s = 0; s < out.in.Stores(); s++)
    {
      for (w = 0; w < out.in.Warehouses(); w++)
        { 
          if (out.supply[s][w] > 0)
            {
              if (!first) 
                os << ", ";
              os << "(" << s+1 << ", " << w+1 << ", " << out.supply[s][w] << ")";
              first = false;
            }
        }
    } 
  os << "}";
  return os;
}

istream& operator>>(istream& is, WL_Output& out)
{
  unsigned s, w, q;
  out.Reset();
  char ch;
  is >> ch;
  if (ch == '[')
  {
    for (s = 0; s < out.in.Stores(); s++)
      {
        is >> ch;
        for (w = 0; w < out.in.Warehouses(); w++)
          {
            is >> q >> ch;
            out.Assign(s,w,q);
          }
        is >> ch;
      }
    is >> ch;
  }
  else // compact solution format
  {
    while (is >> ch >> s >> ch >> w >> q >> ch >> ch)
      {
        out.Assign(s-1,w-1,q);
      }
    is >> ch;
  }
 return is;
}

