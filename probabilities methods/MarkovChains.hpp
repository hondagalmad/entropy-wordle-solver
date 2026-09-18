//
// Created by honda on 7/4/26.
//

#ifndef MARKOVCHAINS_HPP
#define MARKOVCHAINS_HPP
#include <bits/stdc++.h>
#include <exception>
class grams {
private:
    int sz;
    vector<int>probs;
    int indexing(std::string s) {
        int multi=1;
        int ans=0;
        for (int i=(int)s.size()-1; i>=0; i--){ans+=(s[i]-'a')*multi;multi*=26;}
        return ans;
    }
public:
    grams() {
        probs.assign(30,0);
        sz=1;
    }
    grams(int sz) {
        this->sz=sz;
        int i=1;
        int ans=i;
        while (i<=sz) {
            ans*=26;
            i++;
        }
        probs.assign(ans+5,0);
    }
    void sum(int idx) {
        if (idx>=probs.size()) throw runtime_error("indexing error");
        probs[idx]+=1;
    }
    int wordsummer(std::string s) {
        int l=0,r=sz-1;
            int multi=1;
            int sums=0;
            for (int i=r;i>=l;i--) {
                sums+=(s[i]-'a')*multi;multi*=26;
            }
            return sums;
    }
    void wordprob(std::string word) {
        int f=0,k=sz-1;
        while (k<word.size()) {
            int sums=wordsummer(word);
            if (sums>=probs.size()) throw runtime_error("wordprob error");
            sum(sums);
            f++,k++;
        }
    }
    int getnum(int idx) {
        if (idx>=probs.size()) throw runtime_error("indexing error");
        return probs[idx];
    }
    int getwordnum(std::string word) {
        int f=wordsummer(word);
        return getnum(f);
    }
};
#endif //ENTROPY_WORDLE_SOLVER_MARKOVCHAINS_HPP
