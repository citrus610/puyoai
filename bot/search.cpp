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
    result.node += placement.get_size();

    for (int i = 0; i < placement.get_size(); ++i) {
        SearchCandidate candidate = {
            .node = node,
            .placement = placement[i],
            .score = SearchScore()
        };
        Chain chain = Chain();
        candidate.node.field.drop_pair(placement[i].x, placement[i].rotation, queue[0]);
        candidate.node.field.pop(chain);

        if (chain.count <= max_harass) {
            evaluator.evaluate(candidate.node, node, placement[i], queue[0]);
            ttable.add_entry(ttable.hash(candidate.node.field), 0);
            result.candidate.add(candidate);
        }
        else {
            candidate.score.chain_score = Field::calculate_point(chain);
            candidate.score.chain_count = chain.count;
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
        result.candidate[i].score = Search::nsearch(result.candidate[i].node, queue, evaluator, ttable, max_harass, 1, result.node);
    }
};

SearchScore Search::nsearch(Node& node, std::vector<Pair>& queue, Evaluator& evaluator, TTable& ttable, int max_harass, int depth, int& node_count)
{
    SearchScore score = SearchScore();

    avec<Node, 22> children = avec<Node, 22>();
    avec<Placement, 22> placement;
    Generator::generate(node.field, queue[depth], placement);
    node_count += placement.get_size();

    for (int i = 0; i < placement.get_size(); ++i) {
        Node child = node;
        Chain chain = Chain();
        child.field.drop_pair(placement[i].x, placement[i].rotation, queue[depth]);
        child.field.pop(chain);

        if (child.field.get_height(2) > 11) {
            continue;
        }

        if (chain.count <= max_harass) {
            evaluator.evaluate(child, node, placement[i], queue[depth]);
            if (ttable.add_entry(ttable.hash(child.field), depth)) {
                if (depth + 1 < queue.size()) {
                    children.add(child);
                }
                else {
                    SearchScore qscore = Search::qsearch(child, node_count);
                    score.chain_score = std::max(score.chain_score, qscore.chain_score);
                    score.chain_count = std::max(score.chain_count, qscore.chain_count);
                    score.eval = std::max(score.eval, child.score.accumulate + child.score.evaluation);
                }
            }
        }
        else {
            score.chain_score = std::max(score.chain_score, Field::calculate_point(chain));
            score.chain_count = std::max(score.chain_count, chain.count);
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
        SearchScore nscore = Search::nsearch(children[i], queue, evaluator, ttable, max_harass, depth + 1, node_count);
        score.chain_score = std::max(score.chain_score, nscore.chain_score);
        score.chain_count = std::max(score.chain_count, nscore.chain_count);
        score.eval = std::max(score.eval, nscore.eval);
    }

    return score;
};

SearchScore Search::qsearch(Node& node, int& node_count)
{
    SearchScore score = SearchScore();

    avec<Placement, 22> placement;
    Generator::generate(node.field, { Puyo::RED, Puyo::RED }, placement);
    node_count += placement.get_size() * (static_cast<uint8_t>(Puyo::COUNT) - 1);

    for (int i = 0; i < placement.get_size(); ++i) {
        if (placement[i].rotation != Rotation::RIGHT) {
            continue;
        }

        if (std::abs(node.field.get_height(placement[i].x) - node.field.get_height(placement[i].x + 1)) > 1) {
            continue;
        }

        for (uint8_t p = 0; p < static_cast<uint8_t>(Puyo::COUNT) - 1; ++p) {
            Field field = node.field;
            Chain chain = Chain();
            field.drop_pair(placement[i].x, placement[i].rotation, { Puyo(p), Puyo(p) });
            field.pop(chain);

            if (field.get_height(2) > 11) {
                continue;
            }

            if (chain.count > 0) {
                score.chain_score = std::max(score.chain_score, Field::calculate_point(chain));
                score.chain_count = std::max(score.chain_count, chain.count);
            }
        }
    }

    return score;
};