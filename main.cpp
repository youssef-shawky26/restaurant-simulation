#include "raylib.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <random>
#include <string>

using namespace std;

//------------------------------ Customer ------------------------------
struct Customer {
    int customernum;
    int num_persons;
    int arrivalTime;
    int serviceTime;
    int waitingTime;
    bool isReserved;
    bool isTakeaway;

    Customer() {
        customernum = 0;
        num_persons = 1;
        arrivalTime = 0;
        serviceTime = 0;
        waitingTime = 0;
        isReserved = false;
        isTakeaway = false;
    }

    Customer(int num, int persons, int arrival, int service, bool reserved, bool takeaway) {
        customernum = num;
        num_persons = persons;
        arrivalTime = arrival;
        serviceTime = service;
        waitingTime = 0;
        isReserved = reserved;
        isTakeaway = takeaway;
    }
};

//------------------------------ Queues ------------------------------  
class node {
public:
    Customer data;
    node* next;

    node() {
        next = NULL;
    }
};

class Queue_takeaway {
public:
    node* front;
    node* rear;

    Queue_takeaway() {
        front = rear = NULL;
    }

    bool isEmpty() {
        return (front == NULL);
    }

    void enqueue(Customer val) {
        node* newnode = new node;
        newnode->data = val;

        if (isEmpty()) {
            front = rear = newnode;
        }
        else {
            rear->next = newnode;
            rear = newnode;
        }
    }

    Customer dequeue() {
        if (isEmpty()) {
            return Customer();
        }

        node* ptr = front;
        Customer dv = ptr->data;

        if (front == rear) {
            delete front;
            front = rear = NULL;
        }
        else {
            front = front->next;
            delete ptr;
        }

        return dv;
    }

    Customer& Front() {
        return front->data;
    }

    int getLength() {
        int count = 0;
        node* ptr = front;
        while (ptr) {
            count++;
            ptr = ptr->next;
        }
        return count;
    }
};

class Queue_entrance {
public:
    int front, rear, size, length;
    Customer* arr;

    Queue_entrance(int sz) {
        size = sz;
        arr = new Customer[size];
        front = rear = -1;
        length = 0;
    }

    bool isFull() {
        return (length == size);
    }

    bool isEmpty() {
        return (length == 0);
    }

    void enqueue(Customer val) {
        if (!isFull()) {
            rear = (rear + 1) % size;
            arr[rear] = val;
            length++;
        }
    }

    void dequeue() {
        if (!isEmpty()) {
            front = (front + 1) % size;
            length--;
        }
    }

    Customer& Front() {
        int cur = (front + 1) % size;
        return arr[cur];
    }
};

class Queue_reserved {
public:
    int front, rear, size, length;
    Customer* arr;

    Queue_reserved(int size) {
        this->size = size;
        arr = new Customer[size];
        front = rear = -1;
        length = 0;
    }

    bool isFull() {
        return (length == size);
    }

    bool isEmpty() {
        return (length == 0);
    }

    void enqueue(Customer val) {
        if (!isFull()) {
            rear = (rear + 1) % size;
            arr[rear] = val;
            length++;
        }
    }

    void dequeue() {
        if (!isEmpty()) {
            front = (front + 1) % size;
            length--;
        }
    }

    Customer& Front() {
        int cur = (front + 1) % size;
        return arr[cur];
    }
};

//------------------------------ Table ------------------------------
class Server {
public:
    int numCustomer;
    int numOfSeats;
    int TableNo;
    int remainingSeats;
    bool busy;
    Customer party;

    Server() {
        numOfSeats = 4;
        TableNo = 0;
        remainingSeats = 0;
        busy = false;
        numCustomer = 0;
    }

    bool isFull() {
        return busy;
    }

    int getSeats() {
        return numOfSeats;
    }

    void setSeats(int s) {
        numOfSeats = s;
    }
};

//------------------------------ Global Variables ------------------------------
int countCustomers = 0;
int numOfTables;
int dine_in = 0;
int takeawaycust = 0;

Server* table = new Server[6];

Queue_entrance waiting(6);
Queue_takeaway takeaway;
Queue_reserved reservedQueue(5);
vector<Customer> report;

//------------------------------ Random Func ------------------------------
int randomInt(int min, int max) {
    static random_device R;
    static mt19937 gen(R());
    uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}

//------------------------------ When The customer leaves ------------------------------
void exitTable(Server& t) {
    report.push_back(t.party);
    table[t.TableNo - 1].busy = false;
}

