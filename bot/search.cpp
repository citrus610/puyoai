#include "search.h"

namespace LTPuyo
{

void Search::search(Field field, std::vector<std::pair<Puyo, Puyo>> queue, SearchInfo& result, int trigger_point, int max_harass, Heuristic heuristic)
{
    if (queue.size() < 2) {
        return;
    }

    result = SearchInfo();
    result.candidate.clear();

    Node node = Node();
    node.field = field;

    Evaluator evaluator;
    evaluator.heuristic = heuristic;

    TTable ttable = TTable();
    ttable.init();

    Puyo pair[2] = { queue[0].first, queue[0].second };
    avec<Placement, 22> placement;
    avec<Node, 22> children = avec<Node, 22>();
    Generator::generate(node.field, queue[0], placement);
    result.node += placement.get_size();

    avec<SearchCandidate, 22> ncandidate;

    for (int i = 0; i < placement.get_size(); ++i) {
        SearchCandidate candidate = { node, placement[i], SearchScore() };

        Chain chain = Chain();
        candidate.node.field.drop_pair(placement[i].x, pair, placement[i].rotation);
        candidate.node.field.pop(chain);

        if (chain.count <= max_harass) {
            evaluator.evaluate(candidate.node, node, placement[i], pair);
            ttable.add_entry(ttable.hash(candidate.node.field), 0);
            ncandidate.add(candidate);
        }
        else {
            candidate.score.attack = Field::calculate_point(chain);
            candidate.score.chain = chain.count;
            result.candidate.add(candidate);
        }
    }

    if (ncandidate.get_size() > 0) {
        std::sort
        (
            ncandidate.iter_begin(),
            ncandidate.iter_end(),
            [&] (const SearchCandidate& a, const SearchCandidate& b) {
                return b.node < a.node;
            }
        );
    }

    for (int i = 0; i < ncandidate.get_size(); ++i) {
        ncandidate[i].nscore = Search::nsearch(ncandidate[i].node, queue, evaluator, ttable, 1, trigger_point, max_harass, result.node);
        result.candidate.add(ncandidate[i]);
    }
};

SearchScore Search::nsearch(Node& node, std::vector<std::pair<Puyo, Puyo>>& queue, Evaluator& evaluator, TTable& ttable, int depth, int trigger_point, int max_harass, int& node_count)
{
    SearchScore score = SearchScore();

    Puyo pair[2] = { queue[depth].first, queue[depth].second };
    avec<Placement, 22> placement;
    avec<Node, 22> children = avec<Node, 22>();
    Generator::generate(node.field, queue[depth], placement);
    node_count += placement.get_size();


    for (int i = 0; i < placement.get_size(); ++i) {
        Node child = node;
        Chain chain = Chain();
        child.field.drop_pair(placement[i].x, pair, placement[i].rotation);
        child.field.pop(chain);

        if (child.field.get_puyo(2, 11) != Puyo::NONE) {
            continue;
        }

        if (chain.count <= max_harass) {
            evaluator.evaluate(child, node, placement[i], pair);
            if (ttable.add_entry(ttable.hash(child.field), depth)) {
                if (depth + 1 < queue.size()) {
                    children.add(child);
                }
                else {
                    // if (6 * 13 - node.field.popcount() > trigger_point) {
                    SearchScore qscore = Search::qsearch(child, node_count);
                    score.attack = std::max(score.attack, qscore.attack);
                    score.chain = std::max(score.chain, qscore.chain);
                    // }
                    score.eval = std::max(score.eval, child.score.accumulate + child.score.evaluation);
                }
            }
        }
        else {
            score.attack = std::max(score.attack, Field::calculate_point(chain));
            score.chain = std::max(score.chain, chain.count);
        }
    }

    if (children.get_size() > 0) {
        std::sort
        (
            children.iter_begin(),
            children.iter_end(),
            [&] (const Node& a, const Node& b) {
                return b < a;
            }
        );
    }

    for (int i = 0; i < children.get_size(); ++i) {
        SearchScore nscore = nsearch(children[i], queue, evaluator, ttable, depth + 1, trigger_point, max_harass, node_count);
        score.attack = std::max(score.attack, nscore.attack);
        score.chain = std::max(score.chain, nscore.chain);
        score.eval = std::max(score.eval, nscore.eval);
    }

    return score;
};

SearchScore Search::qsearch(Node& node, int& node_count)
{
    SearchScore score = SearchScore();

    for (uint8_t p = 0; p < Puyo::COUNT - 1; ++p) {
        avec<Placement, 22> placement;
        Generator::generate(node.field, { Puyo(p), Puyo(p) }, placement);
        node_count += placement.get_size();

        for (int i = 0; i < placement.get_size(); ++i) {
            if (placement[i].rotation != Rotation::RIGHT) {
                continue;
            }

            if (std::abs(node.field.get_height(placement[i].x) - node.field.get_height(placement[i].x + 1)) > 1) {
                continue;
            }

            Field field = node.field;
            Chain chain = Chain();
            field.drop_pair(placement[i].x, { Puyo(p), Puyo(p) }, placement[i].rotation);
            field.pop(chain);

            if (field.get_puyo(2, 11) != Puyo::NONE) {
                continue;
            }

            if (chain.count > 0) {
                score.attack = std::max(score.attack, Field::calculate_point(chain));
                score.chain = std::max(score.chain, chain.count);
            }
        }
    }

    return score;
};

};