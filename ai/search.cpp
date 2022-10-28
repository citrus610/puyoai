#include "search.h"

namespace Search
{
    Result search(Field field, std::vector<Cell::Pair> queue, u32 score_bound, Weight w)
    {
        Result result = Result();

        TTable<Score> ttable;
        ttable.init();

        avec<Move::Placement, 22> placement = Move::Generator::generate(field, queue[0].first == queue[0].second);

        for (int i = 0; i < placement.get_size(); ++i) {
            Candidate candidate = 
            {
                .placement = placement[i],
                .plan = Field(),
                .eval = INT32_MIN,
                .eval_im = INT32_MIN,
                .attack = std::vector<Attack>(),
            };

            Field child = field;
            child.drop_pair(placement[i].x, placement[i].direction, queue[0]);
            Chain::Data chain = child.pop();

            if (child.get_height(2) > 11) {
                continue;
            }

            if (chain.links.get_size() > 0) {
                candidate.attack.push_back(
                    Attack
                    {
                        .score = Chain::get_score(chain),
                        .count = u8(chain.links.get_size()),
                        .frame = 0,
                        .all_clear = (child == Field()),
                    }
                );
            }

            if (Chain::get_score(chain) <= score_bound) {
                candidate.eval_im = Evaluator::evaluate(child, std::nullopt, w);
                u8 frame_drop = field.get_drop_pair_frame(placement[i].x, placement[i].direction);
                Score score = dfs(child, queue, score_bound, w, ttable, candidate.attack, 1, frame_drop + chain.links.get_size() * 2);
                if (score.eval > candidate.eval) {
                    candidate.eval = score.eval;
                    candidate.plan = score.plan;
                }
            }

            result.candidate.push_back(candidate);
        }

        return result;
    };

    Score dfs(Field& field, std::vector<Cell::Pair>& queue, u32 score_bound, Weight& w, TTable<Score>& ttable, std::vector<Attack>& attack, u8 depth, u8 frame)
    {
        Score result = Score();

        avec<Move::Placement, 22> placement = Move::Generator::generate(field, queue[depth].first == queue[depth].second);

        for (int i = 0; i < placement.get_size(); ++i) {
            Field child = field;
            child.drop_pair(placement[i].x, placement[i].direction, queue[depth]);
            Chain::Data chain = child.pop();

            if (child.get_height(2) > 11) {
                continue;
            }

            if (chain.links.get_size() > 0) {
                attack.push_back(
                    Attack
                    {
                        .score = Chain::get_score(chain),
                        .count = u8(chain.links.get_size()),
                        .frame = frame,
                        .all_clear = (child == Field()),
                    }
                );
            }

            if (Chain::get_score(chain) <= score_bound) {
                u8 frame_drop = field.get_drop_pair_frame(placement[i].x, placement[i].direction);
                Score score = Score();
                if (!ttable.get_entry(ttable.get_hash(child), score)) {
                    if (depth + 1 < queue.size()) {
                        score = Search::dfs(child, queue, score_bound, w, ttable, attack, depth + 1, frame + frame_drop + chain.links.get_size() * 2);
                    }
                    else {
                        Detect::Score detect = Detect::detect(child);
                        score.eval = Evaluator::evaluate(child, detect, w);
                        score.plan = child;
                        if (detect.chain > 0) {
                            attack.push_back(
                                Attack
                                {
                                    .score = detect.score,
                                    .count = detect.chain,
                                    .frame = u8(frame + frame_drop),
                                    .all_clear = false,
                                }
                            );
                        }
                    }
                    ttable.set_entry(ttable.get_hash(child), score);
                }
                if (score.eval > result.eval) {
                    result = score;
                }
            }
        }

        return result;
    };
};