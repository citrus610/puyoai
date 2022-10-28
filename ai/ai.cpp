#include "ai.h"

namespace AI
{
    Result think_1p(Field& field, std::vector<Cell::Pair>& queue, u8 harass_bound, u8 score_bound, Weight w)
    {
        u32 field_count = field.get_count();

        // Set maximum skim puyo chain
        u8 harass_score = 0;
        if (13 * 6 - field_count <= harass_bound) {
            harass_score = score_bound;
        }

        // Search
        Search::Result search_result = Search::search(field, queue, harass_score, w);
        if (search_result.candidate.empty()) {
            return Result
            {
                .placement = { .x = 2, .direction = Direction::Type::UP },
                .plan = Field(),
                .eval = INT32_MIN,
            };
        }

        // Choose when to trigger main chain
        u32 chain_score_max = 0;
        u32 attack_count = 0;
        for (auto candidate : search_result.candidate) {
            for (auto attack : candidate.attack) {
                chain_score_max = std::max(chain_score_max, attack.score);
            }
            attack_count += candidate.attack.size();
        }
        bool trigger = (13 * 6 - field_count <= 16) || (chain_score_max >= 65536);

        // If not trigger then build chain
        if (!trigger) {
            Search::Candidate best_candidate = *std::max_element(
                search_result.candidate.begin(),
                search_result.candidate.end(),
                [&] (const Search::Candidate& a, const Search::Candidate& b) {
                    if (a.eval == b.eval) {
                        return a.eval_im < b.eval_im;
                    }
                    return a.eval < b.eval;
                }
            );
            if (best_candidate.eval > INT32_MIN) {
                return Result
                {
                    .placement = best_candidate.placement,
                    .plan = best_candidate.plan,
                    .eval = best_candidate.eval,
                };
            }
        }

        // If trigger
        // Or if the trigger condition hasn't been met, but there is no best eval move (since all move trigger a chain), then trigger the biggest chain
        if (attack_count > 0) {
            u32 attack_max = 0;
            Result best_result = Result();
            for (auto candidate : search_result.candidate) {
                if (candidate.attack.empty()) {
                    continue;
                }
                Search::Attack best_attack = *std::max_element(
                    candidate.attack.begin(),
                    candidate.attack.end(),
                    [&] (const Search::Attack& a, const Search::Attack& b) {
                        return a.score < b.score;
                    }
                );
                if (attack_max < best_attack.score) {
                    attack_max = best_attack.score;
                    best_result.placement = candidate.placement;
                    best_result.plan = candidate.plan;
                    best_result.eval = candidate.eval;
                }
            }
            return best_result;
        }

        // Else return null
        return Result
        {
            .placement = { .x = 2, .direction = Direction::Type::UP },
            .plan = Field(),
            .eval = INT32_MIN,
        };
    };
};