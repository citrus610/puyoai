#include "ai.h"

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
    SearchResult search_result;
    Search::search(field, queue, search_result, harass_max, heuristic);
    if (search_result.candidate.get_size() == 0 && search_result.candidate_attack.get_size() == 0) {
        return { Placement { .x = 2, .rotation = Rotation::UP }, MoveQueue(), 0 };
    }

    // Classify type of candidate
    if (search_result.candidate.get_size() > 0) {
        std::sort
        (
            search_result.candidate.iter_begin(),
            search_result.candidate.iter_end(),
            [&] (const SearchCandidate& a, const SearchCandidate& b) {
                if (b.score.chain_count == a.score.chain_count) {
                    return b.score.eval < a.score.eval;
                }
                return b.score.chain_count < a.score.chain_count;
            }
        );
    }

    if (search_result.candidate_attack.get_size() > 0) {
        std::sort
        (
            search_result.candidate_attack.iter_begin(),
            search_result.candidate_attack.iter_end(),
            [&] (const SearchCandidate& a, const SearchCandidate& b) {
                if (b.score.chain_count == a.score.chain_count) {
                    return b.score.chain_score < a.score.chain_score;
                }
                return b.score.chain_count < a.score.chain_count;
            }
        );
    }

    // Go to all clear
    for (int i = 0; i < search_result.candidate_attack.get_size(); ++i) {
        if (search_result.candidate_attack[i].score.all_clear) {
            return { search_result.candidate_attack[i].placement, MoveQueue(), search_result.node };
        }
    }

    for (int i = 0; i < search_result.candidate.get_size(); ++i) {
        if (search_result.candidate[i].score.all_clear) {
            return { search_result.candidate[i].placement, MoveQueue(), search_result.node };
        }
    }

    // Take action: build chain or execute chain
    int max_chain_point = 0;
    if (search_result.candidate_attack.get_size() > 0) {
        max_chain_point = std::max(max_chain_point, search_result.candidate_attack[0].score.chain_score);
    }
    if (search_result.candidate.get_size() > 0) {
        max_chain_point = std::max(max_chain_point, search_result.candidate[0].score.chain_score);
    }
    if (13 * 6 - field_popcnt <= 16 || max_chain_point >= 56000) {
        // If the biggest chain point > C or the field's space left is < T then trigger the biggest chain possible
        SearchCandidate best_candidate;

        if (search_result.candidate.get_size() > 0) {
            best_candidate = search_result.candidate[0];
        }
        if (search_result.candidate_attack.get_size() > 0 && search_result.candidate_attack[0].score.chain_count > best_candidate.score.chain_count) {
            best_candidate = search_result.candidate_attack[0];
        }

        return { best_candidate.placement, MoveQueue(), search_result.node };
    }

    // Else continue building chain
    if (search_result.candidate.get_size() > 0) {
        return { search_result.candidate[0].placement, MoveQueue(), search_result.node };
    }
    
    return { search_result.candidate_attack[0].placement, MoveQueue(), search_result.node };
};

AiResult Ai::think_pvp(Field& field, std::vector<Pair>& queue, int incomming_attack, int target_point, int bonus_point, bool pc, bool enemy_danger, Heuristic heuristic)
{
    avec<AiAttackCandidate, 22> attack_candidate = Ai::get_attack_candidate(field, queue[0], target_point, bonus_point, pc);

    if (incomming_attack > 0) {
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
            return Ai::think_mono(field, queue, 32, 2, heuristic);
        }

        AiAttackCandidate offset_candidate;
        int min_chain_count = INT_MAX;
        for (int i = 0; i < attack_candidate.get_size(); ++i) {
            if (attack_candidate[i].chain_attack >= incomming_attack) {
                if (attack_candidate[i].chain_count < min_chain_count) {
                    offset_candidate = attack_candidate[i];
                    min_chain_count = attack_candidate[i].chain_count;
                }
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
        field_copy.drop_pair(placement[i].x, placement[i].rotation, pair);
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