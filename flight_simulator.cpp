#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <cmath>
#include <algorithm>
#include <string>
#include <cctype>
#include <iomanip>
#include <ctime>
#include <random>
#include <chrono>
#include <sstream>
using namespace std;

struct Airport
{
    string code;
    sf::Vector2f position;
};

struct WeatherCondition
{
    bool isBad;
    string description;
};

struct FlightGraph
{
    vector<Airport> airports;
    vector<vector<pair<int, double>>> adj;
    vector<vector<bool>> pathAvailable;
    vector<vector<WeatherCondition>> pathWeather;

    void addAirport(const string &code, float x, float y)
    {
        airports.push_back({code, {x, y}});
        adj.emplace_back();

        for (auto &row : pathAvailable)
        row.push_back(false);
        pathAvailable.push_back(vector<bool>(airports.size(), false));

        for (auto &row : pathWeather)
        row.push_back({false, "Clear"});
        pathWeather.push_back(vector<WeatherCondition>(airports.size(), {false, "Clear"}));
    }

    void addEdge(int u, int v, double dist)
    {
        adj[u].emplace_back(v, dist);
        adj[v].emplace_back(u, dist);
        pathAvailable[u][v] = true;
        pathAvailable[v][u] = true;
        pathWeather[u][v] = {false, "Clear"};
        pathWeather[v][u] = {false, "Clear"};
    }

    void updateWeather(int u, int v, bool isBad, const string &description)
    {
        pathWeather[u][v] = {isBad, description};
        pathWeather[v][u] = {isBad, description};
        pathAvailable[u][v] = !isBad;
        pathAvailable[v][u] = !isBad;
    }

    bool hasBadWeather(const vector<int> &path) const
    {
        for (size_t i = 0; i < path.size() - 1; ++i)
        {
            int u = path[i];
            int v = path[i + 1];
            if (pathWeather[u][v].isBad)
            {
                return true;
            }
        }
        return false;
    }

    vector<pair<string, string>> getPathWeatherInfo(const vector<int> &path) const
    {
        vector<pair<string, string>> result;
        for (size_t i = 0; i < path.size() - 1; ++i)
        {
            int u = path[i];
            int v = path[i + 1];
            if (pathWeather[u][v].isBad)
            {
                result.push_back({airports[u].code + "-" + airports[v].code, pathWeather[u][v].description});
            }
        }
        return result;
    }

    vector<int> dijkstra(int src, int dst) const
    {
        int n = adj.size();
        vector<double> dist(n, numeric_limits<double>::infinity());
        vector<int> prev(n, -1);
        dist[src] = 0;

        using PDI = pair<double, int>;
        priority_queue<PDI, vector<PDI>, greater<>> pq;
        pq.push({0, src});

        while (!pq.empty())
        {
            auto [d, u] = pq.top();
            pq.pop();
            if (u == dst)
                break;

            for (auto [v, w] : adj[u])
            {
                if (!pathAvailable[u][v])
                    continue;

                double alt = d + w;
                if (alt < dist[v])
                {
                    dist[v] = alt;
                    prev[v] = u;
                    pq.push({alt, v});
                }
            }
        }

        vector<int> path;
        for (int at = dst; at != -1; at = prev[at])
        path.push_back(at);
        reverse(path.begin(), path.end());

        if (path.empty() || path.front() != src)
        return {};
        return path;
    }

    vector<int> findRouteWithWeatherRerouting(int src, int dst, bool &rerouted)
    {
        vector<int> originalPath = dijkstra(src, dst);
        if (originalPath.empty() || !hasBadWeather(originalPath))
        {
            rerouted = false;
            return originalPath;
        }

        rerouted = true;

        FlightGraph tempGraph = *this;

        for (size_t i = 0; i < originalPath.size() - 1; ++i)
        {
            int u = originalPath[i];
            int v = originalPath[i + 1];
            if (pathWeather[u][v].isBad)
            {

                tempGraph.pathAvailable[u][v] = false;
                tempGraph.pathAvailable[v][u] = false;
            }
        }
        return tempGraph.dijkstra(src, dst);
    }
};

