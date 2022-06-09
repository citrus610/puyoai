#pragma once

#include "eval.h"
#include "ttable.h"

namespace LTPuyo
{

struct SearchScore
{
    int attack = 0;
    int chain = 0;
    int eval = INT_MIN;
};

struct SearchCandidate
{
    Node node = Node();
    Placement placement = Placement();
    SearchScore score = SearchScore();
    SearchScore nscore = SearchScore();
};

struct SearchInfo
{
    avec<SearchCandidate, 22> candidate;
    int node = 0;
};

class Search
{
public:
    static void search(Field field, std::vector<std::pair<Puyo, Puyo>> queue, SearchInfo& result, int trigger_point, int max_harass, Heuristic heuristic = DEFAULT_HEURISTIC());
    static SearchScore nsearch(Node& node, std::vector<std::pair<Puyo, Puyo>>& queue, Evaluator& evaluator, TTable& ttable, int depth, int trigger_point, int max_harass, int& node_count);
    static SearchScore qsearch(Node& node, int& node_count);
};

};