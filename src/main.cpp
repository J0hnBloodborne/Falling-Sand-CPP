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
    sf::RenderTexture frameBuffer({ static_cast<unsigned int>(windowWidth), static_cast<unsigned int>(windowHeight) });

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(-1, 1); // Random horizontal movement (-1, 0, or 1)

    // Create a square shape for rendering
    sf::RectangleShape square(sf::Vector2f(static_cast<float>(squareSize), static_cast<float>(squareSize)));
       
    // Draw static particles to staticCanvas once
    staticCanvas.clear(sf::Color::Black); // Clear background (could be transparent if needed)
    frameBuffer.clear(sf::Color::Black);
    for (int row = 0; row < rows; ++row)
    {
        for (int col = 0; col < cols; ++col)
        {
            if (squareStates[row][col]) // Static square
            {
                square.setFillColor(sf::Color::White);  // Static particles color
                square.setPosition(sf::Vector2f(static_cast<float>(col * squareSize), static_cast<float>(row * squareSize)));
                staticCanvas.draw(square);
            }
        }
    }
    staticCanvas.display();  // Ensure the staticCanvas is rendered

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
        bool particlesMoved = false;

        // If there are active particles, move them
        if (!activeParticles.empty())
        {
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
                    particlesMoved = true;
                }
                // Try to move left-down
                else if (col > 0 && row + 1 < rows && !squareStates[row + 1][col - 1])
                {
                    squareStates[row + 1][col - 1] = true;
                    squareStates[row][col] = false;
                    newActiveParticles.emplace_back(row + 1, col - 1);
                    moved = true;
                    particlesMoved = true;
                }
                // Try to move right-down
                else if (col < cols - 1 && row + 1 < rows && !squareStates[row + 1][col + 1])
                {
                    squareStates[row + 1][col + 1] = true;
                    squareStates[row][col] = false;
                    newActiveParticles.emplace_back(row + 1, col + 1);
                    moved = true;
                    particlesMoved = true;
                }
                // If particle didn't move, it becomes static
                if (!moved) {
                    // Draw it to the static canvas
                    square.setFillColor(sf::Color::White);
                    square.setPosition(sf::Vector2f(static_cast<float>(col * squareSize),
                        static_cast<float>(row * squareSize)));
                    staticCanvas.draw(square);
                }
            }

            // Update active particles
            activeParticles = newActiveParticles;

            if (particlesMoved) {
                // Update the static canvas to show all static particles
                for (int row = 0; row < rows; ++row) {
                    for (int col = 0; col < cols; ++col) {
                        if (squareStates[row][col]) {
                            bool isActive = false;
                            for (const auto& [activeRow, activeCol] : activeParticles) {
                                if (row == activeRow && col == activeCol) {
                                    isActive = true;
                                    break;
                                }
                            }
                            if (!isActive) {
                                square.setFillColor(sf::Color::White);
                                square.setPosition(sf::Vector2f(static_cast<float>(col * squareSize),
                                    static_cast<float>(row * squareSize)));
                                staticCanvas.draw(square);
                            }
                        }
                    }
                }
                staticCanvas.display();
            }
        }

        // Draw the frame by combining static and active particles
        frameBuffer.clear(sf::Color::Black);

        // Draw the static canvas to the frame buffer
        sf::Sprite staticSprite(staticCanvas.getTexture());
        frameBuffer.draw(staticSprite);

        // Draw active particles to the frame buffer
        square.setFillColor(sf::Color::White);
        for (const auto& [row, col] : activeParticles)
        {
            square.setPosition(sf::Vector2f(static_cast<float>(col * squareSize),
                static_cast<float>(row * squareSize)));
            frameBuffer.draw(square);
        }
        frameBuffer.display();

        // Final draw to window
        window.clear();
        sf::Sprite frameSprite(frameBuffer.getTexture());
        window.draw(frameSprite);
        window.display();
    }
}
