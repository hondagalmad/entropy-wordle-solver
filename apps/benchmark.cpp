#include "probabilities methods/Strategies.hpp"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <unordered_set>

namespace {
struct Result { int solved = 0; int totalGuesses = 0; int turns[7]{}; };
Result benchmark(wordle::Strategy strategy, const wordle::Words& all, std::size_t limit) {
    wordle::GuessSelector selector(strategy, all); Result result;
    for (std::size_t targetIndex = 0; targetIndex < limit; ++targetIndex) {
        const std::string& answer = all[targetIndex]; wordle::Words candidates = all; std::unordered_set<std::string> used;
        for (int turn = 1; turn <= 6; ++turn) {
            const auto guess = selector.choose(candidates, used);
            if (guess.empty()) break;
            used.insert(guess); ++result.totalGuesses;
            if (guess == answer) { ++result.solved; ++result.turns[turn]; break; }
            candidates = wordle::filterCandidates(candidates, guess, wordle::feedbackCode(guess, answer));
        }
    }
    return result;
}
void usage() { std::cout << "Usage: wordle-benchmark [--limit N] [entropy|frequency|markov|all] [word-list-path]\n"; }
}
int main(int argc, char** argv) {
    try {
        std::size_t limit = 100; std::string requested = "all", path = "database/valid-wordle-words.txt";
        for (int i = 1; i < argc; ++i) {
            const std::string arg = argv[i];
            if (arg == "--limit" && ++i < argc) limit = std::stoul(argv[i]);
            else if (arg == "entropy" || arg == "frequency" || arg == "markov" || arg == "all") requested = arg;
            else if (!arg.empty() && arg[0] != '-') path = arg;
            else { usage(); return 2; }
        }
        const auto words = wordle::loadWords(path); limit = std::min(limit, words.size());
        std::vector<wordle::Strategy> strategies;
        if (requested == "all") strategies = {wordle::Strategy::Entropy, wordle::Strategy::Frequency, wordle::Strategy::Markov};
        else strategies.push_back(wordle::parseStrategy(requested));
        std::cout << "Benchmarking " << limit << " answers from " << words.size() << " words; max 6 turns.\n";
        for (const auto strategy : strategies) {
            const auto started = std::chrono::steady_clock::now(); const auto result = benchmark(strategy, words, limit);
            const auto elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - started).count();
            std::cout << std::fixed << std::setprecision(2) << wordle::strategyName(strategy) << ": "
                      << (100.0 * result.solved / limit) << "% solved, "
                      << (static_cast<double>(result.totalGuesses) / limit) << " mean guesses, " << elapsed << " s\n  turns:";
            for (int turn = 1; turn <= 6; ++turn) std::cout << ' ' << turn << '=' << result.turns[turn];
            std::cout << '\n';
        }
    } catch (const std::exception& error) { std::cerr << "Error: " << error.what() << '\n'; return 1; }
}
