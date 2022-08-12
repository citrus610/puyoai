#include "search.h"

void Search::search(Field field, std::vector<Pair> queue, SearchResult& result, int max_harass, Heuristic heuristic)
{
    if (queue.size() < 2) {
        return;
    }

    result = SearchResult();

    Node node = Node();
    node.field = field;

    Evaluator evaluator;
    evaluator.heuristic = heuristic;

    TTable ttable = TTable();
    ttable.init();

    avec<Placement, 22> placement;
    Generator::generate(node.field, queue[0], placement);

    for (int i = 0; i < placement.get_size(); ++i) {
        SearchCandidate candidate = {
            .node = node,
            .placement = placement[i],
            .score = SearchScore()
        };
        Chain chain = Chain();
        candidate.node.field.drop_pair(placement[i].x, placement[i].rotation, queue[0]);
        candidate.node.field.pop(chain);

        candidate.score.all_clear = candidate.score.all_clear || (candidate.node.field.popcount() == 0);

        if (chain.count <= max_harass) {
            evaluator.evaluate(candidate.node, node, placement[i], queue[0]);
            candidate.node.score.dect = Detector::detect(candidate.node.field);
            ttable.add_entry(ttable.hash(candidate.node.field), 0);
            result.candidate.add(candidate);
        }
        else {
            candidate.score.chain_score = Field::calculate_point(chain);
            candidate.score.dect = { .chain_count = chain.count, .puyo_add_count = 0 };
            result.candidate_attack.add(candidate);
        }
    }

    if (result.candidate.get_size() > 0) {
        std::sort
        (
            result.candidate.iter_begin(),
            result.candidate.iter_end(),
            [&] (const SearchCandidate& a, const SearchCandidate& b) {
                return b.node < a.node;
            }
        );
    }

    for (int i = 0; i < result.candidate.get_size(); ++i) {
        result.candidate[i].score = Search::nsearch(result.candidate[i].node, queue, evaluator, ttable, max_harass, 1);
    }
};

SearchScore Search::nsearch(Node& node, std::vector<Pair>& queue, Evaluator& evaluator, TTable& ttable, int max_harass, int depth)
{
    SearchScore score = SearchScore();

    avec<Node, 22> children = avec<Node, 22>();
    avec<Placement, 22> placement;
    Generator::generate(node.field, queue[depth], placement);

    for (int i = 0; i < placement.get_size(); ++i) {
        Node child = node;
        Chain chain = Chain();
        child.field.drop_pair(placement[i].x, placement[i].rotation, queue[depth]);
        child.field.pop(chain);

        if (child.field.get_height(2) > 11) {
            continue;
        }

        score.all_clear = score.all_clear || (child.field.popcount() == 0);

        if (chain.count <= max_harass) {
            if (ttable.add_entry(ttable.hash(child.field), depth)) {
                evaluator.evaluate(child, node, placement[i], queue[depth]);
                child.score.dect = Detector::detect(child.field);
                score.dect = std::max(score.dect, child.score.dect);
                
                if (depth + 1 < queue.size()) {
                    children.add(child);
                }
                else {
                    score.eval = std::max(score.eval, child.score.eval.accumulate + child.score.eval.evaluation);
                }
            }
        }
        else {
            score.chain_score = std::max(score.chain_score, Field::calculate_point(chain));
            score.dect = std::max(score.dect, DetectorScore { .chain_count = chain.count, .puyo_add_count = 1 });
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
        SearchScore nscore = Search::nsearch(children[i], queue, evaluator, ttable, max_harass, depth + 1);
        score.dect = std::max(score.dect, nscore.dect);
        score.chain_score = std::max(score.chain_score, nscore.chain_score);
        score.eval = std::max(score.eval, nscore.eval);
        score.all_clear = score.all_clear || nscore.all_clear;
    }

    return score;
};