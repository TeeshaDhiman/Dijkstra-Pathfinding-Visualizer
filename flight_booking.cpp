#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <ctime>
#include <random>
#include <chrono>
#include <sstream>
using namespace std;

struct Airport
{
    string code;
    string name;
};

struct Date
{
    int day;
    int month;
    int year;

    string toString() const
    {
        stringstream ss;
        ss << setfill('0') << setw(2) << day << "/" << setfill('0') << setw(2) << month << "/" << year;
        return ss.str();
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

    Date addDays(int days) const
    {
        Date newDate = *this;
        newDate.day += days;

        if (newDate.day > 28)
        {
            newDate.day = newDate.day % 28;
            newDate.month += 1;
            if (newDate.month > 12)
            {
                newDate.month = 1;
                newDate.year += 1;
            }
        }
        return newDate;
    }
};

struct FlightTicket
{
    string departureAirport;
    string arrivalAirport;
    string departureDate;
    string departureTime;
    string arrivalTime;
    double price;
    string passengerName;
    string seatNumber;
    string bookingReference;
    bool isBooked;
};

string generateRandomTime()
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> hour_dist(0, 23);
    uniform_int_distribution<> minute_dist(0, 59);

    int hour = hour_dist(gen);
    int minute = minute_dist(gen);

    stringstream ss;
    ss << setfill('0') << setw(2) << hour << ":" << setfill('0') << setw(2) << minute;

    return ss.str();
}

pair<string, string> generateFlightTimes(double distance)
{
    random_device rd;
    mt19937 gen(rd());

    string departureTime = generateRandomTime();

    int durationHours = static_cast<int>(distance / 100) + 1;
    uniform_int_distribution<> minute_dist(0, 59);
    int durationMinutes = minute_dist(gen);

    int depHour, depMinute;
    sscanf(departureTime.c_str(), "%d:%d", &depHour, &depMinute);

    int arrHour = depHour + durationHours;
    int arrMinute = depMinute + durationMinutes;

    if (arrMinute >= 60)
    {
        arrHour += 1;
        arrMinute -= 60;
    }

    if (arrHour >= 24)
    {
        arrHour -= 24;
    }

    stringstream ss;
    ss << setfill('0') << setw(2) << arrHour << ":" << setfill('0') << setw(2) << arrMinute;

    return {departureTime, ss.str()};
}

double generateRandomPrice(double distance)
{
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dist(0.8, 1.2);

    double basePrice = distance * 0.5;
    return basePrice * dist(gen);
}

string generateSeatNumber()
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> row_dist(1, 30);
    uniform_int_distribution<> col_dist(0, 5);

    int row = row_dist(gen);
    char col = 'A' + col_dist(gen);

    stringstream ss;
    ss << row << col;
    return ss.str();
}

string generateBookingReference()
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> char_dist(0, 25);
    uniform_int_distribution<> num_dist(0, 9);

    stringstream ss;

    for (int i = 0; i < 2; ++i)
    {
        ss << static_cast<char>('A' + char_dist(gen));
    }

    for (int i = 0; i < 4; ++i)
    {
        ss << num_dist(gen);
    }

    return ss.str();
}

void printLine(char c = '-', int length = 50)
{
    for (int i = 0; i < length; i++)
    {
        cout << c;
    }
    cout << endl;
}

FlightTicket bookFlight(const vector<Airport> &airports, int src, int dst)
{

    double distance = 500 + (src * 100) + (dst * 50);

    Date currentDate = Date::getCurrentDate();

    vector<FlightTicket> flightOptions;

    printLine('=');
    cout << "AVAILABLE FLIGHTS" << std::endl;
    printLine('=');
    cout << "From: " << airports[src].code << " To: " << airports[dst].code << endl
         << endl;
    cout << left << setw(5) << "No."
         << setw(12) << "Date"
         << setw(12) << "Departure"
         << setw(12) << "Arrival"
         << setw(10) << "Price"
         << endl;
    printLine();

    for (int i = 0; i < 5; ++i)
    {
        Date flightDate = currentDate.addDays(i);
        auto [departureTime, arrivalTime] = generateFlightTimes(distance);
        double price = generateRandomPrice(distance);

        FlightTicket ticket;
        ticket.departureAirport = airports[src].code;
        ticket.arrivalAirport = airports[dst].code;
        ticket.departureDate = flightDate.toString();
        ticket.departureTime = departureTime;
        ticket.arrivalTime = arrivalTime;
        ticket.price = price;
        ticket.isBooked = false;

        flightOptions.push_back(ticket);

        cout << left << setw(5) << (i + 1)
             << setw(12) << flightDate.toString()
             << setw(12) << departureTime
             << setw(12) << arrivalTime
             << "$" << fixed << setprecision(2) << price;
        cout << endl;
        cout.flush();
    }
    printLine();

    int selection;
    do
    {
        cout << "Select a flight (1-5): ";
        cout.flush();
        cin >> selection;

        if (selection < 1 || selection > 5)
        {
            cout << "Invalid selection. Please enter a number between 1 and 5." << endl;
            cout.flush();
        }
    } while (selection < 1 || selection > 5);

    FlightTicket selectedTicket = flightOptions[selection - 1];

    string passengerName;
    cout << "Enter passenger name: ";
    cout.flush();
    cin.ignore();
    getline(cin, passengerName);

    cout << "Processing your booking..." << endl;
    cout.flush();

    selectedTicket.passengerName = passengerName;
    selectedTicket.seatNumber = generateSeatNumber();
    selectedTicket.bookingReference = generateBookingReference();
    selectedTicket.isBooked = true;

    printLine('*');
    cout << "BOOKING CONFIRMATION" << endl;
    printLine('*');
    cout << "Booking Reference: " << selectedTicket.bookingReference << endl;
    cout << "Passenger: " << selectedTicket.passengerName << endl;
    cout << "Flight: " << selectedTicket.departureAirport << " to " << selectedTicket.arrivalAirport << endl;
    cout << "Date: " << selectedTicket.departureDate << endl;
    cout << "Time: " << selectedTicket.departureTime << " - " << selectedTicket.arrivalTime << endl;
    cout << "Seat: " << selectedTicket.seatNumber << endl;
    cout << "Price: $" << fixed << setprecision(2) << selectedTicket.price << endl;
    printLine('*');

    cout << "Ticket booked successfully!" << endl;
    cout << "Now checking weather conditions for your flight..." << endl;
    cout.flush();

    return selectedTicket;
}

