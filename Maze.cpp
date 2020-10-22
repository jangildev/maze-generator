#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <stack>
#include <vector>
#include <fstream>

enum class Direction
{
    Left,
    Top,
    Right ,
    Bottom 
};


bool existsFieldAtPosition(int x, int y , std::vector<sf::RectangleShape> fields); //get field which contains position
void removeWall(sf::FloatRect bounds , std::vector<sf::RectangleShape> & walls);
std::vector<sf::Vector2f> getLongestPath(std::vector<std::vector<sf::Vector2f>> paths); //bubble sort

int main()
{
    srand(NULL);

    const int CELL_WIDTH = 30;
    const int CELL_HEIGHT = 30;

    const int WIDTH = 750;
    const int HEIGHT = 750;

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Maze", sf::Style::Close);
    window.setFramerateLimit(0);
    window.setVerticalSyncEnabled(true);

    std::vector<sf::RectangleShape> walls;
    std::vector<sf::RectangleShape> fields;
    std::stack<std::pair<int, int>> path;

    std::vector<std::vector<sf::Vector2f>> allPaths; //all possible paths
    allPaths.push_back({ sf::Vector2f(0,0) });

    sf::VertexArray longestPath = sf::VertexArray(sf::PrimitiveType::LinesStrip); //longest possible path without repetitions
    sf::RectangleShape start; //start of the maze
    sf::RectangleShape end; // end of maze
    int current_path = 0; 

    int maxCells = (WIDTH / CELL_WIDTH) * (HEIGHT / CELL_HEIGHT);
    int visitedCells = 1;

    sf::RectangleShape startField;
    startField.setPosition(0, 0);
    startField.setSize(sf::Vector2f(CELL_WIDTH , CELL_HEIGHT));
    startField.setFillColor(sf::Color::White);
    fields.push_back(startField);

    path.push(std::make_pair(0,0));

    //Building initial walls grid

    for (int i = 0; i < WIDTH; i += CELL_WIDTH)
    {
        for (int j = 0; j < HEIGHT; j += CELL_HEIGHT)
        {
            sf::RectangleShape leftWall;
            leftWall.setPosition(i - (CELL_WIDTH / 10) , j - (CELL_HEIGHT / 10));
            leftWall.setSize(sf::Vector2f((CELL_WIDTH / 5), CELL_HEIGHT + (CELL_HEIGHT / 5)));
            leftWall.setFillColor(sf::Color::Black);

            sf::RectangleShape topWall;
            topWall.setPosition(i - (CELL_WIDTH / 10), j - (CELL_HEIGHT / 10));
            topWall.setSize(sf::Vector2f(CELL_WIDTH + (CELL_WIDTH / 5), (CELL_HEIGHT / 5)));
            topWall.setFillColor(sf::Color::Black);

            walls.push_back(leftWall);
            walls.push_back(topWall);

            if (i + CELL_WIDTH == WIDTH)
            {
                sf::RectangleShape rightWall;
                rightWall.setPosition(i + CELL_WIDTH - (CELL_WIDTH / 10), j - (CELL_HEIGHT / 10));
                rightWall.setSize(sf::Vector2f((CELL_WIDTH / 5), CELL_HEIGHT + (CELL_HEIGHT / 5)));
                rightWall.setFillColor(sf::Color::Black);
                walls.push_back(rightWall);
            }

            if (j + CELL_HEIGHT == HEIGHT)
            {
                sf::RectangleShape bottomWall;
                bottomWall.setPosition(i - (CELL_WIDTH / 10) , j + (CELL_HEIGHT - (CELL_HEIGHT / 10)));
                bottomWall.setSize(sf::Vector2f(CELL_WIDTH + (CELL_WIDTH / 5) , CELL_HEIGHT / 5));
                bottomWall.setFillColor(sf::Color::Black);
                walls.push_back(bottomWall);
            }
        }
    }

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (visitedCells < maxCells)
        {
            std::pair<int, int> current_pos = path.top();
            std::vector<Direction> dirs = { Direction::Left , Direction::Top , Direction::Right , Direction::Bottom };

            //Exclusion of occupied neighbors

            if (current_pos.first == 0 || existsFieldAtPosition(current_pos.first - CELL_WIDTH , current_pos.second , fields))
            {
                auto it = std::find(dirs.begin() , dirs.end() , Direction::Left);
                dirs.erase(it);
            }

            if (current_pos.first == WIDTH - CELL_WIDTH || existsFieldAtPosition(current_pos.first + CELL_WIDTH, current_pos.second, fields))
            {
                auto it = std::find(dirs.begin(), dirs.end(), Direction::Right);
                dirs.erase(it);
            }

            if (current_pos.second == 0 || existsFieldAtPosition(current_pos.first , current_pos.second - CELL_HEIGHT , fields))
            {
                auto it = std::find(dirs.begin(), dirs.end(), Direction::Top);
                dirs.erase(it);
            }
            
            if (current_pos.second == HEIGHT - CELL_HEIGHT || existsFieldAtPosition(current_pos.first, current_pos.second + CELL_HEIGHT, fields))
            {
                auto it = std::find(dirs.begin(), dirs.end(), Direction::Bottom);
                dirs.erase(it);
            }

            if (dirs.size() == 0) // if there is not possible direction - turn back
            {
                allPaths.push_back(std::vector<sf::Vector2f>(allPaths[current_path].begin() , allPaths[current_path].end() - 1));
                current_path++;
                path.pop();
            }
            else
            {
                Direction current_dir = dirs[rand() % dirs.size()];

                std::pair<int, int> new_pos;

                sf::FloatRect wall_bounds;

                switch (current_dir)
                {
                case Direction::Left:
                    {
                        new_pos = std::make_pair(current_pos.first - CELL_WIDTH, current_pos.second);
                        wall_bounds = sf::FloatRect(new_pos.first + (CELL_WIDTH / 2) , new_pos.second + (CELL_HEIGHT / 4), CELL_WIDTH, CELL_HEIGHT - (CELL_HEIGHT / 2));
                    }
                    break;
                case Direction::Top:
                    {
                        new_pos = std::make_pair(current_pos.first, current_pos.second - CELL_HEIGHT);
                        wall_bounds = sf::FloatRect(new_pos.first + (CELL_WIDTH / 4), new_pos.second + (CELL_HEIGHT / 2), CELL_WIDTH - (CELL_WIDTH / 2) , CELL_HEIGHT);
                    }
                    break;
                case Direction::Right:
                    {
                        new_pos = std::make_pair(current_pos.first + CELL_WIDTH, current_pos.second);
                        wall_bounds = sf::FloatRect(current_pos.first + (CELL_WIDTH / 2), current_pos.second + (CELL_HEIGHT / 4), CELL_WIDTH ,  CELL_HEIGHT - (CELL_HEIGHT / 2));
                    }
                    break;
                case Direction::Bottom:
                    {
                        new_pos = std::make_pair(current_pos.first, current_pos.second + CELL_HEIGHT);
                        wall_bounds = sf::FloatRect(current_pos.first + (CELL_WIDTH / 4), current_pos.second + (CELL_HEIGHT / 2), CELL_WIDTH - (CELL_WIDTH / 2), CELL_HEIGHT);
                    }
                    break;
                default:
                    break;
                }

                removeWall(wall_bounds , walls);

                path.push(new_pos);

                sf::RectangleShape f;
                f.setPosition(new_pos.first , new_pos.second);
                f.setSize(sf::Vector2f(CELL_WIDTH , CELL_HEIGHT));
                f.setFillColor(sf::Color::White);
                fields.push_back(f);

                allPaths[current_path].push_back(f.getPosition());

                visitedCells++;
            }

            //Generate longest path

            if (visitedCells == maxCells)
            {
                std::vector<sf::Vector2f> path = getLongestPath(allPaths);

                start.setSize(sf::Vector2f(CELL_WIDTH, CELL_HEIGHT));
                start.setPosition(path[0]);
                start.setFillColor(sf::Color::Green);

                end.setSize(sf::Vector2f(CELL_WIDTH, CELL_HEIGHT));
                end.setPosition(path[path.size() - 1]);
                end.setFillColor(sf::Color::Red);

                for (int i = 0; i < path.size(); i++)
                {
                    sf::Vertex point;
                    point.color = sf::Color::Magenta;
                    point.position = sf::Vector2f(path[i].x + CELL_WIDTH / 2, path[i].y + CELL_HEIGHT / 2);
                    longestPath.append(point);
                }

                //Write maze to file

                /*std::ofstream stream;
                stream.open("maze.txt");
                stream << "window\n";
                stream << WIDTH << " " << HEIGHT << "\n";
                stream << "cell\n";
                stream << CELL_WIDTH << " " << CELL_HEIGHT << "\n";
                stream << "walls\n";
                for (auto w : walls)
                {
                    sf::FloatRect b = w.getGlobalBounds();
                    stream << b.left << " " << b.top << " " << b.width << " " << b.height << "\n";
                }
                stream << "path\n";
                for (auto p : path)
                {
                    stream << p.x << " " << p.y << "\n";
                }
                stream.close();*/
            }
        }

        // Drawing things

        window.clear(sf::Color::Black);

        for (auto f : fields)
        {
            window.draw(f);
        }

        if (visitedCells == maxCells)
        {
            window.draw(start);
            window.draw(end);

            window.draw(longestPath);
        }

        for (auto w : walls)
        {
            window.draw(w);
        }

        window.display();
    }

    return 0;
}

bool existsFieldAtPosition(int x, int y , std::vector<sf::RectangleShape> fields)
{
    for (auto f : fields)
    {
        if (f.getPosition().x == x && f.getPosition().y == y)
        {
            return true;
        }
    }

    return false;
}

void removeWall(sf::FloatRect bounds, std::vector<sf::RectangleShape>& walls)
{
    int i = 0;
    for (auto w : walls)
    {
        if (w.getGlobalBounds().intersects(bounds))
        {
            walls.erase(walls.begin() + i);
            return;
        }
        i++;
    }

}

std::vector<sf::Vector2f> getLongestPath(std::vector<std::vector<sf::Vector2f>> paths) 
{
    for (int i = 0; i < paths.size(); i++)
    {
        for (int j = 0; j < paths.size() - 1; j++)
        {
            if (paths[j + 1].size() > paths[j].size() )
            {
                std::vector<sf::Vector2f> swap = paths[j];
                paths[j] = paths[j + 1];
                paths[j + 1] = swap;
            }
        }
    }

    return paths[0];
}