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

    while (window.isOpen())
    {
        while (window.pollEvent()) while (const optional event = window.pollEvent()) if (event->is<sf::Event::Closed>()) window.close();

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


        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dist(0, 1);

        std::vector<std::pair<int, int>> newActiveParticles; // Holds newly moved particles

        // Move particles from top to bottom, left to right
        for (int row = rows - 2; row >= 0; --row)
        {
            for (int col = 0; col < cols; ++col)
            {
                if (squareStates[row][col]) // Particle is active
                {
                    bool moved = false;

                    // Try to move the particle down if space is available
                    if (row + 1 < rows && !squareStates[row + 1][col])
                    {
                        squareStates[row + 1][col] = true;
                        squareStates[row][col] = false;
                        newActiveParticles.emplace_back(row + 1, col);
                        moved = true;
                    }
                    // If not, try left-down
                    else if (col > 0 && row + 1 < rows && !squareStates[row + 1][col - 1])
                    {
                        squareStates[row + 1][col - 1] = true;
                        squareStates[row][col] = false;
                        newActiveParticles.emplace_back(row + 1, col - 1);
                        moved = true;
                    }
                    // If not, try right-down
                    else if (col < cols - 1 && row + 1 < rows && !squareStates[row + 1][col + 1])
                    {
                        squareStates[row + 1][col + 1] = true;
                        squareStates[row][col] = false;
                        newActiveParticles.emplace_back(row + 1, col + 1);
                        moved = true;
                    }

                    // If it can't move anywhere, mark it as static
                    if (!moved)
                    {
                        // Mark as processed (static)
                        // These particles stay in place and should be rendered as static (white)
                    }
                }
            }
        }

        // At this point, only add new active particles to activeParticles and keep processed ones out of the loop.
        activeParticles = newActiveParticles;

        // Draw active particles and static ones
        window.clear();

        // Draw active particles as usual
        sf::RectangleShape square(sf::Vector2f(static_cast<float>(squareSize), static_cast<float>(squareSize)));
        square.setOutlineThickness(1.f);
        square.setOutlineColor(sf::Color::White);

        for (auto [row, col] : activeParticles)
        {
            square.setFillColor(sf::Color::White);  // Active particle color
            square.setPosition(sf::Vector2f(static_cast<float>(col * squareSize), static_cast<float>(row * squareSize)));
            window.draw(square);
        }

        // Now render all static particles (those that are stuck) in white
        for (int row = 0; row < rows; ++row)
        {
            for (int col = 0; col < cols; ++col)
            {
                if (squareStates[row][col]) // Static square
                {
                    square.setFillColor(sf::Color::White);  // Static particle color (white)
                    square.setPosition(sf::Vector2f(static_cast<float>(col * squareSize), static_cast<float>(row * squareSize)));
                    window.draw(square);
                }
            }
        }

        window.display();
    }
}
