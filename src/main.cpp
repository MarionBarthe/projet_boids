#include <cstdlib>
#define DOCTEST_CONFIG_IMPLEMENT
#include "boid.hpp"
#include "doctest/doctest.h"
#include "p6/p6.h"

int main()
{
    std::vector<Boid> boids(10);

    // Run the tests
    if (doctest::Context{}.run() != 0)
        return EXIT_FAILURE;

    // Actual application code
    auto ctx = p6::Context{{.title = "boids boids boids !!!"}};
    ctx.maximize_window();

    // Declare your infinite update loop.
    ctx.update = [&]() {
        ctx.background(p6::NamedColor::BlueCrayola);
            ctx.use_fill = true;

        ctx.fill = p6::NamedColor::BabyBlue;
        ctx.square(p6::Center{glm::vec2(0., 0.)}, p6::Radius{0.8f});
        for (auto& b : boids)
        {
            b.draw(ctx);
            b.update(&ctx, boids);
        }
    };

    // Should be done last. It starts the infinite loop.
    ctx.start();
}