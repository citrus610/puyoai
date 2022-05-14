#include "decision.h"

namespace LTPuyo
{

void Decision::decide(Field field, SearchInfo& search_info, SearchCandidate& result)
{
    if (search_info.candidate.empty()) {
        return;
    }

    std::vector<SearchCandidate> have_chain;
    std::vector<SearchCandidate> have_nchain;
    std::vector<SearchCandidate> have_neval;

    for (auto candidate : search_info.candidate) {
        if (candidate.score.chain > 0) {
            have_chain.push_back(candidate);
            continue;
        }
        if (candidate.nscore.chain > 0) {
            have_nchain.push_back(candidate);
            continue;
        }
        have_neval.push_back(candidate);
    }

    if (!have_chain.empty()) {
        std::sort
        (
            have_chain.begin(),
            have_chain.end(),
            [&] (const SearchCandidate& a, const SearchCandidate& b) {
                // return b.score.attack < a.score.attack;
                if (b.score.chain == a.score.chain) {
                    return b.score.attack < a.score.attack;
                }
                return b.score.chain < a.score.chain;
            }
        );
    }

    if (!have_nchain.empty()) {
        std::sort
        (
            have_nchain.begin(),
            have_nchain.end(),
            [&] (const SearchCandidate& a, const SearchCandidate& b) {
                if (b.nscore.chain == a.nscore.chain) {
                    return b.node.score < a.node.score;
                }
                return b.nscore.chain < a.nscore.chain;
            }
        );
    }

    if (!have_neval.empty()) {
        std::sort
        (
            have_neval.begin(),
            have_neval.end(),
            [&] (const SearchCandidate& a, const SearchCandidate& b) {
                return b.nscore.eval < a.nscore.eval;
            }
        );
    }

    int height[6];
    field.get_height(height);
    int remain_empty = 0;
    for (int i = 0; i < 6; ++i) {
        remain_empty += 13 - height[i];
    }

    if (remain_empty <= TRIGGER_POINT) {
        if (!have_nchain.empty()) {
            std::sort
            (
                have_nchain.begin(),
                have_nchain.end(),
                [&] (const SearchCandidate& a, const SearchCandidate& b) {
                    if (b.nscore.chain == a.nscore.chain) {
                        return b.nscore.attack < a.nscore.attack;
                    }
                    return b.nscore.chain < a.nscore.chain;
                }
            );
        }

        if (!have_neval.empty()) {
            result = have_neval[0];
        }
        if (!have_nchain.empty()) {
            result = have_nchain[0];
        }
        // if (!have_chain.empty() && have_chain[0].score.attack > result.nscore.attack) {
        if (!have_chain.empty() && have_chain[0].score.chain > result.nscore.chain) {
            result = have_chain[0];
        }
    }
    else {
        if (!have_neval.empty()) {
            result = have_neval[0];
        }
        if (!have_nchain.empty()) {
            result = have_nchain[0];
        }
    }
};

};