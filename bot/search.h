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
    std::vector<SearchCandidate> candidate;
    int node = 0;
};

class Search
{
public:
    static void search(Field field, std::vector<std::pair<Puyo, Puyo>> queue, SearchInfo& result);
    static SearchScore nsearch(Node& node, std::vector<std::pair<Puyo, Puyo>>& queue, Evaluator& evaluator, TTable& ttable, int depth, int& node_count);
    static SearchScore qsearch(Node& node, Evaluator& evaluator, TTable& ttable, int depth, int limit, int& node_count);
};

};