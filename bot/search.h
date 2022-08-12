
#pragma once

#include "eval.h"
#include "ttable.h"
#include "detect.h"

struct SearchScore
{
    int eval = INT_MIN;
    int chain_score = 0;
    bool all_clear = false;
    DetectorScore dect = DetectorScore();
};

struct SearchCandidate
{
    Node node;
    Placement placement = Placement();
    SearchScore score = SearchScore();
};

struct SearchResult
{
    avec<SearchCandidate, 22> candidate = avec<SearchCandidate, 22>();
    avec<SearchCandidate, 22> candidate_attack = avec<SearchCandidate, 22>();
};

class Search
{
public:
    static void search(Field field, std::vector<Pair> queue, SearchResult& result, int max_harass, Heuristic heuristic = DEFAULT_HEURISTIC());
    static SearchScore nsearch(Node& node, std::vector<Pair>& queue, Evaluator& evaluator, TTable& ttable, int max_harass, int depth);
};