#include "probabilities methods/Strategies.hpp"
#include <cassert>
#include <iostream>

int main() {
    using namespace wordle;
    assert(feedbackCode("raise", "arise") == 238); // yellow, yellow, green, green, green
    assert(filterCandidates({"cigar", "rebut", "sissy"}, "cigar", feedbackCode("cigar", "cigar")).size() == 1);
    assert(filterCandidates({"allay", "array"}, "allay", feedbackCode("allay", "array")) == Words{"array"});
    Words words{"cigar", "rebut", "sissy", "humph", "awake"};
    for (auto strategy : {Strategy::Entropy, Strategy::Frequency, Strategy::Markov}) {
        GuessSelector selector(strategy, words); const auto guess = selector.choose(words);
        assert(std::find(words.begin(), words.end(), guess) != words.end());
    }
    std::cout << "All tests passed.\n";
}
