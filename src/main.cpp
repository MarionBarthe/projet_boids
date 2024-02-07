#include <cstdlib>
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest/doctest.h"
#include "p6/p6.h"
#include "boid.hpp"


int main()
{
    std::vector<Boid> boids;
    for (int i = 0; i < 10; i++)
    {
        boids.emplace_back(Boid());
    }

    // Run the tests
    if (doctest::Context{}.run() != 0)
        return EXIT_FAILURE;

    // Actual application code
    auto ctx = p6::Context{{.title = "Simple-p6-Setup"}};
    ctx.maximize_window();

    // Declare your infinite update loop.
    ctx.update = [&]() {
        ctx.background(p6::NamedColor::BlueCrayola);
        for (auto& b : boids)
        {
            b.draw(&ctx);
            b.update(&ctx);
        }
    };

    // Should be done last. It starts the infinite loop.
    ctx.start();
}