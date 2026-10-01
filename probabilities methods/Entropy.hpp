//
// Created by honda on 7/4/26.
//

#ifndef ENTROPY_WORDLE_SOLVER_ENTROPY_HPP
#define ENTROPY_WORDLE_SOLVER_ENTROPY_HPP
#include "Statistics.hpp"
#include <cmath>
#include <unordered_map>
namespace wordle {
class Entropy {
public:
    explicit Entropy(Words answers = {}) : answers_(std::move(answers)) {}
    void setAnswers(Words answers) { answers_ = std::move(answers); }
    const Words& answers() const { return answers_; }
    // Expected information, in bits, under a uniform prior over the candidate answers.
    double score(const std::string& guess) const {
        if (answers_.empty()) return 0.0;
        std::unordered_map<std::uint16_t, std::size_t> buckets;
        for (const auto& answer : answers_) buckets[feedbackCode(guess, answer)]++;
        const double total = static_cast<double>(answers_.size()); double entropy = 0.0;
        for (const auto& bucket : buckets) { const double p = bucket.second / total; entropy -= p * std::log2(p); }
        return entropy;
    }
    std::vector<std::pair<std::string, double>> rank(const Words& guesses) const {
        std::vector<std::pair<std::string, double>> scores; for (const auto& guess : guesses) scores.emplace_back(guess, score(guess)); return sortScores(std::move(scores));
    }
private: Words answers_;
};
} // namespace wordle
#endif
