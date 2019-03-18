#ifndef STABLE_MARRIAGE_H
#define STABLE_MARRIAGE_H

#include "MatchingAlgorithm.h"
#include "TDefs.h"
#include <stack>

class StableMarriage : public MatchingAlgorithm {
private:
    bool A_proposing_; // true if vertices from partition A propose, otherwise false

public:
    StableMarriage(const std::unique_ptr<BipartiteGraph>& G, bool A_proposing=true);
    virtual ~StableMarriage();
    bool compute_matching();
};

#endif
