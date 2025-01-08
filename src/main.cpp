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

        vector<pair<int, int>> newActiveParticles;
        for (auto [row, col] : activeParticles)
        {
            if (row < rows - 1 && !squareStates[row + 1][col])
            {
                squareStates[row + 1][col] = true;
                squareStates[row][col] = false;
                newActiveParticles.emplace_back(row + 1, col);
            }
            else
            {
                bool checkLeftFirst = dist(gen);
                if (checkLeftFirst)
                {
                    if (col > 0 && row < rows - 1 && !squareStates[row + 1][col - 1])
                    {
                        squareStates[row + 1][col - 1] = true;
                        squareStates[row][col] = false;
                        newActiveParticles.emplace_back(row + 1, col - 1);
                    }
                    else if (col < cols - 1 && row < rows - 1 && !squareStates[row + 1][col + 1])
                    {
                        squareStates[row + 1][col + 1] = true;
                        squareStates[row][col] = false;
                        newActiveParticles.emplace_back(row + 1, col + 1);
                    }
                    else if (row < rows - 1)
                    {
                        newActiveParticles.emplace_back(row, col);
                    }
                }
                else
                {
                    if (col < cols - 1 && row < rows - 1 && !squareStates[row + 1][col + 1])
                    {
                        squareStates[row + 1][col + 1] = true;
                        squareStates[row][col] = false;
                        newActiveParticles.emplace_back(row + 1, col + 1);
                    }
                    else if (col > 0 && row < rows - 1 && !squareStates[row + 1][col - 1])
                    {
                        squareStates[row + 1][col - 1] = true;
                        squareStates[row][col] = false;
                        newActiveParticles.emplace_back(row + 1, col - 1);
                    }
                    else if (row < rows - 1)
                    {
                        newActiveParticles.emplace_back(row, col);
                    }
                }
            }
        }
        activeParticles = move(newActiveParticles);

        window.clear();

        sf::RectangleShape square(sf::Vector2f(static_cast<float>(squareSize), static_cast<float>(squareSize)));
        square.setOutlineThickness(1.f);
        square.setOutlineColor(sf::Color::White);

        for (int row = 0; row < rows; ++row)
        {
            for (int col = 0; col < cols; ++col)
            {
                if (squareStates[row][col])
                {
                    square.setFillColor(sf::Color::White);
                    square.setPosition(sf::Vector2f(static_cast<float>(col * squareSize), static_cast<float>(row * squareSize)));
                    window.draw(square);
                }
            }
        }

        window.display();
    }
}