struct FlightTicket
{
    int departureAirportIndex;
    int arrivalAirportIndex;
    string departureDate;
    string departureTime;
    string arrivalTime;
    double price;
    string seatNumber;
    string passengerName;
    string bookingReference;
    bool isBooked;
};

struct Date
{
    int day;
    int month;
    int year;

    string toString() const
    {
        stringstream ss;
        ss << setfill('0') << setw(2) << day << "/"<< setfill('0') << setw(2) << month << "/"<< year;
        return ss.str();
    }

    static Date fromString(const string &dateStr)
    {
        Date date;
        stringstream ss(dateStr);
        char delimiter;
        ss >> date.day >> delimiter >> date.month >> delimiter >> date.year;
        return date;
    }

    Date addDays(int days) const
    {
        Date newDate = *this;
        newDate.day += days;

        int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

        if (newDate.year % 4 == 0)
        {
            daysInMonth[2] = 29;
        }

        while (newDate.day > daysInMonth[newDate.month])
        {
            newDate.day -= daysInMonth[newDate.month];
            newDate.month++;

            if (newDate.month > 12)
            {
                newDate.month = 1;
                newDate.year++;

                if (newDate.year % 4 == 0)
                {
                    daysInMonth[2] = 29;
                }
                else
                {
                    daysInMonth[2] = 28;
                }
            }
        }
        return newDate;
    }

    static Date getCurrentDate()
    {
        auto now = chrono::system_clock::now();
        time_t now_time = chrono::system_clock::to_time_t(now);
        tm *now_tm = localtime(&now_time);

        Date currentDate;
        currentDate.day = now_tm->tm_mday;
        currentDate.month = now_tm->tm_mon + 1;
        currentDate.year = now_tm->tm_year + 1900;

        return currentDate;
    }
};

void printLine(char c = '-', int length = 50)
{
    for (int i = 0; i < length; i++)
    {
        cout << c;
    }
    cout << endl;
}

int resolveAirportIndex(const string &input, const vector<Airport> &airports)
{
    if (isdigit(input[0]))
    return stoi(input);
    for (size_t i = 0; i < airports.size(); ++i)
    if (airports[i].code == input)
    return static_cast<int>(i);
    return -1;
}

