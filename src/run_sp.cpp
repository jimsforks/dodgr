
#include "dgraph.h"
#include "dijkstra.h"
#include "heap_lib.h"

#include <Rcpp.h>

const float INFINITE_DIST =  std::numeric_limits<float>::max ();

//' rcpp_get_sp
//'
//' @noRd
// [[Rcpp::export]]
Rcpp::NumericMatrix rcpp_get_sp (Rcpp::DataFrame graph, bool quiet)
{
    std::vector <std::string> from = graph ["from_id"];
    std::vector <std::string> to = graph ["to_id"];
    std::vector <float> dist = graph ["d"];
    std::vector <float> wt = graph ["d_weighted"];

    unsigned int nedges = from.size ();
    std::map <std::string, unsigned int> vert_map;
    unsigned int nverts = 0;
    for (unsigned int i = 0; i < nedges; i++)
    {
        if (vert_map.find (from [i]) == vert_map.end ())
            vert_map.emplace (from [i], nverts++);
        if (vert_map.find (to [i]) == vert_map.end ())
            vert_map.emplace (to [i], nverts++);
    }

    // set up graph
    DGraph *g = new DGraph (nverts);
    for (int i = 0; i < nedges; ++i)
    {
        unsigned int fromi = vert_map [from [i]];
        unsigned int toi = vert_map [to [i]];
        g->addNewEdge (fromi, toi, dist [i], wt [i]);
    }

    // Create heap descriptor objects for specifying the kind of heap to be
    // used by Dijkstra's algorithm.  The type of the heap descriptor is
    // specified using the template HeapD<T> where T = BHeap, FHeap, RadixHeap,
    // Heap23, or TriHeap.
    //HeapD<RadixHeap> heapD;
    HeapD<FHeap> heapD;
    //HeapD<Heap23> heapD;
    //HeapD<TriHeap> heapD;
    //HeapD<TriHeapExt> heapD;
    //
    Dijkstra *dijkstra = new Dijkstra (nverts, &heapD);
    float w [nverts], d [nverts];  // result array
    for(unsigned int v = 0; v < nverts; v++)
    {
        w [v] = INFINITE_DIST;
        d [v] = INFINITE_DIST;
    }

    Rcpp::NumericMatrix dout (nverts, nverts);
    for(unsigned int v = 0; v < nverts; v++)
    {
        dijkstra->init (g); // specify the graph
        dijkstra->run (d, w, v);
        for(unsigned int vi = 0; vi < nverts; vi++)
            dout (v, vi) = d [vi];
    }

    delete dijkstra;
    delete g;

    return (dout);
}
