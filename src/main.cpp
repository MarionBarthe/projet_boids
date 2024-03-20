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

        // Show a simple window
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            ImGui::Begin("Boids command panel");                    
            ImGui::Text("Play with the parameters of the flock !"); 
            ImGui::Checkbox("Demo Window", &show_demo_window);      
            ImGui::SeparatorText("Rules");

            coeffs.draw_Gui();

            ImGui::End();
        }

        ctx.background(p6::NamedColor::BlueCrayola);
        ctx.use_fill = true;

        ctx.fill = p6::NamedColor::BabyBlue;
        ctx.square(p6::Center{glm::vec2(0., 0.)}, p6::Radius{coeffs.square_length});
        for (auto& b : boids)
        {
            b.draw(ctx, coeffs.radius_awareness);
            b.update(&ctx, boids, coeffs);
        }
    };

    // Should be done last. It starts the infinite loop.
    ctx.start();
}