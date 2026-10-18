# entropy-wordle-solver

Header-only C++17 statistical foundations for comparing Wordle guess
strategies. Scoring is separate from game/UI code, so each method can be
benchmarked against the same candidate set.

## Requirements

- CMake 3.16 or newer
- A C++17 compiler (GCC 7+, Clang 5+, Apple Clang 10+, or MSVC 2019+)
- A CMake-supported build tool, such as GNU Make, Ninja, or Visual Studio

There are no third-party libraries or package-manager dependencies. The
included `database/valid-wordle-words.txt` file is the default word list used
by the command-line programs.

On Debian/Ubuntu, install the required tools with:

```sh
sudo apt install build-essential cmake
```

On macOS, install the Xcode Command Line Tools and CMake:

```sh
xcode-select --install
brew install cmake
```

On Windows, install Visual Studio 2019 or later with the **Desktop development
with C++** workload, plus CMake. Open a Developer PowerShell for Visual Studio
in the repository directory before running the CMake commands below.

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

## Terminal applications

From the repository root, configure a Release build, compile it, and run the
tests:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build
```

With Visual Studio's multi-configuration generator, choose the configuration
when building and testing instead:

```powershell
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release
```

Use the interactive solver while playing Wordle. After entering the suggested
word in Wordle, enter its tile pattern with `b` for gray, `y` for yellow, and
`g` for green:

```sh
./build/wordle-solver entropy
# or: ./build/wordle-solver frequency
# or: ./build/wordle-solver markov
```

For example, `byggy` means gray, yellow, green, green, yellow. The solver
filters the answer set after every result and suggests the next candidate.

Benchmark strategies against simulated answers (the default sample is 100
answers; raise `--limit` for a broader measurement):

```sh
./build/wordle-benchmark
./build/wordle-benchmark --limit 1000 frequency
./build/wordle-benchmark --limit 500 entropy
```

Run these commands from the repository root so the default relative path to
`database/valid-wordle-words.txt` resolves correctly. To use another list,
pass its path as the solver's second argument:

```sh
./build/wordle-solver entropy path/to/words.txt
```

The report includes six-turn success rate, mean guesses used (including failed
attempts), turn distribution, and elapsed time. All strategies currently pick
from the remaining answer candidates, so the comparison measures their
ranking heuristics rather than exploratory non-answer guesses.
