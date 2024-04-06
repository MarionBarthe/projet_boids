/*
    - unsigned long long binomial_coefficient(int n, int k)

    - bool bernoulli_distribution(double p)
    - double uniform_distribution(double lower_bound, double upper_bound)
    - int discrete_uniform_distribution(int lower_bound, int upper_bound)

    - int binomial_distribution(int n, double p)
    - int binomial_distribution_monte_carlo(int n, double p)

    - int exponential_distribution(double lambda)
    - double laplace_distribution(double mu, double b)
    - double normal_distribution(double average, double quarterType)

    - double beta_distribution(double alpha, double beta): Simulates a beta distribution with parameters alpha and beta.
*/

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>

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

// Simulate uniform distribution with parameters lower_bound, the smallest result possible and upper_bound, the largest result possible
double uniform_distribution(double lower_bound, double upper_bound)
{
    double random_num = static_cast<double>(rand()) / RAND_MAX; // Random number between 0 and 1

    // Scale and shift the random number to fit within the specified range
    return lower_bound + (upper_bound - lower_bound) * random_num;
}

// Simulate discrete uniform distribution
int discrete_uniform_distribution(int lower_bound, int upper_bound)
{
    int n          = upper_bound - lower_bound + 1; // Number of states
    int random_num = rand() % n;                    // Random number between 0 and n-1

    // Shift the random number to fit within the specified range
    return lower_bound + random_num;
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

// Simulate exponential distribution by generating a time quotient, using inverse transform sampling
int exponential_distribution(double lambda)
{
    double random_exp = -log(static_cast<double>(rand()) / RAND_MAX) / lambda; // Random number between 0 and 1

    double random_scaled = random_exp * 9 + 1;     // Scale the random number to 1 - 10
    return static_cast<int>(round(random_scaled)); // Round and return the result
}

// Simulate Laplace distribution with parameters mu, location and b, scale, , using inverse transform sampling
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

// Simulate normal distribution with parameters average and quarterType, using inverse method transform sampling
double normal_distribution(double average, double quarterType)
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

// Simulate beta distribution with parameters alpha and beta, using inverse transform sampling
double beta_distribution(double alpha, double beta)
{
    double random_num = static_cast<double>(rand()) / RAND_MAX; // Random number between 0 and 1

    // Inverse transform sampling
    double cdf          = 0.0;   // Cumulative distribution function
    double step         = 0.001; // Step size for numerical integration
    double x            = 0.0;
    double beta_distrib = 0.0;
    while (cdf < random_num)
    {
        if (x < 0 || x > 1)
        {
            beta_distrib = 0; // Outside the range
        }
        else
        {
            // Calculation of the probability density function
            beta_distrib = (std::pow(x, alpha - 1) * std::pow(1 - x, beta - 1));
            beta_distrib = beta_distrib / (std::tgamma(alpha) * std::tgamma(beta) / std::tgamma(alpha + beta));
        }
        cdf += beta_distrib * step;
        x += step;
    }
    return x;
}

/*
int main()
{
    // Generate seed based on current time
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Test binomial_coefficient function
    std::cout << "Binomial Coefficient (5 choose 2): " << binomial_coefficient(5, 2) << std::endl;

    // Test bernoulli_distribution function
    double p = 0.5;
    std::cout << "Bernoulli Distribution (p = " << p << "): " << bernoulli_distribution(p) << std::endl;

    // Test uniform_distribution function
    double lower_bound = 0.0;
    double upper_bound = 1.0;
    std::cout << "Uniform Distribution (" << lower_bound << ", " << upper_bound << "): " << uniform_distribution(lower_bound, upper_bound) << std::endl;

    // Test discrete_uniform_distribution function
    int discrete_lower_bound = 0;
    int discrete_upper_bound = 255;
    std::cout << "Discrete Uniform Distribution (" << discrete_lower_bound << ", " << discrete_upper_bound << "): " << discrete_uniform_distribution(discrete_lower_bound, discrete_upper_bound) << std::endl;

    // Test binomial_distribution function
    int n = 10;
    p     = 0.3;
    std::cout << "Binomial Distribution (n = " << n << ", p = " << p << "): " << binomial_distribution(n, p) << std::endl;

    // Test binomial_distribution_monte_carlo function
    std::cout << "Binomial Distribution (Monte Carlo) (n = " << n << ", p = " << p << "): " << binomial_distribution_monte_carlo(n, p) << std::endl;

    // Test exponential_distribution function
    double lambda = 0.5;
    std::cout << "Exponential Distribution (lambda = " << lambda << "): " << exponential_distribution(lambda) << std::endl;

    // Test laplace_distribution function
    double mu = 0.0;
    double b  = 1.0;
    std::cout << "Laplace Distribution (mu = " << mu << ", b = " << b << "): " << laplace_distribution(mu, b) << std::endl;

    // Test normal_distribution function
    double average     = 0.0;
    double quarterType = 1.0;
    std::cout << "Normal Distribution (average = " << average << ", quarterType = " << quarterType << "): " << normal_distribution(average, quarterType) << std::endl;

    // Test beta_distribution function
    double alpha      = 2.0;
    double beta_param = 3.0;
    std::cout << "Beta Distribution (alpha = " << alpha << ", beta = " << beta_param << "): " << beta_distribution(alpha, beta_param) << std::endl;

    return 0;
}
*/