//------------------------------ Hostess ------------------------------
class Hostess {
private:
    int lastTableSeated = -1;

public:
    bool findTable(Queue_entrance& q, int partySize) {
        for (int i = 0; i < numOfTables; i++) {
            int checkIndex = (lastTableSeated + 1 + i) % numOfTables; // checks the last table seated and moves on to the next one for the next service to divide the load//

            bool correctSize = (partySize <= 2 && table[checkIndex].getSeats() == 2) ||
                (partySize > 2 && table[checkIndex].getSeats() >= partySize);

            if (!table[checkIndex].busy && correctSize) {
                Customer c = q.Front();
                q.dequeue();

                table[checkIndex].TableNo = checkIndex + 1;
                table[checkIndex].party = c;
                table[checkIndex].remainingSeats = table[checkIndex].numOfSeats - partySize;
                table[checkIndex].busy = true;
                table[checkIndex].numCustomer++;
                dine_in++;

                lastTableSeated = checkIndex;
                return true;
            }
        }
        return false;
    }

    bool findTable(Queue_reserved& q, int partySize) {
        for (int i = 0; i < numOfTables; i++) {
            int checkIndex = (lastTableSeated + 1 + i) % numOfTables; // checks the last table seated and moves on to the next one for the next service to divide the load//

            bool correctSize = (partySize <= 2 && table[checkIndex].getSeats() == 2) ||
                (partySize > 2 && table[checkIndex].getSeats() >= partySize);

            if (!table[checkIndex].busy && correctSize) {
                Customer c = q.Front();
                q.dequeue();

                table[checkIndex].TableNo = checkIndex + 1;
                table[checkIndex].party = c;
                table[checkIndex].remainingSeats = table[checkIndex].numOfSeats - partySize;
                table[checkIndex].busy = true;
                table[checkIndex].numCustomer++;
                dine_in++;

                lastTableSeated = checkIndex;
                return true;
            }
        }
        return false;
    }

    void prioritySeating(Queue_reserved& reserved, Queue_entrance& entrance) {
        while (!reserved.isEmpty()) {
            int partySize = reserved.Front().num_persons;
            bool seated = findTable(reserved, partySize);
            if (!seated) break;
        }
        while (!entrance.isEmpty()) {
            int partySize = entrance.Front().num_persons;
            bool seated = findTable(entrance, partySize);
            if (!seated) break;
        }
    }
};

//------------------------------ Cashier ------------------------------
void cashier(int time, int totaltime) {
    if (!takeaway.isEmpty()) {
        Customer c = takeaway.Front();
        if (time - c.arrivalTime >= c.serviceTime) {
            c.waitingTime = time - c.arrivalTime;
            report.push_back(c);
            takeaway.dequeue();
            takeawaycust++;
        }
    }
}

//------------------------------ Display Functions ------------------------------
void display() {
    cout << "============================================================================================================================" << endl;
    cout << "Total Customers Arrived: " << countCustomers << endl;
    cout << "Total Customers Served: " << report.size() << endl;
    cout << "============================================================================================================================" << endl;

    for (int i = 0; i < report.size(); i++) {
        cout << setfill('0') << setw(2) << report[i].customernum << "\t\t"
            << setw(2) << report[i].num_persons << "\t\t"
            << setw(2) << report[i].arrivalTime << "\t\t"
            << setw(2) << report[i].waitingTime << "\t\t"
            << setw(2) << report[i].serviceTime << "\t\t";

        if (report[i].isTakeaway == 0) {
            cout << "Dine-in" << endl;
        }
        else {
            cout << "Takeaway" << endl;
        }
    }
}

