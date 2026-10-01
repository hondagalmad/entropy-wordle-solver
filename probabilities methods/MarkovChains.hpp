//
// Created by honda on 7/4/26.
//

#ifndef ENTROPY_WORDLE_SOLVER_MARKOVCHAINS_HPP
#define ENTROPY_WORDLE_SOLVER_MARKOVCHAINS_HPP
#include "Statistics.hpp"
#include <cmath>
#include <map>
namespace wordle {
class MarkovChains {
public:
    explicit MarkovChains(std::size_t order = 2, double smoothing = 0.5) : order_(order), smoothing_(smoothing) {
        if (order_ == 0 || smoothing_ <= 0.0) throw std::invalid_argument("invalid Markov model parameters");
    }
    void fit(const Words& words) {
        transitions_.clear(); totals_.clear();
        for (const auto& raw : words) {
            const std::string word = normalize(raw); if (!isWord(word, word.size())) continue;
            const std::string sequence = std::string(order_ - 1, '^') + word + "$";
            for (std::size_t i = order_ - 1; i < sequence.size(); ++i) {
                const std::string context = sequence.substr(i - (order_ - 1), order_ - 1);
                ++transitions_[context][sequence[i]]; ++totals_[context];
            }
        }
    }
    double logLikelihood(const std::string& raw) const {
        const std::string word = normalize(raw); if (!isWord(word, word.size())) return -INFINITY;
        const std::string sequence = std::string(order_ - 1, '^') + word + "$"; double score = 0.0;
        for (std::size_t i = order_ - 1; i < sequence.size(); ++i) {
            const std::string context = sequence.substr(i - (order_ - 1), order_ - 1);
            const auto total = totals_.count(context) ? totals_.at(context) : 0;
            const auto row = transitions_.find(context);
            const auto count = row != transitions_.end() && row->second.count(sequence[i]) ? row->second.at(sequence[i]) : 0;
            score += std::log((count + smoothing_) / (total + smoothing_ * 27.0));
        }
        return score;
    }
    std::vector<std::pair<std::string, double>> rank(const Words& words) const {
        std::vector<std::pair<std::string, double>> scores; for (const auto& word : words) scores.emplace_back(word, logLikelihood(word)); return sortScores(std::move(scores));
    }
private:
    std::size_t order_; double smoothing_;
    std::map<std::string, std::map<char, std::size_t>> transitions_; std::map<std::string, std::size_t> totals_;
};
} // namespace wordle
using grams = wordle::MarkovChains;
#endif
