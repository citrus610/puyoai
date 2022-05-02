#include "search.h"

namespace LTPuyo
{

void Search::search(Field field, std::vector<std::pair<Puyo, Puyo>> queue, SearchInfo& result)
{
    if (queue.size() < 2) {
        return;
    }

    result = SearchInfo();
    result.candidate.reserve(22);

    Node node = Node();
    node.field = field;

    Evaluator evaluator;
    evaluator.heuristic = DEFAULT_HEURISTIC();

    TTable ttable = TTable();
    ttable.init();

    Placement placement[22];
    int placement_count = 0;
    generate(node.field, placement, placement_count);
    Puyo pair[2] = { queue[0].first, queue[0].second };
    result.node += placement_count;

    std::vector<SearchCandidate> ncandidate;
    ncandidate.reserve(22);

    for (int i = 0; i < placement_count; ++i) {
        SearchCandidate candidate = { node, placement[i], SearchScore() };

        Chain chain = Chain();
        candidate.node.field.drop_pair(placement[i].x, pair, placement[i].rotation);
        candidate.node.field.pop(chain);

        if (chain.count == 0) {
            evaluator.evaluate(candidate.node, node, placement[i], pair);
            ttable.add_entry(ttable.hash(candidate.node.field), 0);
            ncandidate.push_back(candidate);
        }
        else {
            candidate.score.attack = Field::calculate_point(chain);
            candidate.score.chain = chain.count;
            result.candidate.push_back(candidate);
        }
    }

    if (!ncandidate.empty()) {
        std::sort
        (
            ncandidate.begin(),
            ncandidate.end(),
            [&] (const SearchCandidate& a, const SearchCandidate& b) {
                return b.node < a.node;
            }
        );
    }

    for (auto c : ncandidate) {
        c.nscore = Search::nsearch(c.node, queue, evaluator, ttable, 1, result.node);
        result.candidate.push_back(c);
    }
};

SearchScore Search::nsearch(Node& node, std::vector<std::pair<Puyo, Puyo>>& queue, Evaluator& evaluator, TTable& ttable, int depth, int& node_count)
{
    SearchScore score = SearchScore();

    Puyo pair[2] = { queue[depth].first, queue[depth].second };
    Placement placement[22];
    int placement_count = 0;
    generate(node.field, placement, placement_count);
    node_count += placement_count;

    Node children[22];
    int children_count = 0;

    for (int i = 0; i < placement_count; ++i) {
        Node child = node;
        Chain chain = Chain();
        child.field.drop_pair(placement[i].x, pair, placement[i].rotation);
        child.field.pop(chain);

        if (chain.count == 0) {
            evaluator.evaluate(child, node, placement[i], pair);
            if (ttable.add_entry(ttable.hash(child.field), depth)) {
                if (depth + 1 < queue.size()) {
                    children[children_count] = child;
                    ++children_count;
                }
                else {
                    if (6 * 13 - node.field.popcount() > 16) {
                        SearchScore qscore = Search::qsearch(child, evaluator, ttable, 0, 2, node_count);
                        score.attack = std::max(score.attack, qscore.attack);
                        score.chain = std::max(score.chain, qscore.chain);
                        score.eval = std::max(score.eval, qscore.eval);
                    }
                    score.eval = std::max(score.eval, child.score.accumulate + child.score.evaluation);
                }
            }
        }
        else {
            score.attack = std::max(score.attack, Field::calculate_point(chain));
            score.chain = std::max(score.chain, chain.count);
        }
    }

    if (children_count > 0) {
        std::sort
        (
            children,
            children + children_count,
            [&] (const Node& a, const Node& b) {
                return b < a;
            }
        );
    }

    for (int i = 0; i < children_count; ++i) {
        SearchScore nscore = nsearch(children[i], queue, evaluator, ttable, depth + 1, node_count);
        score.attack = std::max(score.attack, nscore.attack);
        score.chain = std::max(score.chain, nscore.chain);
        score.eval = std::max(score.eval, nscore.eval);
    }

    return score;
};

SearchScore Search::qsearch(Node& node, Evaluator& evaluator, TTable& ttable, int depth, int limit, int& node_count)
{
    SearchScore score = SearchScore();

    int height[6];
    node.field.get_height(height);

    for (uint8_t x = 0; x < 6; ++x) {
        if (height[x] >= 12) {
            continue;
        }

        for (uint8_t p = 0; p < Puyo::COUNT - 1; ++p) {
            Puyo pair[2] = { Puyo(p), Puyo(p) };

            Node child = node;
            Chain chain = Chain();
            child.field.drop_pair(x, pair, Rotation::UP);
            child.field.pop(chain);

            if (chain.count > 0) {
                score.attack = std::max(score.attack, Field::calculate_point(chain));
                score.chain = std::max(score.chain, chain.count);
            }
        }

        node_count += Puyo::COUNT - 1;
    }

    return score;
};

};