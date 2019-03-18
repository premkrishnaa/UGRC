#ifndef ONE_SIDED_REDUCTION_H
#define ONE_SIDED_REDUCTION_H

#include "BipartiteGraph.h"

std::unique_ptr<BipartiteGraph> one_sided_reduce_graph(
    const std::unique_ptr<BipartiteGraph>& G);

void map_back(const std::unique_ptr<BipartiteGraph>& G1, MatchedPairListType& M1,
                    const std::unique_ptr<BipartiteGraph>& G, MatchedPairListType& M);

#endif
