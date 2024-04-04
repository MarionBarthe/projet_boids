#include <cmath>
#include <iostream>
#include <vector>

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
    double random_num = static_cast<double>(rand()) / RAND_MAX; // Random number between 0 and 1

    return random_num < p; // Return 0 or 1 depending on the success
}

// Simulate binomial distribution by generating a number of success with n, number of trials, and p, probability of success, using multiples Bernoulli disribution
int binomial_distribution(int n, double p)
{
    int successes = 0;
    for (int i = 0; i < n; i++)
    {
        bernoulli_distribution(p) && successes++;
    }
    return successes;
}

// Simulate binomial distribution by generating a number of success with n, number of trials, and p, probability of success, using the Monte Carlo method
int binomial_distribution_monte_carlo(int n, double p)
{
    int    success_probability = 0;
    double random_threshold    = (double)rand() / RAND_MAX; // Random number between 0 and 1

    for (int i = 0; i <= n; i++)
    {
        // Cumulative probability of obtaining i success in the sequence of trials
        success_probability += static_cast<double>(binomial_coefficient(n, i)) * pow(p, i) * pow(1 - p, n - i);

        // Check if the generated threshold is lower than the cumulative probability
        if (random_threshold < success_probability)
        {
            return i;
        }
    }
    return -1;
}

// Simulate exponential distribution by generating a time quotient
int exponential_distribution(double lambda)
{
    double random_exp = -log(static_cast<double>(rand()) / RAND_MAX) / lambda; // Random number between 0 and 1

    double random_scaled = random_exp * 9 + 1;     // Scale the random number to 1 - 10
    return static_cast<int>(round(random_scaled)); // Round and return the result
}

// Simulate Laplace distribution with parameters mu, location and b, scale
double laplace_distribution(double mu, double b)
{
    double u = static_cast<double>(rand()) / RAND_MAX - 0.5; // Random number between -0.5 and 0.5

    if (u < 0)
    {
        // If u is less than 0, use the left side of the Laplace distribution
        return mu + b * log(1 - 2 * u);
    }
    // If u is greater than or equal to 0, use the right side of the Laplace distribution
    return mu - b * log(1 - 2 * u);
}

// TODO(Guilhem): Study to understand better the function
// Simulate normal distribution with parameters average and quarterType
double normalDistribution(double average, double quarterType)
{
    // Ensure u isn't zero to prevent division by zero
    double u = 0.0;
    while (u == 0.0)
    {
        u = static_cast<double>(rand()) / RAND_MAX; // Random number between 0 and 1
    }

    // Ensure v isn't zero to prevent division by zero
    double v = 0.0;
    while (v == 0.0)
    {
        v = static_cast<double>(rand()) / RAND_MAX; // Random number between 0 and 1
    }

    // Box-Muller transform to generate normally distributed variable
    double result = sqrt(-2.0 * log(u)) * cos(2.0 * M_PI * v);

    // Scale and shift the normal variable to match the desired mean and standard deviation
    return result * sqrt(quarterType) + average;
}

// Simulate uniform distribution with parameters lower_bound, the smallest result possible and upper_bound, the largest result possible
double uniformDistribution(double lower_bound, double upper_bound)
{
    // Generate a random number between 0 and RAND_MAX
    double random_num = static_cast<double>(rand()) / RAND_MAX; // Random number between 0 and 1

    // Scale and shift the random number to fit within the specified range
    return lower_bound + (upper_bound - lower_bound) * random_num;
}

// TODO(guilhem): beta_distribution