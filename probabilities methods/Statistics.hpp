#ifndef ENTROPY_WORDLE_SOLVER_STATISTICS_HPP
#define ENTROPY_WORDLE_SOLVER_STATISTICS_HPP

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

namespace wordle {
using Words = std::vector<std::string>;
enum class LetterState : std::uint8_t { Absent = 0, Present = 1, Correct = 2 };
inline bool isWord(const std::string& word, std::size_t length = 5) {
    return word.size() == length && std::all_of(word.begin(), word.end(), [](unsigned char c) { return c >= 'a' && c <= 'z'; });
}
inline std::string normalize(std::string word) {
    for (char& c : word) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return word;
}
inline Words loadWords(const std::string& path, std::size_t length = 5) {
    std::ifstream input(path);
    if (!input) throw std::runtime_error("unable to open word list: " + path);
    Words words; std::unordered_set<std::string> seen; std::string word;
    while (input >> word) { word = normalize(word); if (isWord(word, length) && seen.insert(word).second) words.push_back(word); }
    return words;
}
// Encodes tile states in base 3; duplicate letters follow the official two-pass Wordle rule.
inline std::uint16_t feedbackCode(const std::string& guessInput, const std::string& answerInput) {
    const std::string guess = normalize(guessInput), answer = normalize(answerInput);
    if (guess.empty() || guess.size() != answer.size() || !isWord(guess, guess.size()) || !isWord(answer, answer.size()))
        throw std::invalid_argument("guess and answer must be equally sized alphabetic words");
    std::array<int, 26> remaining{}; std::vector<LetterState> states(guess.size(), LetterState::Absent);
    for (std::size_t i = 0; i < guess.size(); ++i) {
        if (guess[i] == answer[i]) states[i] = LetterState::Correct;
        else ++remaining[static_cast<unsigned char>(answer[i]) - 'a'];
    }
    for (std::size_t i = 0; i < guess.size(); ++i) {
        const auto letter = static_cast<unsigned char>(guess[i]) - 'a';
        if (states[i] != LetterState::Correct && remaining[letter] > 0) { states[i] = LetterState::Present; --remaining[letter]; }
    }
    std::uint16_t code = 0, multiplier = 1;
    for (LetterState state : states) { code += multiplier * static_cast<std::uint16_t>(state); multiplier *= 3; }
    return code;
}
inline Words filterCandidates(const Words& candidates, const std::string& guess, std::uint16_t observedFeedback) {
    Words filtered;
    for (const auto& candidate : candidates) if (feedbackCode(guess, candidate) == observedFeedback) filtered.push_back(candidate);
    return filtered;
}
inline std::vector<std::pair<std::string, double>> sortScores(std::vector<std::pair<std::string, double>> scores) {
    std::sort(scores.begin(), scores.end(), [](const auto& a, const auto& b) { return a.second != b.second ? a.second > b.second : a.first < b.first; });
    return scores;
}
} // namespace wordle
#endif