void visualizeGraph(const FlightGraph &graph, const vector<int> &path, bool rerouted = false)
{
    sf::RenderWindow window(sf::VideoMode(900, 650), "Flight Path Visualization");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("default.ttf"))
    {
        cerr << "Error loading font 'default.ttf'. Make sure it's available.\n";
        return;
    }

    sf::Color availableColor = sf::Color(100, 255, 100);
    sf::Color unavailableColor = sf::Color(255, 80, 80);
    sf::Color airportColor = sf::Color(50, 120, 250);
    sf::Color pathColor = sf::Color::Yellow;
    sf::Color reroutedPathColor = sf::Color(255, 165, 0);
    sf::Color planeColor = sf::Color::White;
    sf::Color waypointColor = sf::Color::Magenta;
    sf::Color badWeatherColor = sf::Color(255, 0, 0, 128);

    cout << "\nVisualizing path: ";
    for (int idx : path)
    {
        cout << graph.airports[idx].code << " ";
    }
    cout << "\n";

    vector<sf::CircleShape> airportShapes;
    vector<sf::Text> airportLabels;

    for (size_t i = 0; i < graph.airports.size(); ++i)
    {
        const auto &airport = graph.airports[i];
        sf::CircleShape shape(8);
        shape.setFillColor(airportColor);
        shape.setPosition(airport.position);
        airportShapes.push_back(shape);

        sf::Text label;
        label.setFont(font);
        label.setString(airport.code);
        label.setCharacterSize(14);
        label.setFillColor(sf::Color::White);
        label.setPosition(airport.position.x + 12, airport.position.y - 5);
        airportLabels.push_back(label);
    }

    vector<sf::CircleShape> pathAirportShapes;
    if (!path.empty())
    {
        for (size_t i = 0; i < path.size(); ++i)
        {
            const auto &airport = graph.airports[path[i]];
            sf::CircleShape shape(10);
            shape.setFillColor(waypointColor);
            shape.setPosition(airport.position.x - 2, airport.position.y - 2);
            pathAirportShapes.push_back(shape);
        }
    }

    vector<sf::VertexArray> connectionLines;
    vector<sf::VertexArray> badWeatherLines;

    vector<pair<int, int>> badWeatherPaths;

    for (int i = 0; i < graph.airports.size(); ++i)
    {
        for (int j = i + 1; j < graph.airports.size(); ++j)
        {
            sf::Color color;
            bool isBadWeather = graph.pathWeather[i][j].isBad;

            if (isBadWeather)
            {
                sf::VertexArray badWeatherLine(sf::Lines, 2);
                badWeatherLine[0] = sf::Vertex(graph.airports[i].position, badWeatherColor);
                badWeatherLine[1] = sf::Vertex(graph.airports[j].position, badWeatherColor);
                badWeatherLines.push_back(badWeatherLine);

                badWeatherPaths.push_back({i, j});
                badWeatherPaths.push_back({j, i});

                color = unavailableColor;
            }
            else if(!graph.pathAvailable[i][j])
            {
                color = sf::Color(150, 150, 150);
            }
            else
            {
                color = availableColor;
            }

            sf::VertexArray line(sf::Lines, 2);
            line[0] = sf::Vertex(graph.airports[i].position, color);
            line[1] = sf::Vertex(graph.airports[j].position, color);
            connectionLines.push_back(line);
        }
    }

    sf::Text statusText;
    statusText.setFont(font);
    if (rerouted)
    {
        statusText.setString("FLIGHT REROUTED DUE TO WEATHER CONDITIONS");
        statusText.setFillColor(sf::Color::Red);
    }
    else if (path.empty())
    {
        statusText.setString("NO PATH AVAILABLE");
        statusText.setFillColor(sf::Color::Red);
    }
    else
    {
        statusText.setString("ORIGINAL FLIGHT PATH");
        statusText.setFillColor(sf::Color::Green);
    }
    statusText.setCharacterSize(18);
    statusText.setPosition(20, 20);

    sf::Text pathText;
    pathText.setFont(font);
    string pathStr = "Path: ";
    for (size_t i = 0; i < path.size(); ++i)
    {
        pathStr += graph.airports[path[i]].code;
        if (i < path.size() - 1)
        pathStr += " → ";
    }

    pathText.setString(pathStr);
    pathText.setCharacterSize(16);
    pathText.setFillColor(sf::Color::White);
    pathText.setPosition(20, 50);

    vector<sf::VertexArray> pathSegments;
    vector<float> pathLengths;
    vector<pair<int, int>> pathPairs;
    float totalPathLength = 0.0f;

    if (!path.empty())
    {
        for (size_t i = 1; i < path.size(); ++i)
        {
            int fromIdx = path[i - 1];
            int toIdx = path[i];
            sf::Vector2f start = graph.airports[fromIdx].position;
            sf::Vector2f end = graph.airports[toIdx].position;

            bool hasBadWeather = graph.pathWeather[fromIdx][toIdx].isBad;

            float dx = end.x - start.x;
            float dy = end.y - start.y;
            float length = sqrt(dx * dx + dy * dy);
            pathLengths.push_back(length);
            totalPathLength += length;

            pathPairs.push_back({fromIdx, toIdx});

            sf::VertexArray segment(sf::Lines, 2);
            sf::Color actualPathColor;

            if (hasBadWeather)
            {
                cout << "WARNING: Path includes bad weather segment: "
                     << graph.airports[fromIdx].code << " -> "
                     << graph.airports[toIdx].code << "\n";
                actualPathColor = sf::Color::Red;
            }
            else if (rerouted)
            {
                actualPathColor = reroutedPathColor;
            }
            else
            {
                actualPathColor = pathColor;
            }

            segment[0] = sf::Vertex(start, actualPathColor);
            segment[1] = sf::Vertex(end, actualPathColor);
            pathSegments.push_back(segment);

            cout << "Created path segment: " << graph.airports[fromIdx].code << " -> "<< graph.airports[toIdx].code << " (length: " << length << ")";

            if (hasBadWeather)
            {
                cout << " [BAD WEATHER!]";
            }
            cout << "\n";
        }
    }

    sf::Clock clock;
    float animationProgress = 0.0f;
    const float animationSpeed = 0.3f;

    sf::CircleShape airplane(6, 3);
    airplane.setFillColor(planeColor);
    airplane.setOrigin(6, 6);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        if (event.type == sf::Event::Closed)
        window.close();

        float deltaTime = clock.restart().asSeconds();
        if (!path.empty() && path.size() > 1)
        {
            animationProgress += deltaTime * animationSpeed;
            if (animationProgress > 1.0f)
            {
                animationProgress = 0.0f;
            }
        }

        window.clear();

        for (const auto &line : connectionLines)
        window.draw(line);

        for (const auto &line : badWeatherLines)
        window.draw(line);

        for (const auto &shape : airportShapes)
        window.draw(shape);

        for (const auto &shape : pathAirportShapes)
        window.draw(shape);

        for (const auto &label : airportLabels)
        window.draw(label);
        if (!path.empty() && path.size() > 1)
        {
            float distanceCovered = animationProgress * totalPathLength;
            float accumulatedLength = 0.0f;
            int currentSegment = -1;
            float segmentProgress = 0.0f;

            for (size_t i = 0; i < pathLengths.size(); ++i)
            {
                if (distanceCovered <= accumulatedLength + pathLengths[i])
                {
                    currentSegment = i;
                    segmentProgress = (distanceCovered - accumulatedLength) / pathLengths[i];
                    break;
                }
                accumulatedLength += pathLengths[i];
            }

            if (currentSegment == -1)
            {
                currentSegment = pathLengths.size() - 1;
                segmentProgress = 1.0f;
            }

            for (int i = 0; i < currentSegment; ++i)
            {
                window.draw(pathSegments[i]);
            }

            if (currentSegment < pathSegments.size())
            {
                sf::VertexArray partialSegment = pathSegments[currentSegment];
                sf::Vector2f start = partialSegment[0].position;
                sf::Vector2f end = partialSegment[1].position;
                sf::Vector2f partialEnd = start + (end - start) * segmentProgress;

                partialSegment[1].position = partialEnd;
                window.draw(partialSegment);

                airplane.setPosition(partialEnd);
                float angle = atan2(end.y - start.y, end.x - start.x) * 180 / 3.14159f;
                airplane.setRotation(angle + 90);
            }
            window.draw(airplane);
        }

        window.draw(statusText);
        window.draw(pathText);

        sf::Text animText;
        animText.setFont(font);
        animText.setString("Press ESC to exit");
        animText.setCharacterSize(14);
        animText.setFillColor(sf::Color::White);
        animText.setPosition(20, 80);
        window.draw(animText);

        window.display();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        window.close();
    }
}

