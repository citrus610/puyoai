#include "decision.h"

namespace LTPuyo
{

void Decision::decide(Field field, SearchInfo& search_info, SearchCandidate& result)
{
    if (search_info.candidate.get_size() == 0) {
        return;
    }

    avec<SearchCandidate, 22> have_chain;
    avec<SearchCandidate, 22> have_nchain;
    avec<SearchCandidate, 22> have_neval;

    Decision::classify_candidate(search_info, have_chain, have_nchain, have_neval);

    int max_chain_point = 0;
    if (have_chain.get_size() > 0) {
        max_chain_point = std::max(max_chain_point, have_chain[0].score.attack);
    }
    if (have_nchain.get_size() > 0) {
        max_chain_point = std::max(max_chain_point, have_nchain[0].score.attack);
    }

    if (13 * 6 - field.popcount() <= 18 || max_chain_point >= 56000) {
        Decision::action_execute_biggest_chain(have_chain, have_nchain, have_neval, result);
        return;
    }
    else {
        Decision::action_build(have_chain, have_nchain, have_neval, result);
        return;
    }
};

void Decision::classify_candidate(SearchInfo& search_info, avec<SearchCandidate, 22>& have_chain, avec<SearchCandidate, 22>& have_nchain, avec<SearchCandidate, 22>& have_neval)
{
    for (int i = 0; i < search_info.candidate.get_size(); ++i) {
        if (search_info.candidate[i].score.chain > 0) {
            have_chain.add(search_info.candidate[i]);
            continue;
        }
        if (search_info.candidate[i].nscore.chain > 0) {
            have_nchain.add(search_info.candidate[i]);
            continue;
        }
        have_neval.add(search_info.candidate[i]);
    }

    if (!have_chain.get_size() == 0) {
        std::sort
        (
            have_chain.iter_begin(),
            have_chain.iter_end(),
            [&] (const SearchCandidate& a, const SearchCandidate& b) {
                if (b.score.chain == a.score.chain) {
                    return b.score.attack < a.score.attack;
                }
                return b.score.chain < a.score.chain;
            }
        );
    }

    if (!have_nchain.get_size() == 0) {
        std::sort
        (
            have_nchain.iter_begin(),
            have_nchain.iter_end(),
            [&] (const SearchCandidate& a, const SearchCandidate& b) {
                if (b.nscore.chain == a.nscore.chain) {
                    return b.nscore.eval < a.nscore.eval;
                }
                return b.nscore.chain < a.nscore.chain;
            }
        );
    }

    if (!have_neval.get_size() == 0) {
        std::sort
        (
            have_neval.iter_begin(),
            have_neval.iter_end(),
            [&] (const SearchCandidate& a, const SearchCandidate& b) {
                return b.nscore.eval < a.nscore.eval;
            }
        );
    }
};

void Decision::action_build(avec<SearchCandidate, 22>& have_chain, avec<SearchCandidate, 22>& have_nchain, avec<SearchCandidate, 22>& have_neval, SearchCandidate& result)
{
    if (!have_neval.get_size() == 0) {
        result = have_neval[0];
    }
    if (!have_nchain.get_size() == 0) {
        result = have_nchain[0];
        return;
    }
    if (!have_chain.get_size() == 0) {
        result = have_chain[0];
    }
};

void Decision::action_execute_biggest_chain(avec<SearchCandidate, 22>& have_chain, avec<SearchCandidate, 22>& have_nchain, avec<SearchCandidate, 22>& have_neval, SearchCandidate& result)
{
    if (!have_nchain.get_size() == 0) {
        std::sort
        (
            have_nchain.iter_begin(),
            have_nchain.iter_end(),
            [&] (const SearchCandidate& a, const SearchCandidate& b) {
                if (b.nscore.chain == a.nscore.chain) {
                    return b.nscore.attack < a.nscore.attack;
                }
                return b.nscore.chain < a.nscore.chain;
            }
        );
    }

    if (!have_neval.get_size() == 0) {
        result = have_neval[0];
    }
    if (!have_nchain.get_size() == 0) {
        result = have_nchain[0];
    }
    if (!have_chain.get_size() == 0 && have_chain[0].score.chain > result.nscore.chain) {
        result = have_chain[0];
    }
};

};