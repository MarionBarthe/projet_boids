#include <cstdlib>
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest/doctest.h"
#include "p6/p6.h"

struct Boid {
    glm::vec2 position;
    glm::vec2 velocity;
    float radius = 0.05;

    Boid()
        : position(p6::random::point()), velocity(p6::random::point(glm::vec2(0., 0.), glm::vec2(0.01, 0.01))){};
};

void updateBoid(p6::Context* ctx, Boid* b)
{
    b->position += b->velocity;
    if (b->position.x < -ctx->aspect_ratio())
        b->position.x = ctx->aspect_ratio();
    if (b->position.y < -1)
        b->position.y = 1;
    if (b->position.x > ctx->aspect_ratio())
        b->position.x = -ctx->aspect_ratio();
    if (b->position.y > 1)
        b->position.y = -1;
};

void drawBoid(p6::Context* ctx, Boid* b)
{
    ctx->fill = {1.f, 0.7f, 0.2f};
    ctx->circle(p6::Center{b->position}, p6::Radius{b->radius});
}

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
        ctx.background(p6::NamedColor::Blue);
        for (auto& b : boids)
        {
            drawBoid(&ctx, &b);
            updateBoid(&ctx, &b);
        }
    };

    // Should be done last. It starts the infinite loop.
    ctx.start();
}