int main(int argc, char *argv[])
{
    ios_base::sync_with_stdio(true);

    int src = -1, dst = -1;
    bool useCommandLineArgs = false;

    if (argc == 3)
    {
        try
        {
            src = stoi(argv[1]);
            dst = stoi(argv[2]);
            useCommandLineArgs = true;
            cout << "Using command-line arguments: src=" << src << ", dst=" << dst << endl;
        }
        catch (const exception &e)
        {
            cerr << "Error parsing command-line arguments: " << e.what() << endl;
            useCommandLineArgs = false;
        }
    }

    FlightGraph graph;

    graph.addAirport("John F. Kennedy International Airport (New York)", 150, 100);
    graph.addAirport("Los Angeles International Airport (Los Angeles)", 50, 500);
    graph.addAirport("O'Hare International Airport (Chicago)", 350, 150);
    graph.addAirport("Dallas/Fort Worth International Airport (Dallas–Fort Worth)", 450, 350);
    graph.addAirport("Hartsfield–Jackson Atlanta International Airport (Atlanta)", 300, 300);
    graph.addAirport("San Francisco International Airport (San Francisco)", 100, 450);
    graph.addAirport("Miami International Airport (Miami)", 250, 550);
    graph.addAirport("Seattle–Tacoma International Airport (Seattle)", 50, 50);
    graph.addAirport("Denver International Airport (Denver)", 300, 200);
    graph.addAirport("Logan International Airport (Boston)", 200, 80);
    graph.addAirport("Harry Reid International Airport (Las Vegas) (formerly McCarran)", 150, 450);
    graph.addAirport("Phoenix Sky Harbor International Airport (Phoenix)", 350, 450);
    graph.addAirport("George Bush Intercontinental Airport (Houston)", 500, 400);
    graph.addAirport("Newark Liberty International Airport (Newark)", 180, 90);
    graph.addAirport("Charlotte Douglas International Airport (Charlotte)", 330, 280);

    int n = graph.airports.size();
    for (int i = 0; i < n; ++i)
    {
        for (int j = i + 1; j < n; ++j)
        {
            const auto &pi = graph.airports[i].position;
            const auto &pj = graph.airports[j].position;
            double dx = pi.x - pj.x;
            double dy = pi.y - pj.y;
            double dist = sqrt(dx * dx + dy * dy);
            graph.addEdge(i, j, dist);
        }
    }

    printLine('=');
    cout << "WELCOME TO FLIGHT SIMULATOR" << endl;
    printLine('=');

    if (!useCommandLineArgs)
    {
        cout << "Available Airports (index: code):" << endl;
        for (int i = 0; i < n; ++i)
        {
            cout << "  " << i << ": " << graph.airports[i].code << endl;
        }
        printLine();

        string input;
        cout << "FLIGHT SELECTION" << endl;
        printLine();

        do
        {
            cout << "Enter departure index or code: ";
            cout.flush();
            cin >> input;
            src = resolveAirportIndex(input, graph.airports);

            if (src < 0 || src >= n)
            {
                cout << "Invalid airport. Please try again." << endl;
                cout.flush();
            }
        }while (src < 0 || src >= n);

        do
        {
            cout << "Enter arrival index or code:   ";
            cout.flush();
            cin >> input;
            dst = resolveAirportIndex(input, graph.airports);

            if (dst < 0 || dst >= n)
            {
                cout << "Invalid airport. Please try again." << endl;
                cout.flush();
            }
        } while(dst < 0 || dst >= n);
    }

    if (useCommandLineArgs)
    {
        if (src < 0 || src >= n || dst < 0 || dst >= n)
        {
            cerr << "Invalid airport indices provided via command line." << endl;
            cerr << "Valid range is 0 to " << (n - 1) << endl;
            return 1;
        }
    }

    cout << "Selected route: " << graph.airports[src].code << " to " << graph.airports[dst].code << endl;

    printLine('=');
    cout << "WEATHER CONDITIONS UPDATE" << endl;
    printLine('=');

    cout << "Would you like to update weather conditions? (y/n): ";
    cout.flush();
    string updateWeather;
    cin >> updateWeather;

    if (updateWeather == "y" || updateWeather == "Y")
    {
        cout << "Enter number of weather updates: ";
        cout.flush();
        int numUpdates;
        cin >> numUpdates;

        for (int i = 0; i < numUpdates; ++i)
        {
            printLine('-');
            cout << "Update " << (i + 1) << ":" << endl;

            cout << "Enter airport 1 index: ";
            cout.flush();
            int a1;
            cin >> a1;

            cout << "Enter airport 2 index: ";
            cout.flush();
            int a2;
            cin >> a2;

            cout << "Enter weather condition (0 for good, 1 for bad): ";
            cout.flush();
            int condition;
            cin >> condition;

            string description;
            if (condition == 1)
            {
                cout << "Enter weather description: ";
                cout.flush();
                cin.ignore();
                getline(cin, description);
            }
            else
            {
                description = "Clear skies";
            }

            graph.updateWeather(a1, a2, condition == 1, description);

            cout << "Weather updated: "
                 << graph.airports[a1].code << " to "
                 << graph.airports[a2].code << " - "
                 << (condition == 1 ? "Bad weather (" + description + ")" : "Clear skies")
                 << endl;
        }
    }

    FlightGraph directGraph = graph;

    for (int i = 0; i < directGraph.airports.size(); ++i)
    {
        for (int j = 0; j < directGraph.airports.size(); ++j)
        {
            if (i != j)
            {
                directGraph.pathAvailable[i][j] = true;
            }
        }
    }

    vector<int> directPath = directGraph.dijkstra(src, dst);

    FlightGraph weatherGraph = graph;

    for (int i = 0; i < weatherGraph.airports.size(); ++i)
    {
        for (int j = 0; j < weatherGraph.airports.size(); ++j)
        {
            if (weatherGraph.pathWeather[i][j].isBad)
            {
                weatherGraph.pathAvailable[i][j] = false;
            }
        }
    }

    vector<int> weatherAwarePath = weatherGraph.dijkstra(src, dst);

    bool hasDirectPathBadWeather = false;
    vector<pair<string, string>> badWeatherSegments;

    if (!directPath.empty())
    {
        badWeatherSegments = graph.getPathWeatherInfo(directPath);
        hasDirectPathBadWeather = !badWeatherSegments.empty();
    }

    bool rerouted = false;
    vector<int> finalPath;

    if (weatherAwarePath.empty())
    {
        cout << "No path found between the selected airports due to weather conditions.\n";
        finalPath = weatherAwarePath;
    }
    else if (hasDirectPathBadWeather)
    {
        cout << "\nBAD WEATHER DETECTED on the direct route!\n";
        cout << "Affected segments:\n";
        for (const auto &segment : badWeatherSegments)
        {
            cout << "  " << segment.first << ": " << segment.second << "\n";
        }

        bool pathsAreDifferent = directPath.size() != weatherAwarePath.size() || !equal(directPath.begin(), directPath.end(), weatherAwarePath.begin());

        if (pathsAreDifferent && !weatherAwarePath.empty())
        {
            cout << "Automatically rerouted to avoid bad weather.\n";
            rerouted = true;
            finalPath = weatherAwarePath;

            bool finalPathHasBadWeather = false;
            for (size_t i = 1; i < finalPath.size(); ++i)
            {
                int u = finalPath[i - 1];
                int v = finalPath[i];
                if (graph.pathWeather[u][v].isBad)
                {
                    finalPathHasBadWeather = true;
                    cout << "WARNING: Rerouted path still contains bad weather segment: "<< graph.airports[u].code << " -> " << graph.airports[v].code << "\n";
                }
            }

            if (finalPathHasBadWeather)
            {
                cout << "Fixing rerouted path to completely avoid bad weather...\n";
                FlightGraph fixedGraph = graph;
                for (int i = 0; i < fixedGraph.airports.size(); ++i)
                {
                    for (int j = 0; j < fixedGraph.airports.size(); ++j)
                    {
                        if (fixedGraph.pathWeather[i][j].isBad)
                        {
                            fixedGraph.pathAvailable[i][j] = false;
                        }
                    }
                }
                finalPath = fixedGraph.dijkstra(src, dst);

                if (finalPath.empty())
                {
                    cout << "No completely safe path found. Using best available route.\n";
                    finalPath = weatherAwarePath;
                }
            }

            cout << "New route: ";
            for (int idx : finalPath)
            cout << graph.airports[idx].code << " ";
            cout << "\n";
        }
        else
        {
            cout << "Attempting to find alternative route...\n";

            FlightGraph rerouteGraph = graph;

            for (int i = 0; i < rerouteGraph.airports.size(); ++i)
            {
                for (int j = 0; j < rerouteGraph.airports.size(); ++j)
                {
                    if (rerouteGraph.pathWeather[i][j].isBad)
                    {
                        rerouteGraph.pathAvailable[i][j] = false;
                    }
                }
            }

            vector<int> alternativePath = rerouteGraph.dijkstra(src, dst);

            if (alternativePath.empty())
            {
                cout << "No alternative route found. All possible paths are affected by bad weather.\n";
                finalPath = weatherAwarePath;
            }
            else
            {
                cout << "REROUTED SUCCESSFULLY!\n";
                rerouted = true;
                finalPath = alternativePath;

                cout << "New route: ";
                for (int idx : finalPath)
                cout << graph.airports[idx].code << " ";
                cout << "\n";
            }
        }
    }
    else
    {
        cout << "Route has good weather conditions.\n";
        finalPath = weatherAwarePath;

        cout << "Shortest path: ";
        for (int idx : finalPath)
        cout << graph.airports[idx].code << " ";
        cout << "\n";
    }

    visualizeGraph(graph, finalPath, rerouted);
    return 0;
}
