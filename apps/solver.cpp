#include "probabilities methods/Strategies.hpp"
#include <iostream>
#include <sstream>
#include <unordered_set>

namespace {
std::uint16_t parseFeedback(std::string text) {
    if (text.size() != 5) throw std::invalid_argument("feedback must contain exactly five letters: b, y, or g");
    std::uint16_t code = 0, multiplier = 1;
    for (char& c : text) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        const int state = c == 'b' ? 0 : c == 'y' ? 1 : c == 'g' ? 2 : -1;
        if (state < 0) throw std::invalid_argument("feedback may only use b (gray), y (yellow), and g (green)");
        code += state * multiplier; multiplier *= 3;
    }
    return code;
}
void usage() { std::cout << "Usage: wordle-solver [entropy|frequency|markov] [word-list-path]\n"; }
}

int main(int argc, char** argv) {
    try {
        if (argc > 3) { usage(); return 2; }
        const auto strategy = wordle::parseStrategy(argc >= 2 ? argv[1] : "entropy");
        const std::string path = argc == 3 ? argv[2] : "database/valid-wordle-words.txt";
        wordle::Words candidates = wordle::loadWords(path);
        wordle::GuessSelector selector(strategy, candidates);
        std::unordered_set<std::string> used;
        std::cout << "Loaded " << candidates.size() << " words. Strategy: " << wordle::strategyName(strategy)
                  << "\nEnter results as five b/y/g letters (b=gray, y=yellow, g=green). Type quit to stop.\n";
        for (int turn = 1; turn <= 6 && !candidates.empty(); ++turn) {
            const std::string guess = selector.choose(candidates, used);
            if (guess.empty()) break;
            std::cout << "Turn " << turn << " — try: " << guess << "  (" << candidates.size() << " candidates)\n> ";
            std::string feedback;
            if (!(std::cin >> feedback) || feedback == "quit" || feedback == "q") return 0;
            const auto code = parseFeedback(feedback);
            used.insert(guess);
            if (code == 242) { std::cout << "Solved in " << turn << " turn(s).\n"; return 0; }
            candidates = wordle::filterCandidates(candidates, guess, code);
            if (candidates.empty()) std::cout << "No candidates remain; check that the feedback was entered correctly.\n";
        }
        if (!candidates.empty()) std::cout << "No solution within six turns. " << candidates.size() << " candidates remain.\n";
    } catch (const std::exception& error) { std::cerr << "Error: " << error.what() << '\n'; return 1; }
}
