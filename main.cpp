#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <array>
#include <chrono>
#include <thread>
#include <iostream>

namespace Dimensions{
    constexpr int cellXDims{ 20 };
    constexpr int cellYDims{ 20 };
    constexpr int winXDims{ 800 };
    constexpr int winYDims{ 600 };
    constexpr int gridXDims{ winXDims / cellXDims };
    constexpr int gridYDims{ winYDims / cellYDims };
}

using grid_t = std::array<std::array<bool, Dimensions::gridXDims>, Dimensions::gridYDims>;
using cell_t = sf::RectangleShape;

cell_t buildRectangleShape(const sf::Vector2f& size, const sf::Color& color);
void renderCells(sf::RenderWindow& window, grid_t grid, cell_t& liveCell, cell_t& deadCell);
void updateCell(int x, int y, const grid_t& currentGrid, grid_t& nextGrid);
void updateGrid(const grid_t& currentGrid, grid_t& nextGrid);
void setTestCase(grid_t& currentGrid);
void convertXYToTile(int x, int y, grid_t& currentGrid);
void clearGrid(grid_t& currentGrid);
void renderCurrentScreen(sf::RenderWindow& window, const grid_t& currentGrid, cell_t& liveCell, cell_t& deadCell);

int main()
{
    sf::RenderWindow window(sf::VideoMode(Dimensions::winXDims, Dimensions::winYDims), "Game of Life");
    const sf::Vector2f cellSize { Dimensions::cellXDims, Dimensions::cellYDims };
    sf::Color liveColor{ sf::Color::White };
    sf::Color deadColor{ sf::Color::Black };

    sf::RectangleShape liveCell{ buildRectangleShape(cellSize, liveColor) };
    sf::RectangleShape deadCell{ buildRectangleShape(cellSize, deadColor) };

    grid_t currentGrid{};
    grid_t nextGrid{};

    //setTestCase(currentGrid);

    bool started{ false };
    while (window.isOpen())
    {
        if (!started){ // load first grid before starting
            renderCurrentScreen(window, currentGrid, liveCell, deadCell);
        }
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (!started){
                // reset cells
                if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Key::R)){
                    clearGrid(currentGrid);
                    renderCurrentScreen(window, currentGrid, liveCell, deadCell);
                }
                // add cells
                if (event.type == sf::Event::MouseButtonPressed){
                    convertXYToTile(event.mouseButton.x, event.mouseButton.y, currentGrid);
                    renderCurrentScreen(window, currentGrid, liveCell, deadCell);
                }
            }
            // start/stop
            if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Key::Space))
                started = !started;

            if (event.type == sf::Event::Closed)
                window.close();
        }
        if (started){
            updateGrid(currentGrid, nextGrid);
            renderCurrentScreen(window, currentGrid, liveCell, deadCell);
            nextGrid.swap(currentGrid);
            // delay between updates
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    return 0;
}

cell_t buildRectangleShape(const sf::Vector2f& size, const sf::Color& color){
    sf::RectangleShape newShape{size};
    newShape.setFillColor(color);
    return newShape;
}

void renderCells(sf::RenderWindow& window, grid_t grid, cell_t& liveCell, cell_t& deadCell){
    float x{0};
        float y{0};
        for (auto& row : grid){
            x = 0;
            for (auto& tile : row){
                if (tile){
                    liveCell.setPosition({x, y});
                    window.draw(liveCell);
                }
                else{
                    deadCell.setPosition({x, y});
                    window.draw(deadCell);
                }
                x += Dimensions::cellXDims;
            }
            y += Dimensions::cellYDims;
        }
}

void updateCell(int x, int y, const grid_t& currentGrid, grid_t& nextGrid){
    int maxCols = currentGrid.size();
    int maxRows = currentGrid[0].size();
    int count = 0;
    int x_pos = (x + 1) % maxCols;
    int y_pos = (y + 1) % maxRows;
    int x_neg = (x == 0) ? maxCols - 1 : x - 1;
    int y_neg = (y == 0) ? maxRows - 1 : y - 1;
    count += currentGrid[x_neg][y];      // d
    count += currentGrid[x_pos][y];      // u
    count += currentGrid[x][y_neg];      // l
    count += currentGrid[x][y_pos];      // r
    count += currentGrid[x_neg][y_neg];  // dl
    count += currentGrid[x_neg][y_pos];  // ur
    count += currentGrid[x_pos][y_neg];  // ul
    count += currentGrid[x_pos][y_pos];  // ur
    if (currentGrid[x][y]) {
        nextGrid[x][y] = (count == 2 || count == 3);
    } else {
        nextGrid[x][y] = (count == 3);
    }
}

void updateGrid(const grid_t& currentGrid, grid_t& nextGrid){
    for (grid_t::size_type x{ 0 }; x < currentGrid.size(); ++x){
        for (grid_t::size_type y{ 0 }; y < currentGrid[0].size(); ++y){
            updateCell(x, y, currentGrid, nextGrid);
        }
    }
}

void setTestCase(grid_t& currentGrid){
    // block
    // currentGrid[0][0] = true;
    // currentGrid[0][1] = true;
    // currentGrid[1][0] = true;
    // currentGrid[1][1] = true;

    // glider
    // 010
    // 001
    // 111
    currentGrid[0][0] = false;
    currentGrid[0][1] = true;
    currentGrid[0][2] = false;
    currentGrid[1][0] = false;
    currentGrid[1][1] = false;
    currentGrid[1][2] = true;
    currentGrid[2][0] = true;
    currentGrid[2][1] = true;
    currentGrid[2][2] = true;
}

void convertXYToTile(int x, int y, grid_t& currentGrid){
    int convertedX{ x / Dimensions::cellXDims};
    int convertedY{ y / Dimensions::cellYDims};

    if (convertedX < 0 || convertedX > Dimensions::gridXDims) return;
    if (convertedY < 0 || convertedY > Dimensions::gridYDims) return;

    currentGrid[convertedY][convertedX] ^= true;
}

void clearGrid(grid_t& currentGrid){
    for (auto& row : currentGrid) row.fill(false);
}

void renderCurrentScreen(sf::RenderWindow& window, const grid_t& currentGrid, cell_t& liveCell, cell_t& deadCell){
    window.clear();
    renderCells(window, currentGrid, liveCell, deadCell);
    window.display();
}