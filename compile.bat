echo off

echo Compiling flight_booking.cpp...
g++ -IC:\SFML-2.6.2\include -c "C:\Users\USER\OneDrive\Desktop\Dijkstra PathFinder Visualizer\flight_booking.cpp" -o "C:\Users\USER\OneDrive\Desktop\Dijkstra PathFinder Visualizer\flight_booking.o"
if %ERRORLEVEL% NEQ 0 (
    echo Error compiling flight_booking.cpp
    pause
    exit /b 1
)

echo Linking flight_booking.exe...
g++ "C:\Users\USER\OneDrive\Desktop\Dijkstra PathFinder Visualizer\flight_booking.o" -o "C:\Users\USER\OneDrive\Desktop\Dijkstra PathFinder Visualizer\flight_booking.exe"
if %ERRORLEVEL% NEQ 0 (
    echo Error linking flight_booking.exe
    pause
    exit /b 1
)

echo  Compiling flight_simulator.cpp...
g++ -IC:\SFML-2.6.2\include -c "C:\Users\USER\OneDrive\Desktop\Dijkstra PathFinder Visualizer\flight_simulator.cpp" -o "C:\Users\USER\OneDrive\Desktop\Dijkstra PathFinder Visualizer\flight_simulator.o"
if %ERRORLEVEL% NEQ 0 (
    echo Error compiling flight_simulator.cpp
    pause
    exit /b 1
)

echo Linking flight_simulator.exe with SFML...
g++ "C:\Users\USER\OneDrive\Desktop\Dijkstra PathFinder Visualizer\flight_simulator.o" -o "C:\Users\USER\OneDrive\Desktop\Dijkstra PathFinder Visualizer\flight_simulator.exe" -LC:\SFML-2.6.2\lib -lsfml-graphics -lsfml-window -lsfml-system
if %ERRORLEVEL% NEQ 0 (
    echo Error linking flight_simulator.exe
    pause
    exit /b 1
)

echo.
echo Compilation successful!
echo.
pause
