#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <algorithm>
#include <random>
using namespace std;

int main()
{
    int windowWidth = 800;
    int windowHeight = 600;
    int squareSize = 10;
    int rows = windowHeight / squareSize;
    int cols = windowWidth / squareSize;

    sf::RenderWindow window(sf::VideoMode({ static_cast<unsigned int>(windowWidth), static_cast<unsigned int>(windowHeight) }), "Grid of Squares");
    window.setFramerateLimit(60);

    vector<vector<bool>> squareStates(rows, vector<bool>(cols, false));
    vector<pair<int, int>> activeParticles;

    sf::RenderTexture staticCanvas({ static_cast<unsigned int>(windowWidth), static_cast<unsigned int>(windowHeight) });

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(-1, 1); // Random horizontal movement (-1, 0, or 1)

    while (window.isOpen())
    {
        while (window.pollEvent()) while (const optional event = window.pollEvent()) if (event->is<sf::Event::Closed>()) window.close();

        // Input handling: Create a new active particle when mouse is clicked
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            int col = mousePos.x / squareSize;
            int row = mousePos.y / squareSize;

            if (row >= 0 && row < rows && col >= 0 && col < cols && !squareStates[row][col])
            {
                squareStates[row][col] = true;
                activeParticles.emplace_back(row, col);
            }
        }

        vector<pair<int, int>> newActiveParticles; // Holds newly moved particles

        // Move particles and update square states
        for (size_t i = 0; i < activeParticles.size(); ++i)
        {
            auto [row, col] = activeParticles[i];
            bool moved = false;

            // Try to move the particle down
            if (row + 1 < rows && !squareStates[row + 1][col])
            {
                squareStates[row + 1][col] = true;
                squareStates[row][col] = false;
                newActiveParticles.emplace_back(row + 1, col);
                moved = true;
            }
            // Try to move left-down
            else if (col > 0 && row + 1 < rows && !squareStates[row + 1][col - 1])
            {
                squareStates[row + 1][col - 1] = true;
                squareStates[row][col] = false;
                newActiveParticles.emplace_back(row + 1, col - 1);
                moved = true;
            }
            // Try to move right-down
            else if (col < cols - 1 && row + 1 < rows && !squareStates[row + 1][col + 1])
            {
                squareStates[row + 1][col + 1] = true;
                squareStates[row][col] = false;
                newActiveParticles.emplace_back(row + 1, col + 1);
                moved = true;
            }

        }

        // Update active particles by keeping only the newly moved particles
        activeParticles = newActiveParticles;

        // Draw active particles and static ones
        window.clear();

        // Create a square shape for rendering
        sf::RectangleShape square(sf::Vector2f(static_cast<float>(squareSize), static_cast<float>(squareSize)));
        // Draw active particles (in white)
        square.setFillColor(sf::Color::White);
        for (auto [row, col] : activeParticles)
        {
            square.setPosition(sf::Vector2f(static_cast<float>(col * squareSize), static_cast<float>(row * squareSize)));
            window.draw(square);
        }

        // Make sure static particles are set to white too
        for (int row = 0; row < rows; ++row)
        {
            for (int col = 0; col < cols; ++col)
            {
                if (squareStates[row][col]) // Static square
                {
                    square.setFillColor(sf::Color::White);  // Explicitly set static color to white
                    square.setPosition(sf::Vector2f(static_cast<float>(col * squareSize), static_cast<float>(row * squareSize)));
                    window.draw(square);
                }
            }
        }

        // Draw active particles
        for (auto [row, col] : activeParticles)
        {
            square.setFillColor(sf::Color::White);  // Active particle color
            square.setPosition(sf::Vector2f(static_cast<float>(col * squareSize), static_cast<float>(row * squareSize)));
            window.draw(square);
        }

        // Draw static particles (empty particles that are stuck)
        // Here, you can skip rendering static particles entirely if they're not in activeParticles anymore
        // No need for a second loop now that we removed static particles

        window.display();
    }
}
