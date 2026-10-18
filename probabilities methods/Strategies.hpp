#ifndef ENTROPY_WORDLE_SOLVER_STRATEGIES_HPP
#define ENTROPY_WORDLE_SOLVER_STRATEGIES_HPP

#include "DynamicFrequencyAnalysis.hpp"
#include "Entropy.hpp"
#include "MarkovChains.hpp"
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>

namespace wordle {
enum class Strategy { Entropy, Frequency, Markov };

inline Strategy parseStrategy(const std::string& name) {
    if (name == "entropy") return Strategy::Entropy;
    if (name == "frequency") return Strategy::Frequency;
    if (name == "markov") return Strategy::Markov;
    throw std::invalid_argument("unknown strategy: " + name);
}
inline const char* strategyName(Strategy strategy) {
    switch (strategy) {
        case Strategy::Entropy: return "entropy";
        case Strategy::Frequency: return "frequency";
        case Strategy::Markov: return "markov";
    }
    return "unknown";
}

// Picks from the remaining answers. Results are memoized because a benchmark
// often reaches the same candidate set for many different hidden answers.
class GuessSelector {
public:
    GuessSelector(Strategy strategy, const Words& dictionary)
        : strategy_(strategy) { markov_.fit(dictionary); }

    std::string choose(const Words& candidates, const std::unordered_set<std::string>& excluded = {}) {
        if (candidates.empty()) return {};
        const std::string key = cacheKey(candidates, excluded);
        const auto cached = cache_.find(key);
        if (cached != cache_.end()) return cached->second;

        std::vector<std::pair<std::string, double>> ranking;
        switch (strategy_) {
            case Strategy::Entropy: ranking = Entropy(candidates).rank(candidates); break;
            case Strategy::Frequency: frequency_.fit(candidates); ranking = frequency_.rank(candidates); break;
            case Strategy::Markov: ranking = markov_.rank(candidates); break;
        }
        for (const auto& entry : ranking) {
            if (!excluded.count(entry.first)) return cache_.emplace(key, entry.first).first->second;
        }
        return {};
    }
private:
    static std::string cacheKey(const Words& candidates, const std::unordered_set<std::string>& excluded) {
        std::string key;
        key.reserve(candidates.size() * 6 + excluded.size() * 6);
        for (const auto& word : candidates) key += word + ',';
        key += '|';
        for (const auto& word : excluded) key += word + ',';
        return key;
    }
    Strategy strategy_;
    DynamicFrequencyAnalysis frequency_;
    MarkovChains markov_;
    std::unordered_map<std::string, std::string> cache_;
};
} // namespace wordle
#endif
