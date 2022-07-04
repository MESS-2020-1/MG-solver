#include <fstream>
#include <chrono>
#include <random>
#include "WL_Data.hh"

using namespace std;

void GreedyWLSolver(const WL_Input& in, WL_Output& out, bool ignore_incompatibilities, mt19937& gen, double equal_tolerance, double amortization_factor)
{ 
  bool found_first;
  unsigned s, w, best_s, best_w, quantity;
  double cost, best_cost, amortized_fixed_cost;

  unsigned equal_bests; // used by the random tie break
  
  out.Reset();
  do
    {
      found_first = false;
      for (s = 0; s < in.Stores(); s++)
        if (out.ResidualAmount(s) > 0)
          {
            for (w = 0; w < in.Warehouses(); w++)
              if (out.ResidualCapacity(w) > 0 && (ignore_incompatibilities || out.Compatible(s,w)))
                {
                  if (out.Load(w) == 0)  
                    amortized_fixed_cost = amortization_factor * (in.FixedCost(w) * 
                                            out.ResidualAmount(s))/static_cast<double>(in.Capacity(w));
                  else
                    amortized_fixed_cost = 0.0;
                  if (!found_first)
                    {
                      found_first = true;
                      best_w = w;
                      best_s = s;
                      best_cost = in.SupplyCost(s,w) + amortized_fixed_cost;
					  equal_bests = 1;
                    }
                  else
                    {
                      cost = in.SupplyCost(s,w) + amortized_fixed_cost;
                      if (cost < best_cost) // if better, becomes new best (without tolerance)
                        {
                          best_w = w;
                          best_s = s;
                          best_cost = cost;
                          equal_bests = 1;
                        }
                      else if (cost < best_cost + equal_tolerance)
                        {
                          equal_bests++;
                          uniform_int_distribution<int> dis(1,equal_bests);
                          if (dis(gen) == 1)
                            {
                              best_w = w;
                              best_s = s;
                              // best_cost is not updated in this case
                            }
                        }
                    }
                }
          }
      if (found_first)
        {          
          quantity = min(out.ResidualAmount(best_s), out.ResidualCapacity(best_w));
          out.Assign(best_s,best_w,quantity);
        }
    }
  while (found_first);
}

int main(int argc, char* argv[])
{
  string instance, solution;
  double tolerance = 0.288, cost_discount = 0.25;
  int timeout, seed;
  unsigned runs = 0;

  if (argc != 5)
    {
      cerr << "Usage: " << argv[0] << " <input_file> <output_file> <timeout> <seed>" << endl;
      exit(1);
    }
  
  instance = argv[1];
  solution = argv[2];
  timeout = atoi(argv[3]);
  seed = atoi(argv[4]);
	  
  WL_Input in(instance);
  WL_Output out(in), best(in);	  

  chrono::time_point<chrono::system_clock> start, end;

  mt19937 gen(seed);
  start = chrono::system_clock::now();
  if (timeout == 0) // make one run
    {
      GreedyWLSolver(in, best, false, gen, tolerance, cost_discount);
      end = chrono::system_clock::now();
    }
  else
    {
      do 
        {      
          GreedyWLSolver(in, out, false, gen, tolerance, cost_discount);
          cerr << "Run " << runs << ", cost " << out.Cost() << endl; 
          end = chrono::system_clock::now();
          if (runs == 0  // the first run is included even if timeout expires
              ||  (chrono::duration_cast<chrono::seconds>(end-start).count() <= timeout 
		           && out.Cost() < best.Cost()))
               best = out;
          runs++;		
        } 
      while (chrono::duration_cast<chrono::seconds>(end-start).count() <= timeout);
    }
  ofstream os(solution);
  os << best << endl;
  cout << "Greedy solver: " << endl;
  cout << "Cost: " << best.Cost() << " (" << best.SupplyCost() << "+" 
       << best.LocationCost() << ")" << endl;
  cout << "Time: " << chrono::duration_cast<chrono::milliseconds>
    (end-start).count()/1000.0 << "s" << endl << endl;      

  return 0;
}



