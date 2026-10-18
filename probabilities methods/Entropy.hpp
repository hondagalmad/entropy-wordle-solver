//
// Created by honda on 7/4/26.
//

#ifndef ENTROPY_WORDLE_SOLVER_ENTROPY_HPP
#define ENTROPY_WORDLE_SOLVER_ENTROPY_HPP
#include "Statistics.hpp"
#include <cmath>
#include <array>
namespace wordle {
class Entropy {
public:
    explicit Entropy(Words answers = {}) : answers_(cleanAnswers(std::move(answers))) {}
    void setAnswers(Words answers) { answers_ = cleanAnswers(std::move(answers)); }
    const Words& answers() const { return answers_; }
    // Expected information, in bits, under a uniform prior over the candidate answers.
    double score(const std::string& guess) const {
        if (answers_.empty()) return 0.0;
        // A five-letter Wordle board has exactly 3^5 possible feedback states.
        // An array avoids allocating and hashing once for every candidate guess.
        const std::string normalizedGuess = normalize(guess);
        if (!isWord(normalizedGuess)) return 0.0;
        std::array<std::size_t, 243> buckets{};
        for (const auto& answer : answers_) ++buckets[fastFeedbackCode(normalizedGuess, answer)];
        const double total = static_cast<double>(answers_.size()); double entropy = 0.0;
        for (const auto bucket : buckets) if (bucket) { const double p = bucket / total; entropy -= p * std::log2(p); }
        return entropy;
    }
    std::vector<std::pair<std::string, double>> rank(const Words& guesses) const {
        std::vector<std::pair<std::string, double>> scores; for (const auto& guess : guesses) scores.emplace_back(guess, score(guess)); return sortScores(std::move(scores));
    }
private: Words answers_;
    static Words cleanAnswers(Words words) {
        for (auto& word : words) word = normalize(std::move(word));
        words.erase(std::remove_if(words.begin(), words.end(), [](const std::string& word) { return !isWord(word); }), words.end());
        return words;
    }
    static std::uint8_t fastFeedbackCode(const std::string& guess, const std::string& answer) {
        std::array<int, 26> remaining{}; std::uint8_t code = 0, multiplier = 1;
        std::array<bool, 5> correct{};
        for (std::size_t i = 0; i < 5; ++i) {
            if (guess[i] == answer[i]) correct[i] = true;
            else ++remaining[answer[i] - 'a'];
        }
        for (std::size_t i = 0; i < 5; ++i) {
            std::uint8_t state = 0;
            if (correct[i]) state = 2;
            else if (remaining[guess[i] - 'a'] > 0) { state = 1; --remaining[guess[i] - 'a']; }
            code += state * multiplier; multiplier *= 3;
        }
        return code;
    }
};
} // namespace wordle
#endif
