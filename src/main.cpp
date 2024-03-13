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

    bool show_demo_window = false;

    Coeffs coeffs;

    // Declare your infinite update loop.
    ctx.update = [&]() {
        // auto radius_boid = 0.8f;

        // Show a simple window
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            ImGui::Begin("Boids command panel");                    // Create a window
            ImGui::Text("Play with the parameters of the flock !"); // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::SeparatorText("Rules");
            ImGui::SliderFloat("Align", &coeffs.coeff_align, 0.0f, .1f);
            ImGui::SliderFloat("Cohesion", &coeffs.coeff_cohesion, 0.0f, .1f);
            ImGui::SliderFloat("Separate", &coeffs.coeff_separate, 0.0f, .1f);
            ImGui::SliderFloat("Radius of awareness", &coeffs.radius_awareness, 0.0f, 1.f);

            ImGui::End();
        }

        ctx.background(p6::NamedColor::BlueCrayola);
        ctx.use_fill = true;

        ctx.fill = p6::NamedColor::BabyBlue;
        ctx.square(p6::Center{glm::vec2(0., 0.)}, p6::Radius{0.8f});
        for (auto& b : boids)
        {
            b.draw(ctx, coeffs.radius_awareness);
            b.update(&ctx, boids, coeffs);
        }
    };

    // Should be done last. It starts the infinite loop.
    ctx.start();
}