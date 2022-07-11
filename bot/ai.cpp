#include "ai.h"

namespace LTPuyo
{

AiResult Ai::think(Field& field, std::vector<Pair>& queue, bool pvp, int incomming_attack, int target_point, int bonus_point, bool pc, bool enemy_danger, Heuristic heuristic)
{
    if (pvp) {
        return Ai::think_pvp
        (
            field,
            queue, 
            incomming_attack,
            target_point,
            bonus_point,
            pc,
            enemy_danger,
            heuristic
        );
    }

    return Ai::think_mono(field, queue, 32, 2, heuristic);
};

AiResult Ai::think_mono(Field& field, std::vector<Pair>& queue, int harass_trigger_point, int max_harass_count, Heuristic heuristic)
{
    int field_popcnt = field.popcount();

    // Set maximum skim puyo chain
    int harass_max = 0;
    if (13 * 6 - field_popcnt <= harass_trigger_point) {
        harass_max = max_harass_count;
    }

    // Search
    SearchInfo sinfo;
    Search::search(field, queue, sinfo, 18, harass_max, heuristic);
    if (sinfo.candidate.get_size() == 0) {
        return { Placement { .x = 2, .rotation = Rotation::UP }, MoveQueue(), 0 };
    }

    // Classify type of candidate
    avec<SearchCandidate, 22> have_chain_now;
    avec<SearchCandidate, 22> have_chain_future;
    avec<SearchCandidate, 22> have_eval_future;

    for (int i = 0; i < sinfo.candidate.get_size(); ++i) {
        if (sinfo.candidate[i].score.chain > harass_max) {
            have_chain_now.add(sinfo.candidate[i]);
            continue;
        }
        if (sinfo.candidate[i].nscore.chain > 0) {
            have_chain_future.add(sinfo.candidate[i]);
            continue;
        }
        have_eval_future.add(sinfo.candidate[i]);
    }

    if (!have_chain_now.get_size() == 0) {
        std::sort
        (
            have_chain_now.iter_begin(),
            have_chain_now.iter_end(),
            [&] (const SearchCandidate& a, const SearchCandidate& b) {
                if (b.score.chain == a.score.chain) {
                    return b.score.attack < a.score.attack;
                }
                return b.score.chain < a.score.chain;
            }
        );
    }

    if (!have_chain_future.get_size() == 0) {
        std::sort
        (
            have_chain_future.iter_begin(),
            have_chain_future.iter_end(),
            [&] (const SearchCandidate& a, const SearchCandidate& b) {
                if (b.nscore.chain == a.nscore.chain) {
                    return b.nscore.eval < a.nscore.eval;
                }
                return b.nscore.chain < a.nscore.chain;
            }
        );
    }

    if (!have_eval_future.get_size() == 0) {
        std::sort
        (
            have_eval_future.iter_begin(),
            have_eval_future.iter_end(),
            [&] (const SearchCandidate& a, const SearchCandidate& b) {
                return b.nscore.eval < a.nscore.eval;
            }
        );
    }

    // Take action: build chain or execute chain
    int max_chain_point = 0;
    if (have_chain_now.get_size() > 0) {
        max_chain_point = std::max(max_chain_point, have_chain_now[0].score.attack);
    }
    if (have_chain_future.get_size() > 0) {
        max_chain_point = std::max(max_chain_point, have_chain_future[0].score.attack);
    }
    if (13 * 6 - field.popcount() <= 18 || max_chain_point >= 52840) {
        // If the biggest chain point > C or the field's space left is < T then trigger the biggest chain possible
        if (!have_chain_future.get_size() == 0) {
            std::sort
            (
                have_chain_future.iter_begin(),
                have_chain_future.iter_end(),
                [&] (const SearchCandidate& a, const SearchCandidate& b) {
                    if (b.nscore.chain == a.nscore.chain) {
                        return b.nscore.attack < a.nscore.attack;
                    }
                    return b.nscore.chain < a.nscore.chain;
                }
            );
        }

        SearchCandidate best_candidate;

        if (!have_eval_future.get_size() == 0) {
            best_candidate = have_eval_future[0];
        }
        if (!have_chain_future.get_size() == 0) {
            best_candidate = have_chain_future[0];
        }
        if (!have_chain_now.get_size() == 0 && have_chain_now[0].score.chain > best_candidate.nscore.chain) {
            best_candidate = have_chain_now[0];
        }

        return { best_candidate.placement, MoveQueue(), sinfo.node };
    }
    else {
        // Else continue building chain
        AiResult result = { Placement(), MoveQueue(), sinfo.node };
        if (!have_eval_future.get_size() == 0) {
            result.placement = have_eval_future[0].placement;
        }
        if (!have_chain_future.get_size() == 0) {
            result.placement = have_chain_future[0].placement;
            return result;
        }
        if (!have_chain_now.get_size() == 0) {
            result.placement = have_chain_now[0].placement;
        }
        return result;
    }
};

AiResult Ai::think_pvp(Field& field, std::vector<Pair>& queue, int incomming_attack, int target_point, int bonus_point, bool pc, bool enemy_danger, Heuristic heuristic)
{
    avec<AiAttackCandidate, 22> attack_candidate = Ai::get_attack_candidate(field, queue[0], target_point, bonus_point, pc);

    if (incomming_attack > 0) {
        // std::cout << "incomming attack: " << incomming_attack << std::endl;

        if (attack_candidate.get_size() > 0) {
            std::sort
            (
                attack_candidate.iter_begin(),
                attack_candidate.iter_end(),
                [&] (const AiAttackCandidate& a, const AiAttackCandidate& b) {
                    return b.chain_attack < a.chain_attack;
                }
            );
        }

        if (attack_candidate.get_size() == 0 || attack_candidate[0].chain_attack < incomming_attack) {
            // std::cout << "think mono!" << std::endl;
            return Ai::think_mono(field, queue, 32, 2, heuristic);
        }

        AiAttackCandidate offset_candidate;
        for (int i = 0; i < attack_candidate.get_size(); ++i) {
            if (attack_candidate[i].chain_attack >= incomming_attack) {
                offset_candidate = attack_candidate[i];
            }
            else {
                break;
            }
        }

        return { .placement = offset_candidate.placement, .move = MoveQueue(), .node = 0 };
    }

    if (enemy_danger && attack_candidate.get_size() > 0) {
        std::sort
        (
            attack_candidate.iter_begin(),
            attack_candidate.iter_end(),
            [&] (const AiAttackCandidate& a, const AiAttackCandidate& b) {
                return b.chain_attack < a.chain_attack;
            }
        );

        const int quick_kill_attack_min = 6;

        if (attack_candidate[0].chain_attack >= quick_kill_attack_min) {
            AiAttackCandidate quick_kill_candidate;
            for (int i = 0; i < attack_candidate.get_size(); ++i) {
                if (attack_candidate[i].chain_attack >= quick_kill_attack_min) {
                    quick_kill_candidate = attack_candidate[i];
                }
                else {
                    break;
                }
            }

            return { .placement = quick_kill_candidate.placement, .move = MoveQueue(), .node = 0 };
        }
    }

    // std::cout << "think mono!" << std::endl;
    return Ai::think_mono(field, queue, 32, 2, heuristic);
};

avec<AiAttackCandidate, 22> Ai::get_attack_candidate(Field& field, Pair pair, int target_point, int bonus_point, bool pc)
{
    avec<AiAttackCandidate, 22> result = avec<AiAttackCandidate, 22>();

    avec<Placement, 22> placement;
    Generator::generate(field, pair, placement);

    for (int i = 0; i < placement.get_size(); ++i) {
        Field field_copy = field;
        Chain chain;
        field_copy.drop_pair(placement[i].x, pair, placement[i].rotation);
        field_copy.pop(chain);

        if (chain.count == 0) {
            continue;
        }

        int chain_point = Field::calculate_point(chain);
        result.add({ .placement = placement[i], .chain_count = chain.count, .chain_attack = ((chain_point + bonus_point) / target_point + pc * 30) });
    }

    return result;
};

bool Ai::get_enemy_danger(Field& field_enemy, std::vector<Pair>& queue_enemy)
{
    bool enemy_has_no_attack = true;

    for (auto pair : queue_enemy) {
        avec<AiAttackCandidate, 22> enemy_attack_candidate = Ai::get_attack_candidate(field_enemy, pair, 70, 0, 0);
        if (enemy_attack_candidate.get_size() > 0) {
            enemy_has_no_attack = false;
            break;
        }
    }

    int field_enemy_popcnt = field_enemy.popcount();

    return enemy_has_no_attack && (13 * 6 - field_enemy_popcnt < 18);
};

};