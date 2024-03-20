#include <cmath>

// Calculate the binomial coefficient, (n k), representing the number of ways to choose k items from a set of n distinct items
unsigned long long binomial_coefficient(int n, int k)
{
    if (k < 0 || k > n)
    {
        return 0;
    }

    if (k == 0 || k == n)
    {
        return 1;
    }

    if (k == 1 || k == n - 1)
    {
        return n;
    }

    // Formula n!/((n-k)!k!) as a loop
    unsigned long long coeff = n;
    for (unsigned int i = 2; i <= k; i++)
    {
        coeff *= (n - i + 1);
        coeff /= i;
    }

    return coeff;
}

// Simulate bernoulli distribution with p, probability of success
bool bernoulli_distribution(double p)
{
    double random_num = (double)rand() / RAND_MAX; // Random number between 0 and 1

    return random_num < p; // Return 0 or 1 depending on the success
}

// Simulate binomial distribution by generating a number of success with n, number of trials, and p, probability of success
int binomial_distribution(int n, double p)
{
    int    success_probability = 0;
    double random_threshold    = (double)rand() / RAND_MAX; // Random number between 0 and 1

    for (int i = 0; i <= n; i++)
    {
        // Cumulative probability of obtaining i success in the sequence of trials
        success_probability += (double)binomial_coefficient(n, i) * pow(p, i) * pow(1 - p, n - i);

        // Check if the generated threshold is lower than the cumulative probability
        if (random_threshold < success_probability)
        {
            return i;
        }
    }
    return -1;
}