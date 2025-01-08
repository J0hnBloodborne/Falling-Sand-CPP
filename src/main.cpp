#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <algorithm>
#include <random>
using namespace std;

sf::Color getHueColor(int particleCount)
{
    int colorShift = (particleCount / 10) % 5;

    int red = 194 + colorShift * 10;
    int green = 178 + colorShift * 5;
    int blue = 128 + colorShift * 10;

    red = std::min(255, std::max(0, red));
    green = std::min(255, std::max(0, green));
    blue = std::min(255, std::max(0, blue));

    return sf::Color(red, green, blue);
}

void resetGrid(vector<vector<bool>>& squareStates, vector<pair<int, int>>& activeParticles, int rows, int cols)
{
    squareStates = vector<vector<bool>>(rows, vector<bool>(cols, false));
    activeParticles.clear();
}

int main()
{
    int windowWidth = 800;
    int windowHeight = 600;
    int squareSize = 5;
    int rows = windowHeight / squareSize;
    int cols = windowWidth / squareSize;
    int totalParticles = 0;
    sf::RenderWindow window(sf::VideoMode({ static_cast<unsigned int>(windowWidth), static_cast<unsigned int>(windowHeight) }), "Grid of Squares");
    window.setFramerateLimit(60);

    vector<vector<bool>> squareStates(rows, vector<bool>(cols, false));
    vector<pair<int, int>> activeParticles;

    sf::RenderTexture staticCanvas({ static_cast<unsigned int>(windowWidth), static_cast<unsigned int>(windowHeight) });
    sf::RenderTexture frameBuffer({ static_cast<unsigned int>(windowWidth), static_cast<unsigned int>(windowHeight) });

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(-1, 1);

    sf::RectangleShape square(sf::Vector2f(static_cast<float>(squareSize), static_cast<float>(squareSize)));

    staticCanvas.clear(sf::Color::Black);
    frameBuffer.clear(sf::Color::Black);

    for (int row = 0; row < rows; ++row)
    {
        for (int col = 0; col < cols; ++col)
        {
            if (squareStates[row][col])
            {
                square.setFillColor(getHueColor(totalParticles));
                square.setPosition(sf::Vector2f(static_cast<float>(col * squareSize), static_cast<float>(row * squareSize)));
                staticCanvas.draw(square);
            }
        }
    }
    staticCanvas.display();

    while (window.isOpen())
    {
        while (window.pollEvent())
        {
            while (const optional event = window.pollEvent())
            {
                if (event->is<sf::Event::Closed>()) window.close();
                
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R))
                {
                    resetGrid(squareStates, activeParticles, rows, cols);
                    totalParticles = 0;
                    staticCanvas.clear(sf::Color::Black);
                }
            }
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            int col = mousePos.x / squareSize;
            int row = mousePos.y / squareSize;
            if (row >= 0 && row < rows && col >= 0 && col < cols && !squareStates[row][col])
            {
                squareStates[row][col] = true;
                activeParticles.emplace_back(row, col);
                totalParticles++;
            }
        }

        vector<pair<int, int>> newActiveParticles;
        bool particlesMoved = false;

        if (!activeParticles.empty())
        {
            for (auto [row, col] : activeParticles)
            {
                bool moved = false;
                if (row + 1 < rows && !squareStates[row + 1][col])
                {
                    squareStates[row + 1][col] = true;
                    squareStates[row][col] = false;
                    newActiveParticles.emplace_back(row + 1, col);
                    moved = true;
                    particlesMoved = true;
                }
                else if (col > 0 && row + 1 < rows && !squareStates[row + 1][col - 1])
                {
                    squareStates[row + 1][col - 1] = true;
                    squareStates[row][col] = false;
                    newActiveParticles.emplace_back(row + 1, col - 1);
                    moved = true;
                    particlesMoved = true;
                }
                else if (col < cols - 1 && row + 1 < rows && !squareStates[row + 1][col + 1])
                {
                    squareStates[row + 1][col + 1] = true;
                    squareStates[row][col] = false;
                    newActiveParticles.emplace_back(row + 1, col + 1);
                    moved = true;
                    particlesMoved = true;
                }
                if (!moved) { //Static
                    square.setFillColor(getHueColor(totalParticles));
                    square.setPosition(sf::Vector2f(static_cast<float>(col * squareSize),
                        static_cast<float>(row * squareSize)));
                    staticCanvas.draw(square);
                }
            }

            activeParticles = newActiveParticles;

            if (particlesMoved) {
                // Only draw particles that just became static
                for (auto [row, col] : activeParticles) {
                    bool found = false;
                    for (const auto& [newRow, newCol] : newActiveParticles) {
                        if (row == newRow && col == newCol) {
                            found = true;
                            break;
                        }
                    }
                    // If particle isn't in new active list, it became static
                    if (!found) {
                        square.setFillColor(getHueColor(totalParticles));
                        square.setPosition(sf::Vector2f(static_cast<float>(col * squareSize),
                            static_cast<float>(row * squareSize)));
                        staticCanvas.draw(square);
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
        for (auto [row, col] : activeParticles)
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