void displayUI() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Simulation Report");
    SetTargetFPS(60);

    int scrollY = 0;

    while (!WindowShouldClose()) {
        scrollY += GetMouseWheelMove() * 25;
        if (scrollY > 0) scrollY = 0;

        int maxScroll = -((int)report.size() * 30 - screenHeight + 150);
        if (maxScroll > 0) maxScroll = 0;
        if (scrollY < maxScroll) scrollY = maxScroll;

        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int i = 0; i < report.size(); i++) {
            int rowY = 130 + (i * 30) + scrollY;
            if (rowY > 100 && rowY < screenHeight + 30) {
                DrawText(TextFormat("%02d", report[i].customernum), 40, rowY, 20, DARKGRAY);
                DrawText(TextFormat("%d", report[i].num_persons), 140, rowY, 20, DARKGRAY);
                DrawText(TextFormat("%d", report[i].arrivalTime), 260, rowY, 20, DARKGRAY);
                DrawText(TextFormat("%d", report[i].waitingTime), 380, rowY, 20, DARKGRAY);
                DrawText(TextFormat("%d", report[i].serviceTime), 500, rowY, 20, DARKGRAY);

                if (report[i].isTakeaway == 0) {
                    DrawText("Dine-in", 620, rowY, 20, DARKGREEN);
                }
                else {
                    DrawText("Takeaway", 620, rowY, 20, DARKPURPLE);
                }

                DrawLine(20, rowY + 25, screenWidth - 20, rowY + 25, DARKGRAY);
            }
        }

        DrawRectangle(0, 0, screenWidth, 120, DARKPURPLE);
        DrawText("FINAL SIMULATION REPORT", 20, 20, 30, WHITE);
        DrawText(TextFormat("Total Arrived: %d", countCustomers), 20, 60, 20, LIGHTGRAY);
        DrawText(TextFormat("Total Served: %d", report.size()), 300, 60, 20, LIGHTGRAY);
        DrawText("Cust #", 30, 95, 20, RAYWHITE);
        DrawText("Persons", 130, 95, 20, RAYWHITE);
        DrawText("Arrival", 250, 95, 20, RAYWHITE);
        DrawText("Wait", 370, 95, 20, RAYWHITE);
        DrawText("Service", 490, 95, 20, RAYWHITE);
        DrawText("Type", 620, 95, 20, RAYWHITE);

        EndDrawing();
    }
    CloseWindow();
}
void displayTableStatsUI() {
    const int screenWidth = 600;
    const int screenHeight = 500;
    InitWindow(screenWidth, screenHeight, "Server Usage Statistics");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRectangle(0, 0, screenWidth, 80, DARKPURPLE);
        DrawText("SERVER USAGE STATISTICS", 20, 25, 30, WHITE);

        DrawText("Table ID", 40, 100, 22, DARKGRAY);
        DrawText("Total Customers", 180, 100, 22, DARKGRAY);
        DrawText("Usage (%)", 400, 100, 22, DARKGRAY);
        DrawLine(20, 130, screenWidth - 20, 130, GRAY);

        for (int i = 0; i < numOfTables; i++) {
            int rowY = 150 + (i * 50);

            float percentage = (dine_in > 0) ? ((float)table[i].numCustomer / dine_in * 100.0f) : 0.0f;

            DrawText(TextFormat("Table %d", i + 1), 40, rowY, 20, BLACK);
            DrawText(TextFormat("%d", table[i].numCustomer), 180, rowY, 20, BLACK);
            DrawText(TextFormat("%.2f%%", percentage), 400, rowY, 20, MAROON);

            DrawRectangle(400, rowY + 25, (int)(percentage * 1.5f), 10, PURPLE);
            DrawRectangleLines(400, rowY + 25, 150, 10, LIGHTGRAY);
        }

        DrawRectangle(0, screenHeight - 60, screenWidth, 60, LIGHTGRAY);
        DrawText(TextFormat("Total Dine-In Customers Served: %d", dine_in), 20, screenHeight - 40, 20, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();
}

//------------------------------ Main ------------------------------
int main() {
    srand(time(0));
    const int screenWidth = 1100;
    const int screenHeight = 700;

    InitWindow(screenWidth, screenHeight, "Data Structures - Queuing Simulation");
    SetTargetFPS(60);

    int simulated_Time = 180;
    int arrival_Rate = 60;
    numOfTables = 6;

    for (int i = 0; i < numOfTables; i++) {
        if (i < 3) table[i].setSeats(2);
        else table[i].setSeats(4);
    }

    Hostess hostess;
    int currentTime = 1;
    int framesCounter = 0;
    int framesPerSimulationTick = 15;

    while (!WindowShouldClose()) {
        if (currentTime < simulated_Time) {
            framesCounter++;
            if (framesCounter >= framesPerSimulationTick) {
                framesCounter = 0;
                if (simulated_Time - currentTime <= 15) {
                    arrival_Rate = 0;
                }
                if (randomInt(1, 100) <= arrival_Rate) {
                    Customer c;
                    countCustomers++;
                    c.customernum = countCustomers;
                    c.num_persons = randomInt(1, 4);
                    c.arrivalTime = currentTime;
                    c.isTakeaway = (randomInt(0, 2) == 2);
                    c.isReserved = (randomInt(1, 100) <= 20);
                    c.serviceTime = randomInt(5, 15);
                    c.waitingTime = 0;

                    if (c.isReserved) {
                        c.isTakeaway = false;
                        reservedQueue.enqueue(c);
                    }
                    else if (c.isTakeaway) {
                        takeaway.enqueue(c);
                    }
                    else {
                        waiting.enqueue(c);
                    }
                }

                for (int i = 0; i < numOfTables; i++) {
                    if (table[i].busy) {
                        if (currentTime - table[i].party.arrivalTime >= table[i].party.serviceTime) {
                            table[i].party.waitingTime = currentTime - (table[i].party.arrivalTime + table[i].party.serviceTime);
                            if (table[i].party.waitingTime < 0) table[i].party.waitingTime = 0;
                            exitTable(table[i]);
                        }
                    }
                }

                hostess.prioritySeating(reservedQueue, waiting);
                cashier(currentTime, simulated_Time);
                currentTime++;
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRectangle(0, 0, screenWidth, 80, DARKPURPLE);
        DrawText(TextFormat("TIME: %d / %d", currentTime, simulated_Time), 30, 25, 30, WHITE);
        DrawText(TextFormat("Total Arrived: %d", countCustomers), 400, 25, 25, LIGHTGRAY);
        DrawText(TextFormat("Total Served: %d", (int)report.size()), 700, 25, 25, LIGHTGRAY);

        if (currentTime >= simulated_Time) {
            DrawText("SIMULATION COMPLETE", 280, 100, 20, RED);
        }

        DrawText("RESTAURANT TABLES", 50, 120, 20, DARKGRAY);

        for (int i = 0; i < numOfTables; i++) {
            int col = i % 3;
            int row = i / 3;
            int tableX = 50 + (col * 160);
            int tableY = 180 + (row * 160);

            Color tableColor = table[i].busy ? MAROON : LIME;

            DrawRectangle(tableX, tableY, 120, 100, tableColor);
            DrawRectangleLines(tableX, tableY, 120, 100, BLACK);

            DrawText(TextFormat("Table %d", i + 1), tableX + 10, tableY + 10, 20, WHITE);
            DrawText(TextFormat("Max Seats: %d", table[i].numOfSeats), tableX + 10, tableY + 35, 10, WHITE);

            if (table[i].busy) {
                DrawText(TextFormat("Size: %d", table[i].party.num_persons), tableX + 10, tableY + 55, 15, RAYWHITE);
                int timeLeft = table[i].party.serviceTime - (currentTime - table[i].party.arrivalTime);
                DrawText(TextFormat("Left: %d", (timeLeft < 0 ? 0 : timeLeft)), tableX + 10, tableY + 75, 15, YELLOW);
            }
            else {
                DrawText("EMPTY", tableX + 35, tableY + 60, 20, DARKGREEN);
            }
        }

        int queueStartX = 580;

        DrawText(TextFormat("Dine-In Waiting Queue (Length: %d)", waiting.length), queueStartX, 180, 20, DARKGRAY);
        DrawRectangleLines(queueStartX, 210, 480, 50, BLACK);
        for (int i = 0; i < waiting.length; i++) {
            DrawCircle(queueStartX + 30 + (i * 45), 235, 15, ORANGE);
            DrawText("C", queueStartX + 25 + (i * 45), 225, 20, WHITE);
        }

        DrawText(TextFormat("Priority Reserved Queue (Length: %d)", reservedQueue.length), queueStartX, 300, 20, DARKGRAY);
        DrawRectangleLines(queueStartX, 330, 480, 50, BLACK);
        for (int i = 0; i < reservedQueue.length; i++) {
            DrawCircle(queueStartX + 30 + (i * 45), 355, 15, BLUE);
            DrawText("R", queueStartX + 25 + (i * 45), 345, 20, WHITE);
        }

        int twLength = takeaway.getLength();
        DrawText(TextFormat("Takeaway Cashier Queue (Length: %d)", twLength), queueStartX, 420, 20, DARKGRAY);
        DrawRectangleLines(queueStartX, 450, 480, 50, BLACK);
        for (int i = 0; i < twLength; i++) {
            DrawCircle(queueStartX + 30 + (i * 45), 475, 15, PURPLE);
            DrawText("T", queueStartX + 25 + (i * 45), 465, 20, WHITE);
        }

        EndDrawing();
    }

    CloseWindow();
    displayUI();
    displayTableStatsUI();
    display();
    cout << "========================================================================================================================" << endl;
    cout << "Dine In Customers: " << dine_in << endl;
    cout << "takeaway Customers: " << takeawaycust << endl;
    cout << "========================================================================================================================" << endl;
    for (int i = 0; i < numOfTables; i++) {
        cout << "table " << i + 1 << " Customers: " << table[i].numCustomer << "{" << (1.0 * table[i].numCustomer / dine_in) * 100 << "%)" << endl;
    }
    cout << "========================================================================================================================" << endl;

    return 0;
}