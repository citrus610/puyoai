#include "ai.h"

namespace AI
{
    Result think_1p(Field field, std::vector<Cell::Pair> queue, u8 harass_bound, u32 score_bound, Weight w)
    {
        u32 field_count = field.get_count();

        // Set maximum skim puyo chain
        u32 harass_score = 0;
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

        // Decide build action
        return decide_build(search_result, field_count);
    };

    Result think_2p(Field field, std::vector<Cell::Pair> queue, u8 harass_bound, u32 score_bound, Data::Enemy enemy, Data::Point point, Weight w)
    {
        u32 field_count = field.get_count();

        // Set maximum skim puyo chain
        u32 harass_score = 0;
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

        // If there is incomming attack
        if (enemy.incomming_attack > 0) {
            std::vector<std::pair<u32, Search::Attack>> candidate_attack;
            std::vector<std::pair<u32, Search::Attack>> candidate_attack_all;
            for (u32 i = 0; i < search_result.candidate.size(); ++i) {
                if (search_result.candidate[i].attack.empty()) {
                    continue;
                }
                for (auto attack : search_result.candidate[i].attack) {
                    if (attack.frame > enemy.incomming_attack_frame) {
                        continue;
                    }
                    if (attack.score + point.bonus_point + point.all_clear * point.target_point * 30 >= enemy.incomming_attack * point.target_point) {
                        candidate_attack.push_back({i, attack});
                    }
                    candidate_attack_all.push_back({i, attack});
                }
            }

            if (candidate_attack.empty()) {
                if ((enemy.incomming_attack_frame <= 4) && (!candidate_attack_all.empty())) {
                    std::sort(
                        candidate_attack_all.begin(),
                        candidate_attack_all.end(),
                        [&] (const std::pair<u32, Search::Attack>& a, const std::pair<u32, Search::Attack>& b) {
                            return a.second.score > b.second.score;
                        }
                    );
                    if (candidate_attack_all[0].second.score + point.bonus_point + point.all_clear * point.target_point * 30 >= enemy.incomming_attack * point.target_point + 30 * point.target_point) {
                        return Result {
                            .placement = search_result.candidate[candidate_attack_all[0].first].placement,
                            .plan = search_result.candidate[candidate_attack_all[0].first].plan,
                            .eval = search_result.candidate[candidate_attack_all[0].first].eval,
                        };
                    }
                }
            }
            else {
                std::sort(
                    candidate_attack.begin(),
                    candidate_attack.end(),
                    [&] (const std::pair<u32, Search::Attack>& a, const std::pair<u32, Search::Attack>& b) {
                        if (a.second.count == b.second.count) {
                            if (a.second.all_clear == b.second.all_clear) {
                                if (a.second.frame == b.second.frame) {
                                    return b.second.score < a.second.score;
                                }
                                return a.second.frame < b.second.frame;
                            }
                            return i32(b.second.all_clear) < i32(a.second.all_clear);
                        }
                        return a.second.count < b.second.count;
                    }
                );
                return Result {
                    .placement = search_result.candidate[candidate_attack[0].first].placement,
                    .plan = search_result.candidate[candidate_attack[0].first].plan,
                    .eval = search_result.candidate[candidate_attack[0].first].eval,
                };
            }
        }

        // If enemy is in danger, attack
        else if (AI::get_enemy_danger(enemy, point, field)) {
            // Get instant attack candidate
            std::vector<Search::Candidate> attack_candidate;
            for (auto candidate : search_result.candidate) {
                if (!candidate.attack.empty() && candidate.attack[0].frame == 0 && candidate.attack[0].count <= 5 && candidate.attack[0].score + point.bonus_point + point.all_clear * point.target_point * 30 >= 12 * point.target_point) {
                    attack_candidate.push_back(candidate);
                }
            }

            // Sort them if they are not empty then trigger the biggest attack with the smallest chain count
            if (!attack_candidate.empty()) {
                std::sort(
                    attack_candidate.begin(),
                    attack_candidate.end(),
                    [&] (const Search::Candidate& a, const Search::Candidate& b) {
                        if (a.attack[0].count == b.attack[0].count) {
                            return a.attack[0].score > b.attack[0].score;
                        }
                        return a.attack[0].count < b.attack[0].count;
                    }  
                );

                return Result
                {
                    .placement = attack_candidate[0].placement,
                    .plan = Field(),
                    .eval = INT32_MIN,
                };
            }
        }

        // If there is enemy all clear, build on the side
        if (enemy.all_clear && field.get_height(3) + field.get_height(4) + field.get_height(5) <= 33) {
            for (i32 i = i32(search_result.candidate.size()) - 1; i >= 0; --i) {
                if (search_result.candidate[i].placement.x < 3 ||
                    (search_result.candidate[i].placement.x == 3 && search_result.candidate[i].placement.direction == Direction::Type::LEFT)) {
                    search_result.candidate.erase(search_result.candidate.begin() + i);
                }
            }
        }

        return AI::decide_build(search_result, field_count);
    };
    