int resolveAirportIndex(const string &input, const vector<Airport> &airports)
{
    bool isNumber = true;
    for (char c : input)
    {
        if (!std::isdigit(c))
        {
            isNumber = false;
            break;
        }
    }

    if (isNumber)
    {
        int index = stoi(input);
        if (index >= 0 && index < airports.size())
        {
            return index;
        }
    }
    else
    {
        for (int i = 0; i < airports.size(); ++i)
        {
            if (airports[i].code == input)
            {
                return i;
            }
        }
    }
    return -1;
}

int main()
{
    ios_base::sync_with_stdio(true);
    vector<Airport> airports = {
        {"JFK", "John F. Kennedy International Airport"},
        {"LAX", "Los Angeles International Airport"},
        {"ORD", "O'Hare International Airport"},
        {"DFW", "Dallas/Fort Worth International Airport"},
        {"ATL", "Hartsfield-Jackson Atlanta International Airport"},
        {"SFO", "San Francisco International Airport"},
        {"MIA", "Miami International Airport"},
        {"SEA", "Seattle-Tacoma International Airport"},
        {"DEN", "Denver International Airport"},
        {"BOS", "Boston Logan International Airport"},
        {"LAS", "Harry Reid International Airport"},
        {"PHX", "Phoenix Sky Harbor International Airport"},
        {"IAH", "George Bush Intercontinental Airport"},
        {"EWR", "Newark Liberty International Airport"},
        {"CLT", "Charlotte Douglas International Airport"}};

    printLine('=');
    cout << "WELCOME TO FLIGHT BOOKING SYSTEM" << endl;
    printLine('=');
    cout << "Available Airports (index: code):" << endl;
    for (int i = 0; i < airports.size(); ++i)
    {
        cout << "  " << i << ": " << airports[i].code << " - " << airports[i].name << endl;
    }
    printLine();

    string input;
    int src = -1, dst = -1;

    cout << "FLIGHT BOOKING" << endl;
    printLine();

    do
    {
        cout << "Enter departure index or code: ";
        cout.flush();
        cin >> input;
        src = resolveAirportIndex(input, airports);

        if (src < 0 || src >= airports.size())
        {
            cout << "Invalid airport. Please try again." << endl;
            cout.flush();
        }
    } while (src < 0 || src >= airports.size());

    do
    {
        cout << "Enter arrival index or code: ";
        cout.flush();
        cin >> input;
        dst = resolveAirportIndex(input, airports);

        if (dst < 0 || dst >= airports.size())
        {
            cout << "Invalid airport. Please try again." << endl;
            cout.flush();
        }
    } while (dst < 0 || dst >= airports.size());

    FlightTicket ticket = bookFlight(airports, src, dst);

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

            cout << "Weather updated: "
                 << airports[a1].code << " to "
                 << airports[a2].code << " - "
                 << (condition == 1 ? "Bad weather (" + description + ")" : "Clear skies")
                 << endl;
        }
    }

    cout << endl;
    printLine('=');
    cout << "LAUNCHING FLIGHT SIMULATOR" << endl;
    printLine('=');
    cout << "Starting flight simulation for " << ticket.departureAirport << " to " << ticket.arrivalAirport << "..." << endl;

    string command = "flight_simulator.exe " + to_string(src) + " " + to_string(dst);
    cout << "Running: " << command << endl;

    int result = system(command.c_str());

    if (result != 0)
    {
        cout << "\nERROR: Could not launch flight simulator." << endl;
        cout << "This could be due to missing SFML libraries or other dependencies." << endl;
        cout << "\nFlight Summary:" << endl;
        printLine('-');
        cout << "Departure: " << ticket.departureAirport << endl;
        cout << "Arrival: " << ticket.arrivalAirport << endl;
        cout << "Date: " << ticket.departureDate << endl;
        cout << "Time: " << ticket.departureTime << " - " << ticket.arrivalTime << endl;
        cout << "\nThank you for using our booking system!" << endl;
    }

    cout << "\nPress Enter to exit...";
    cin.ignore(1);
    cin.get();
    return 0;
}
