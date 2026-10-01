//
// Created by honda on 7/4/26.
//

#ifndef ENTROPY_WORDLE_SOLVER_DYNAMICFREQUENCYANALYSIS_HPP
#define ENTROPY_WORDLE_SOLVER_DYNAMICFREQUENCYANALYSIS_HPP
#include "Statistics.hpp"
#include <array>
namespace wordle {
// Refit after every turn to score letters by the candidates still possible.
class DynamicFrequencyAnalysis {
public:
    explicit DynamicFrequencyAnalysis(std::size_t length = 5) : length_(length) { clear(); }
    void fit(const Words& candidates) {
        clear(); total_ = 0;
        for (const auto& raw : candidates) {
            const std::string word = normalize(raw); if (!isWord(word, length_)) continue;
            ++total_; std::array<bool, 26> seen{};
            for (std::size_t i = 0; i < length_; ++i) {
                const auto letter = word[i] - 'a'; ++position_[i][letter];
                if (!seen[letter]) { ++document_[letter]; seen[letter] = true; }
            }
        }
    }
    double score(const std::string& raw, bool rewardRepeatedLetters = false) const {
        const std::string word = normalize(raw); if (!isWord(word, length_) || total_ == 0) return 0.0;
        double result = 0.0; std::array<bool, 26> seen{};
        for (std::size_t i = 0; i < length_; ++i) {
            const auto letter = word[i] - 'a'; result += static_cast<double>(position_[i][letter]) / total_;
            if (rewardRepeatedLetters || !seen[letter]) result += static_cast<double>(document_[letter]) / total_;
            seen[letter] = true;
        }
        return result;
    }
    std::vector<std::pair<std::string, double>> rank(const Words& guesses, bool rewardRepeatedLetters = false) const {
        std::vector<std::pair<std::string, double>> scores; for (const auto& guess : guesses) scores.emplace_back(guess, score(guess, rewardRepeatedLetters)); return sortScores(std::move(scores));
    }
    std::size_t candidateCount() const { return total_; }
private:
    void clear() { position_.assign(length_, {}); document_.fill(0); }
    std::size_t length_, total_ = 0; std::vector<std::array<std::size_t, 26>> position_; std::array<std::size_t, 26> document_{};
};
using FrequencyAnalysis = DynamicFrequencyAnalysis;
} // namespace wordle
#endif
