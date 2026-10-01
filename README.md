# entropy-wordle-solver

Header-only C++17 statistical foundations for comparing Wordle guess
strategies. Scoring is separate from game/UI code, so each method can be
benchmarked against the same candidate set.

- `Statistics.hpp`: word-list loading, normalisation, duplicate-correct Wordle
  feedback, and candidate filtering.
- `DynamicFrequencyAnalysis.hpp`: positional and distinct-letter frequency
  scoring, designed to be refit after each result.
- `MarkovChains.hpp`: additively smoothed character n-gram likelihoods.
- `Entropy.hpp`: exact expected feedback entropy in bits.

```cpp
#include "probabilities methods/Entropy.hpp"
#include "probabilities methods/DynamicFrequencyAnalysis.hpp"

auto words = wordle::loadWords("database/valid-wordle-words.txt");
wordle::Entropy entropy(words);
auto bestEntropy = entropy.rank(words);

wordle::DynamicFrequencyAnalysis frequency;
frequency.fit(words);
auto bestFrequency = frequency.rank(words);

auto feedback = wordle::feedbackCode("raise", "arise");
auto remaining = wordle::filterCandidates(words, "raise", feedback);
frequency.fit(remaining);
```

For entropy, the answer candidates may differ from legal guesses: construct it
with answers and call `rank(allLegalGuesses)`. Frequency rankings combine
positional answer fit with distinct-letter coverage by default.
