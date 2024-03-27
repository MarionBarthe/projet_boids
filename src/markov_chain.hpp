#ifndef MARKOV_CHAIN_HPP
#define MARKOV_CHAIN_HPP

#include <iostream>
#include <vector>

class MarkovChain {
private:
    std::vector<std::vector<double>> transition_matrix;
    std::vector<double>              current_state;
    std::vector<int>                 state_counts;

public:
    MarkovChain(const std::vector<std::vector<double>>& transition_matrix, const std::vector<double>& initial_state);

    void check_probability_sum(const std::vector<double>& probabilities, const std::string& error_message) const;

    void transition_probabilities();

    void collapse_values();

    void transition_values();

    const std::vector<double>& get_current_state() const;

    const std::vector<int>& get_state_counts() const;
};

#endif // MARKOV_CHAIN_HPP