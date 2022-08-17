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
        return { Placement { .x = 2, .rotation = Rotation::UP }, MoveQueue() };
    }

    // Classify type of candidate
    if (search_result.candidate.get_size() > 0) {
        std::sort
        (
            search_result.candidate.iter_begin(),
            search_result.candidate.iter_end(),
            [&] (const SearchCandidate& a, const SearchCandidate& b) {
                if (b.score.dect == a.score.dect) {
                    return b.score.eval < a.score.eval;
                }
                return b.score.dect < a.score.dect;
            }
        );
    }

    if (search_result.candidate_attack.get_size() > 0) {
        std::sort
        (
            search_result.candidate_attack.iter_begin(),
            search_result.candidate_attack.iter_end(),
            [&] (const SearchCandidate& a, const SearchCandidate& b) {
                if (b.score.dect.chain_count == a.score.dect.chain_count) {
                    return b.score.chain_score < a.score.chain_score;
                }
                return b.score.dect.chain_count < a.score.dect.chain_count;
            }
        );
    }

    // Go to all clear
#ifdef MAKE_TUNER
#else
    for (int i = 0; i < search_result.candidate_attack.get_size(); ++i) {
        if (search_result.candidate_attack[i].score.all_clear) {
            return { search_result.candidate_attack[i].placement, MoveQueue() };
        }
    }

    for (int i = 0; i < search_result.candidate.get_size(); ++i) {
        if (search_result.candidate[i].score.all_clear) {
            return { search_result.candidate[i].placement, MoveQueue() };
        }
    }
#endif

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
        if (search_result.candidate_attack.get_size() > 0 &&
            (best_candidate.score.dect.chain_count < search_result.candidate_attack[0].score.dect.chain_count ||
            best_candidate.score.dect.chain_count == search_result.candidate_attack[0].score.dect.chain_count)) {
            best_candidate = search_result.candidate_attack[0];
        }

        return { best_candidate.placement, MoveQueue() };
    }

    // Else continue building chain
    if (search_result.candidate.get_size() > 0) {
        return { search_result.candidate[0].placement, MoveQueue() };
    }
    
    return { search_result.candidate_attack[0].placement, MoveQueue() };
};

AiResult Ai::think_pvp(Field& field, std::vector<Pair>& queue, int incomming_attack, int target_point, int bonus_point, bool pc, bool enemy_danger, Heuristic heuristic)
{
    avec<AiAttackCandidate, 22> attack_candidate = Ai::get_attack_candidate(field, queue[0], target_point, bonus_point, pc);

    // If there is incomming attack
    if (incomming_attack > 0) {
        avec<AiAttackCandidate, 22> return_attack_candidate = avec<AiAttackCandidate, 22>();

        for (int i = 0; i < attack_candidate.get_size(); ++i) {
            if (attack_candidate[i].chain_attack >= incomming_attack) {
                return_attack_candidate.add(attack_candidate[i]);
            }
        }

        if (return_attack_candidate.get_size() > 0) {
            std::sort
            (
                return_attack_candidate.iter_begin(),
                return_attack_candidate.iter_end(),
                [&] (const AiAttackCandidate& a, const AiAttackCandidate& b) {
                    if (a.chain_count == b.chain_count) {
                        return b.chain_attack < a.chain_attack;
                    }
                    return a.chain_count < b.chain_count;
                }
            );
        }

        if (return_attack_candidate.get_size() == 0) {
            return Ai::think_mono(field, queue, 18, 2, heuristic);
        }

        return { .placement = return_attack_candidate[0].placement, .move = MoveQueue() };
    }

    if (enemy_danger && attack_candidate.get_size() > 0) {
        avec<AiAttackCandidate, 22> quick_kill_candidate = avec<AiAttackCandidate, 22>();

        for (int i = 0; i < attack_candidate.get_size(); ++i) {
            if (attack_candidate[i].chain_count < 5 && attack_candidate[i].chain_attack >= 12) {
                quick_kill_candidate.add(attack_candidate[i]);
            }
        }

        if (quick_kill_candidate.get_size() > 0) {
            std::sort
            (
                quick_kill_candidate.iter_begin(),
                quick_kill_candidate.iter_end(),
                [&] (const AiAttackCandidate& a, const AiAttackCandidate& b) {
                    if (a.chain_count == b.chain_count) {
                        return b.chain_attack < a.chain_attack;
                    }
                    return a.chain_count < b.chain_count;
                }
            );

            return { .placement = quick_kill_candidate[0].placement, .move = MoveQueue() };
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
    // Enemy has no attack
    bool enemy_has_no_attack = true;

    for (auto pair : queue_enemy) {
        avec<AiAttackCandidate, 22> enemy_attack_candidate = Ai::get_attack_candidate(field_enemy, pair, 70, 0, 0);
        if (enemy_attack_candidate.get_size() > 0) {
            enemy_has_no_attack = false;
            break;
        }
    }

    // Enemy high risk
    bool enemy_high = field_enemy.puyo[static_cast<int>(Puyo::GARBAGE)].popcount() >= (field_enemy.popcount() / 2);

    bool enemy_garbage_obstructed = Ai::get_above_stack_count(field_enemy) <= (field_enemy.popcount() / 2);

    return enemy_has_no_attack && (enemy_high || enemy_garbage_obstructed);
};

int Ai::get_above_stack_count(Field& field)
{
    bool visit[12][6] = { false };
    return Ai::flood_fill_above_stack_count(field, 2, 11, visit);
};

int Ai::flood_fill_above_stack_count(Field& field, int x, int y, bool visit[12][6])
{
    int result = field.get_puyo(x, y) != Puyo::NONE;

    visit[y][x] = true;

    for (int i = 0; i < 4; ++i) {
        int offset_x = ROTATION_OFFSET[i][0];
        int offset_y = ROTATION_OFFSET[i][1];

        if (x + offset_x < 0 ||
            x + offset_x > 5 ||
            y + offset_y < 0 ||
            y + offset_y > 11 ||
            visit[y + offset_y][x + offset_x] ||
            field.get_puyo(x + offset_x, y + offset_y) == Puyo::GARBAGE) {
            continue;
        }

        result += Ai::flood_fill_above_stack_count(field, x + offset_x, y + offset_y, visit);
    }

    return result;
};