    Result decide_build(Search::Result& search_result, u32 field_count)
    {
        // Go to all clear
        std::vector<std::pair<u32, Search::Attack>> all_clear_attack;
        for (u32 i = 0; i < search_result.candidate.size(); ++i) {
            if (search_result.candidate[i].attack.empty()) {
                continue;
            }
            for (auto attack : search_result.candidate[i].attack) {
                if (attack.count > 4) {
                    continue;
                }
                if (attack.all_clear) {
                    all_clear_attack.push_back({i, attack});
                }
            }
        }
        if (!all_clear_attack.empty()) {
            std::sort(
                all_clear_attack.begin(),
                all_clear_attack.end(),
                [&] (const std::pair<u32, Search::Attack>& a, const std::pair<u32, Search::Attack>& b) {
                    return a.second.score > b.second.score;
                }
            );
            return Result {
                .placement = search_result.candidate[all_clear_attack[0].first].placement,
                .plan = search_result.candidate[all_clear_attack[0].first].plan,
                .eval = search_result.candidate[all_clear_attack[0].first].eval,
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

    bool get_enemy_danger(Data::Enemy& enemy, Data::Point& point, Field& field)
    {
        Detect::Score enemy_attack = Detect::detect(enemy.field);
        bool enemy_no_attack = (enemy_attack.chain <= 1) || (enemy_attack.score + enemy.all_clear * 30 * point.target_point <= 400);

        bool enemy_high = enemy.field.get_height(2) >= 11;

        bool enemy_many_garbage = enemy.field.data[static_cast<u8>(Cell::Type::GARBAGE)].get_count() >= (enemy.field.get_count() / 2);

        FieldBit mask_empty { 0 };
        FieldBit mask_color { 0 };
        for (u8 x = 0; x < 6; ++x) {
            for (u8 p = 0; p < Cell::COUNT; ++p) {
                mask_empty.cols[x] |= enemy.field.data[p].cols[x];
                if (p != static_cast<u8>(Cell::Type::GARBAGE)) {
                    mask_color.cols[x] |= enemy.field.data[p].cols[x];
                }
            }
            mask_empty.cols[x] = ~mask_empty.cols[x];
        }
        FieldBit mask_unobstructed = FieldBit::expand(mask_empty) & mask_color;
        u8 enemy_unobstructed_count = mask_unobstructed.get_count();
        while (true)
        {
            mask_unobstructed = FieldBit::expand(mask_unobstructed) & mask_color;
            if (mask_unobstructed.get_count() == enemy_unobstructed_count) {
                break;
            }
            enemy_unobstructed_count = mask_unobstructed.get_count();
        }
        bool enemy_garbage_obstruct = enemy_unobstructed_count <= (enemy.field.get_count() / 2);

        return
            (enemy_no_attack && enemy.field.get_count() >= 48 && field.get_count() >= 48) ||
            (enemy_no_attack && (field.get_count() > enemy.field.get_count() * 2)) ||
            enemy_many_garbage ||
            enemy_garbage_obstruct ||
            (enemy_high && enemy_attack.score <= 480);
    };
};