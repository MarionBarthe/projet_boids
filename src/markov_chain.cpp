#include "markov_chain.hpp"
#include <cstdlib>
#include <ctime>

MarkovChain::MarkovChain(const std::vector<std::vector<double>>& transition_matrix, const std::vector<double>& initial_state)
    : transition_matrix(transition_matrix), current_state(initial_state), state_counts(initial_state.size() + 1, 0.0)
{
    // Check if the transition matrix is valid
    if (transition_matrix.empty() || transition_matrix.size() != transition_matrix[0].size())
    {
        std::cerr << "Error: Transition matrix must be a non-empty square matrix." << '\n';
        throw std::invalid_argument("Invalid transition matrix.");
    }

    // Check if the transition matrix has the same size as the initial state
    if (transition_matrix.size() != initial_state.size())
    {
        std::cerr << "Error: Transition matrix dimensions do not match initial state size." << '\n';
        throw std::invalid_argument("Invalid initial state.");
    }

    // Check if the sum of the probabilities of each row is equal to 1
    for (size_t i = 0; i < transition_matrix.size(); ++i)
    {
        double row_sum = 0.0;
        for (size_t j = 0; j < transition_matrix[i].size(); ++j)
        {
            if (transition_matrix[i][j] < 0 || transition_matrix[i][j] > 1)
            {
                std::cerr << "Error: Transition probabilities must be in the range [0, 1]." << '\n';
                throw std::invalid_argument("Invalid transition probability.");
            }
            row_sum += transition_matrix[i][j];
        }
        if (std::abs(row_sum - 1.0) > 1e-6)
        {
            std::cerr << "Error: Row " << i << " of transition matrix does not sum up to 1." << '\n';
            throw std::invalid_argument("Invalid transition matrix: rows must sum up to 1.");
        }
    }

    // Check if the sum of the probabilities of each row is equal to 1
    for (size_t i = 0; i < transition_matrix.size(); ++i)
    {
        check_probability_sum(transition_matrix[i], "Row " + std::to_string(i) + " of transition matrix does not sum up to 1.");
    }

    // Check if the sum of the probabilities of the initial state is equal to 1
    check_probability_sum(initial_state, "Sum of initial state probabilities is not equal to 1.");

    std::cout << "Markov Chain Initialized with " << initial_state.size() << " states." << '\n';
}

void MarkovChain::check_probability_sum(const std::vector<double>& probabilities, const std::string& error_message) const
{
    double sum = 0.0;
    for (double prob : probabilities)
    {
        if (prob < 0 || prob > 1)
        {
            std::cerr << "Error: " << error_message << " Probability must be in the range [0, 1]." << '\n';
            throw std::invalid_argument("Invalid probability.");
        }
        sum += prob;
    }
    if (std::abs(sum - 1.0) > 1e-6)
    {
        std::cerr << "Error: " << error_message << " Sum of probabilities is not equal to 1." << '\n';
        throw std::invalid_argument("Invalid probability sum.");
    }
}

void MarkovChain::transition_probabilities()
{
    std::vector<double> next_state(current_state.size(), 0.0);

    // Calculate the next state probabilities
    for (size_t i = 0; i < current_state.size(); ++i)
    {
        for (size_t j = 0; j < next_state.size(); ++j)
        {
            next_state[j] += transition_matrix[i][j] * current_state[i];
        }
    }

    // Log the next state probabilities
    std::cout << "Next state probabilities:";
    for (double prob : next_state)
    {
        std::cout << " " << prob;
    }
    std::cout << '\n';

    current_state = next_state;
}

void MarkovChain::collapse_values()
{
    // Calculate cumulative probabilities for the next state
    std::vector<double> prob_cumul(current_state.size(), 0.0);
    //   double              cumulative_prob = 0.0;
    for (size_t i = 0; i < current_state.size(); ++i)
    {
        // cumulative_prob += next_state[i];
        prob_cumul[i] = prob_cumul[std::max(static_cast<double>(i - 1), 0.0)] + current_state[i];
    }

    // Choose the active state based on the cumulative probabilities
    double rand_num           = static_cast<double>(rand()) / RAND_MAX;
    int    active_state_index = -1;
    for (size_t i = 0; i < prob_cumul.size(); ++i)
    {
        if (rand_num <= prob_cumul[i]) // Comparing with the cumulative probability
        {
            active_state_index = i;
            std::cout << "Active state index: " << active_state_index << '\n';
            break;
        }
    }

    // Update state counts if an active state is chosen
    if (active_state_index != -1)
    {
        state_counts[active_state_index]++;
        state_counts[current_state.size()]++; // Update total count
        std::cout << "State counts updated.\n";
    }
    else
    {
        std::cerr << "Error: No active state selected.\n";
    }

    // Update current state
    std::fill(current_state.begin(), current_state.end(), 0.0);
    current_state[active_state_index] = 1.0;

    // Log the current state
    std::cout << "Current state:";
    for (double state_prob : current_state)
    {
        std::cout << " " << state_prob;
    }
    std::cout << '\n';
    std::cout << "_____________________" << '\n';
    std::cout << '\n';
}

void MarkovChain::transition_values()
{
    transition_probabilities();
    collapse_values();
}

const std::vector<double>& MarkovChain::get_current_state() const
{
    return current_state;
}

const std::vector<int>& MarkovChain::get_state_counts() const
{
    return state_counts;
}

/* int main()
{
    // Example input data
    std::vector<std::vector<double>> transition_matrix = {
        {0.2, 0.3, 0.1, 0.4},
        {0.1, 0.2, 0.3, 0.4},
        {0.4, 0.1, 0.2, 0.3},
        {0.3, 0.2, 0.1, 0.4}
    };
    std::vector<double> initial_state = {0.2, 0.3, 0.1, 0.4};

    // Seed the random number generator
    srand(time(NULL));

    // Create Markov chain object
    MarkovChain chain(transition_matrix, initial_state);

    // Perform transitions
    for (int i = 0; i < 10; ++i)
    {
        chain.transition_values();
        std::cout << "Step " << i + 1 << ": ";
        for (double state_prob : chain.get_current_state())
        {
            std::cout << state_prob << " ";
        }
        std::cout << '\n';
    }

    // Display state counts
    std::cout << "State counts:";
    for (double count : chain.get_state_counts())
    {
        std::cout << " " << count;
    }
    std::cout << '\n';

    return 0;
} */