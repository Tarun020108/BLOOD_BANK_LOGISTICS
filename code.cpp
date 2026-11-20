#include <iostream>
#include <string>
using namespace std;

const int NUM_BANKS = 3;
const int NUM_HOSPITALS = 10; 
const int MAX_LOCATIONS = 13;

const int MAX_UNITS = 100;
const int MAX_REQUESTS = 100;

const int MAX_REQUEST_QTY = 10;
const int VAN_CAPACITY = 25;

const int MAX_BLOOD_GROUPS = 8;
const int INF = 1000000000;

const int HOSPITAL_START = 3;

struct BloodUnit
{
    int id;
    string group;
    int quantity;
};

struct BloodBank
{
    string name;
    BloodUnit units[MAX_UNITS];
    int unitCount;
};

struct Hospital
{
    string name;
};

struct BloodRequest
{
    int requestID;
    int hospitalID;
    string bloodGroup;
    int quantity;
    int urgency;
};

BloodBank banks[NUM_BANKS];
Hospital hospitals[NUM_HOSPITALS];

BloodRequest heap[MAX_REQUESTS];
int heapSize = 0;

int graph[MAX_LOCATIONS][MAX_LOCATIONS];

int dist[MAX_LOCATIONS];
int parent[MAX_LOCATIONS];
bool visited[MAX_LOCATIONS];

int nextUnitID = 1;
int nextRequestID = 1;

string bloodGroups[MAX_BLOOD_GROUPS] =
{
    "O-", "O+", "A-", "A+",
    "B-", "B+", "AB-", "AB+"
};

int getBloodGroupIndex(string group)
{
    for (int i = 0; i < MAX_BLOOD_GROUPS; i++)
    {
        if (bloodGroups[i] == group)
            return i;
    }

    return -1;
}

int getAvailableStock(int bankIndex, string group)
{
    int total = 0;

    for (int i = 0; i < banks[bankIndex].unitCount; i++)
    {
        if (banks[bankIndex].units[i].group == group)
        {
            total += banks[bankIndex].units[i].quantity;
        }
    }

    return total;
}

void addInitialUnit(int bankIndex, string group, int quantity)
{
    if (banks[bankIndex].unitCount >= MAX_UNITS)
        return;

    int i = banks[bankIndex].unitCount;

    banks[bankIndex].units[i].id = nextUnitID++;
    banks[bankIndex].units[i].group = group;
    banks[bankIndex].units[i].quantity = quantity;

    banks[bankIndex].unitCount++;
}

void initializeData()
{
    int initialQuantities[3][8] =
    {
        {5, 7, 4, 6, 5, 7, 4, 6},
        {6, 5, 7, 4, 6, 5, 7, 4},
        {4, 6, 5, 7, 4, 6, 5, 7}
    };

    banks[0].name = "Blood Bank 1";
    banks[1].name = "Blood Bank 2";
    banks[2].name = "Blood Bank 3";

    for (int i = 0; i < NUM_BANKS; i++)
    {
        banks[i].unitCount = 0;

        for (int j = 0; j < MAX_BLOOD_GROUPS; j++)
        {
            addInitialUnit(i,bloodGroups[j],initialQuantities[i][j]);
        }
    }

    for (int i = 0; i < NUM_HOSPITALS; i++)
    {
        hospitals[i].name = "Hospital " + to_string(i + 1);
    }

    for (int i = 0; i < MAX_LOCATIONS; i++)
    {
        for (int j = 0; j < MAX_LOCATIONS; j++)
        {
            graph[i][j] = 0;
        }
    }
    graph[0][1] = graph[1][0] = 7;
    graph[1][2] = graph[2][1] = 8;
    graph[0][2] = graph[2][0] = 12;
    graph[0][3] = graph[3][0] = 4;
    graph[0][4] = graph[4][0] = 6;
    graph[0][5] = graph[5][0] = 8;
    graph[0][6] = graph[6][0] = 10;
    graph[1][6] = graph[6][1] = 5;
    graph[1][7] = graph[7][1] = 4;
    graph[1][8] = graph[8][1] = 6;
    graph[1][9] = graph[9][1] = 8;
    graph[2][9] = graph[9][2] = 5;
    graph[2][10] = graph[10][2] = 4;
    graph[2][11] = graph[11][2] = 6;
    graph[2][12] = graph[12][2] = 7;
    graph[5][6] = graph[6][5] = 3;
    graph[8][9] = graph[9][8] = 3;
    graph[11][12] = graph[12][11] = 2;
}

bool higherPriority(BloodRequest a, BloodRequest b)
{
    if (a.urgency != b.urgency)
        return a.urgency > b.urgency;
    return a.requestID < b.requestID;
}

void heapPush(BloodRequest request)
{
    if (heapSize >= MAX_REQUESTS)
    {
        cout << "Request queue is full!\n";
        return;
    }

    int i = heapSize;

    heap[heapSize] = request;
    heapSize++;

    while (i > 0)
    {
        int parentIndex = (i - 1) / 2;

        if (higherPriority(heap[i], heap[parentIndex]))
        {
            BloodRequest temp = heap[i];
            heap[i] = heap[parentIndex];
            heap[parentIndex] = temp;

            i = parentIndex;
        }
        else
        {
            break;
        }
    }
}

BloodRequest heapPop()
{
    BloodRequest result = heap[0];

    heap[0] = heap[heapSize - 1];
    heapSize--;

    int i = 0;

    while (true)
    {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int largest = i;

        if (left < heapSize &&
            higherPriority(heap[left], heap[largest]))
        {
            largest = left;
        }

        if (right < heapSize &&
            higherPriority(heap[right], heap[largest]))
        {
            largest = right;
        }

        if (largest != i)
        {
            BloodRequest temp = heap[i];
            heap[i] = heap[largest];
            heap[largest] = temp;

            i = largest;
        }
        else
        {
            break;
        }
    }

    return result;
}

void submitRequest()
{
    if (heapSize >= MAX_REQUESTS)
    {
        cout << "Request queue is full!\n";
        return;
    }

    BloodRequest request;

    cout << "\nEnter Hospital Number (1-10): ";
    cin >> request.hospitalID;

    if (request.hospitalID < 1 ||
        request.hospitalID > NUM_HOSPITALS)
    {
        cout << "Invalid hospital number!\n";
        return;
    }

    request.hospitalID--;

    cout << "Enter Blood Group: ";
    cin >> request.bloodGroup;

    if (getBloodGroupIndex(request.bloodGroup) == -1)
    {
        cout << "Invalid blood group!\n";
        return;
    }

    cout << "Enter Required Quantity: ";
    cin >> request.quantity;

    if (request.quantity <= 0 ||
        request.quantity > MAX_REQUEST_QTY)
    {
        cout << "Quantity must be between 1 and "
             << MAX_REQUEST_QTY << ".\n";
        return;
    }

    cout << "\nSelect Urgency:\n";
    cout << "1. Normal\n";
    cout << "2. Urgent\n";
    cout << "3. Emergency\n";
    cout << "4. Critical\n";

    cout << "Enter choice: ";
    cin >> request.urgency;

    if (request.urgency < 1 || request.urgency > 4)
    {
        cout << "Invalid urgency!\n";
        return;
    }

    request.requestID = nextRequestID++;

    heapPush(request);

    cout << "\nRequest submitted successfully!\n";
    cout << "Request ID: " << request.requestID << "\n";
}

void displayPendingRequests()
{
    if (heapSize == 0)
    {
        cout << "\nNo pending requests.\n";
        return;
    }

    cout << "\n========== PENDING REQUESTS ==========\n";

    for (int i = 0; i < heapSize; i++)
    {
        cout << "\nRequest ID: "
             << heap[i].requestID;

        cout << "\nHospital: "
             << hospitals[heap[i].hospitalID].name;

        cout << "\nBlood Group: "
             << heap[i].bloodGroup;

        cout << "\nQuantity: "
             << heap[i].quantity;

        cout << "\nUrgency: ";

        if (heap[i].urgency == 4)
            cout << "Critical";
        else if (heap[i].urgency == 3)
            cout << "Emergency";
        else if (heap[i].urgency == 2)
            cout << "Urgent";
        else
            cout << "Normal";

        cout << "\nPriority Value: "
             << heap[i].urgency * heap[i].quantity;

        cout << "\n-----------------------------\n";
    }
}
void dijkstra(int source)
{
    for (int i = 0; i < MAX_LOCATIONS; i++)
    {
        dist[i] = INF;
        parent[i] = -1;
        visited[i] = false;
    }

    dist[source] = 0;

    for (int count = 0; count < MAX_LOCATIONS; count++)
    {
        int u = -1;
        int minimum = INF;
        for (int i = 0; i < MAX_LOCATIONS; i++)
        {
            if (!visited[i] && dist[i] < minimum)
            {
                minimum = dist[i];
                u = i;
            }
        }

        if (u == -1)
            break;

        visited[u] = true;
        for (int v = 0; v < MAX_LOCATIONS; v++)
        {
            if (graph[u][v] > 0 &&
                !visited[v] &&
                dist[u] + graph[u][v] < dist[v])
            {
                dist[v] = dist[u] + graph[u][v];
                parent[v] = u;
            }
        }
    }
}

void displayRoute(int source, int destination)
{
    int path[MAX_LOCATIONS];
    int pathSize = 0;

    int current = destination;

    while (current != -1 && pathSize < MAX_LOCATIONS)
    {
        path[pathSize++] = current;

        if (current == source)
            break;

        current = parent[current];
    }

    if (pathSize == 0 || path[pathSize - 1] != source)
    {
        cout << "Route not found!\n";
        return;
    }

    cout << "Shortest Route: ";

    for (int i = pathSize - 1; i >= 0; i--)
    {
        int vertex = path[i];

        if (vertex < NUM_BANKS)
            cout << "BB" << vertex + 1;
        else
            cout << "H" << vertex - NUM_BANKS + 1;

        if (i > 0)
            cout << " -> ";
    }

    cout << "\nShortest Distance: " << dist[destination] << " km\n";
}

int findNearestBank(BloodRequest request)
{
    int hospitalVertex = HOSPITAL_START + request.hospitalID;
    dijkstra(hospitalVertex);

    int bankOrder[NUM_BANKS];

    for (int i = 0; i < NUM_BANKS; i++)
    {
        bankOrder[i] = i;
    }

    for (int i = 0; i < NUM_BANKS - 1; i++)
    {
        for (int j = i + 1; j < NUM_BANKS; j++)
        {
            if (dist[bankOrder[j]] < dist[bankOrder[i]])
            {
                int temp = bankOrder[i];
                bankOrder[i] = bankOrder[j];
                bankOrder[j] = temp;
            }
        }
    }

    cout << "\nChecking banks for highest-priority request:\n";

    for (int i = 0; i < NUM_BANKS; i++)
    {
        int bankIndex = bankOrder[i];

        int available = getAvailableStock(
            bankIndex,
            request.bloodGroup
        );

        cout << banks[bankIndex].name << " - Distance: " << dist[bankIndex] << " km - Available: " << available;

        if (available >= request.quantity &&
            dist[bankIndex] != INF)
        {
            cout << " - Selected.\n";
            return bankIndex;
        }

        cout << " - Insufficient stock.\n";
    }

    return -1;
}

void sortRequests(BloodRequest requests[], int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (higherPriority(requests[j], requests[i]))
            {
                BloodRequest temp = requests[i];
                requests[i] = requests[j];
                requests[j] = temp;
            }
        }
    }
}

void knapsack(
    BloodRequest eligible[],
    int n,
    int selected[],
    int &selectedCount
)
{
    int dp[MAX_REQUESTS + 1][VAN_CAPACITY + 1];

    bool take[MAX_REQUESTS + 1][VAN_CAPACITY + 1];

    for (int i = 0; i <= n; i++)
    {
        for (int w = 0; w <= VAN_CAPACITY; w++)
        {
            dp[i][w] = 0;
            take[i][w] = false;
        }
    }

    for (int i = 1; i <= n; i++)
    {
        int weight = eligible[i - 1].quantity;

        int value =
            eligible[i - 1].urgency *
            eligible[i - 1].quantity;

        for (int w = 0; w <= VAN_CAPACITY; w++)
        {
            dp[i][w] = dp[i - 1][w];
        
            if (weight <= w)
            {
                int newValue =
                    dp[i - 1][w - weight] + value;

                if (newValue > dp[i][w])
                {
                    dp[i][w] = newValue;
                    take[i][w] = true;
                }
            }
        }
    }
    selectedCount = 0;

    int w = VAN_CAPACITY;

    for (int i = n; i >= 1; i--)
    {
        if (take[i][w])
        {
            selected[selectedCount] = i - 1;
            selectedCount++;

            w -= eligible[i - 1].quantity;
        }
    }
}
int getPriorityValue(BloodRequest request)
{
    return request.urgency * request.quantity;
}

void sortSelectedRequests(
    BloodRequest selectedRequests[],
    int count
)
{
    for (int i = 0; i < count - 1; i++)
    {
        for (int j = i + 1; j < count; j++)
        {
            int value1 = getPriorityValue(selectedRequests[i]);
            int value2 = getPriorityValue(selectedRequests[j]);

            bool shouldSwap = false;

            if (value2 > value1)
            {
                shouldSwap = true;
            }
            else if (value2 == value1 && selectedRequests[j].urgency > selectedRequests[i].urgency)
            {
                shouldSwap = true;
            }
            else if (value2 == value1 && selectedRequests[j].urgency ==selectedRequests[i].urgency && selectedRequests[j].requestID < selectedRequests[i].requestID)
            {
                shouldSwap = true;
            }

            if (shouldSwap)
            {
                BloodRequest temp = selectedRequests[i];
                selectedRequests[i] = selectedRequests[j];
                selectedRequests[j] = temp;
            }
        }
    }
}

void deductInventory(int bankIndex, string group, int quantity)
{
    for (int i = 0; i < banks[bankIndex].unitCount; i++)
    {
        if (banks[bankIndex].units[i].group == group && banks[bankIndex].units[i].quantity > 0)
        {
            int available = banks[bankIndex].units[i].quantity;

            int deduct;

            if (available >= quantity)
                deduct = quantity;
            else
                deduct = available;

            banks[bankIndex].units[i].quantity -= deduct;

            quantity -= deduct;

            if (quantity == 0)
                return;
        }
    }
}

bool isSelected(
    int requestID,
    BloodRequest selectedRequests[],
    int count
)
{
    for (int i = 0; i < count; i++)
    {
        if (selectedRequests[i].requestID == requestID)
            return true;
    }

    return false;
}

void processRequest()
{
    if (heapSize == 0)
    {
        cout << "\nNo pending requests.\n";
        return;
    }

    BloodRequest pending[MAX_REQUESTS];

    int pendingCount = heapSize;

    for (int i = 0; i < heapSize; i++)
    {
        pending[i] = heap[i];
    }

    sortRequests(pending, pendingCount);

    BloodRequest topRequest = pending[0];

    cout << "\n========== NEW VAN TRIP ==========\n";

    cout << "Highest-priority request:\n";

    cout << "Request ID: "
         << topRequest.requestID << "\n";

    cout << "Hospital: "
         << hospitals[topRequest.hospitalID].name << "\n";

    cout << "Blood Group: "
         << topRequest.bloodGroup << "\n";

    cout << "Quantity: "
         << topRequest.quantity << "\n";

    int bankIndex = findNearestBank(topRequest);

    if (bankIndex == -1)
    {
        cout << "\nNo blood bank can fulfil the highest-priority request.\n";
        cout << "All requests remain pending.\n";
        return;
    }

    cout << "\nSelected Blood Bank: " << banks[bankIndex].name << "\n";

    BloodRequest eligible[MAX_REQUESTS];

    int eligibleCount = 0;

    int reserved[MAX_BLOOD_GROUPS] = {0};
    int stock[MAX_BLOOD_GROUPS];

    for (int i = 0; i < MAX_BLOOD_GROUPS; i++)
    {
        stock[i] = getAvailableStock(bankIndex, bloodGroups[i]);
    }

    cout << "\nChecking other pending requests:\n";

    for (int i = 0; i < pendingCount; i++)
    {
        int groupIndex = getBloodGroupIndex(pending[i].bloodGroup);

        if (groupIndex == -1)
            continue;

        int quantity = pending[i].quantity;

        if (quantity > VAN_CAPACITY)
            continue;
        if (reserved[groupIndex] + quantity <= stock[groupIndex])
        {
            eligible[eligibleCount] = pending[i];
            eligibleCount++;

            reserved[groupIndex] += quantity;

            cout << "Request " << pending[i].requestID
                 << " is eligible.\n";
        }
        else
        {
            cout << "Request " << pending[i].requestID << " excluded: insufficient combined stock.\n";
        }
    }

    if (eligibleCount == 0)
    {
        cout << "\nNo eligible requests for this trip.\n";
        return;
    }
    int selected[MAX_REQUESTS];
    int selectedCount = 0;

    knapsack(
        eligible,
        eligibleCount,
        selected,
        selectedCount
    );

    BloodRequest selectedRequests[MAX_REQUESTS];

    int totalUnits = 0;
    int totalPriority = 0;

    cout << "\n========== KNAPSACK RESULT ==========\n";

    cout << "Van Capacity: "
         << VAN_CAPACITY << " units\n";

    for (int i = 0; i < selectedCount; i++)
    {
        selectedRequests[i] = eligible[selected[i]];
    }

    sortSelectedRequests(selectedRequests, selectedCount);

    cout << "\nSelected hospital requests:\n";

    for (int i = 0; i < selectedCount; i++)
    {
        BloodRequest request = selectedRequests[i];

        int value = getPriorityValue(request);

        totalUnits += request.quantity;
        totalPriority += value;

        cout << "\nRequest ID: "
             << request.requestID;

        cout << "\nHospital: "
             << hospitals[request.hospitalID].name;

        cout << "\nBlood Group: "
             << request.bloodGroup;

        cout << "\nQuantity: "
             << request.quantity;

        cout << "\nUrgency: ";

        if (request.urgency == 4)
            cout << "Critical";
        else if (request.urgency == 3)
            cout << "Emergency";
        else if (request.urgency == 2)
            cout << "Urgent";
        else
            cout << "Normal";

        cout << "\nPriority Value: " << value;

        cout << "\n-----------------------------\n";
    }

    cout << "\nTotal Units Loaded: "
         << totalUnits << "/" << VAN_CAPACITY;

    cout << "\nTotal Priority Value: "
         << totalPriority << "\n";


    cout << "\n========== DISPATCH DETAILS ==========\n";

    int currentVertex = bankIndex;

    for (int i = 0; i < selectedCount; i++)
    {
        BloodRequest request = selectedRequests[i];

        int hospitalVertex =
            HOSPITAL_START + request.hospitalID;

        cout << "\n================================\n";

        cout << "Next Destination: "
             << hospitals[request.hospitalID].name << "\n";

        cout << "Priority Value: "
             << getPriorityValue(request) << "\n";

        cout << "Blood Group: "
             << request.bloodGroup << "\n";

        cout << "Quantity: "
             << request.quantity << "\n";

        dijkstra(currentVertex);

        displayRoute(currentVertex, hospitalVertex);

        deductInventory(
            bankIndex,
            request.bloodGroup,
            request.quantity
        );

        currentVertex = hospitalVertex;
    }
    heapSize = 0;

    for (int i = 0; i < pendingCount; i++)
    {
        if (!isSelected(
                pending[i].requestID,
                selectedRequests,
                selectedCount))
        {
            heapPush(pending[i]);
        }
    }

    cout << "\n========== TRIP COMPLETED ==========\n";

    cout << "Blood Bank: "
         << banks[bankIndex].name << "\n";

    cout << "Total Units Dispatched: "
         << totalUnits << "\n";

    cout << "Total Requests Fulfilled: "
         << selectedCount << "\n";

    cout << "Remaining Pending Requests: "
         << heapSize << "\n";
}

void displayInventory()
{
    cout << "\n========== BLOOD INVENTORY ==========\n";

    for (int i = 0; i < NUM_BANKS; i++)
    {
        cout << "\n" << banks[i].name << "\n";

        cout << "--------------------------------------\n";

        cout << "Record ID\tBlood Group\tQuantity\n";

        for (int j = 0; j < banks[i].unitCount; j++)
        {
            cout << banks[i].units[j].id << "\t\t";

            cout << banks[i].units[j].group << "\t\t";

            cout << banks[i].units[j].quantity << "\n";
        }
    }
}

void updateBloodUnit()
{
    int bankNumber;
    int recordID;
    int newQuantity;

    cout << "\nSelect Blood Bank (1-3): ";
    cin >> bankNumber;

    if (bankNumber < 1 || bankNumber > NUM_BANKS)
    {
        cout << "Invalid blood bank number!\n";
        return;
    }

    int bankIndex = bankNumber - 1;

    cout << "\nInventory Records:\n";

    for (int i = 0; i < banks[bankIndex].unitCount; i++)
    {
        cout << "ID: "
             << banks[bankIndex].units[i].id;

        cout << " | Group: "
             << banks[bankIndex].units[i].group;

        cout << " | Quantity: "
             << banks[bankIndex].units[i].quantity << "\n";
    }

    cout << "\nEnter Record ID to update: ";
    cin >> recordID;

    int recordIndex = -1;

    for (int i = 0; i < banks[bankIndex].unitCount; i++)
    {
        if (banks[bankIndex].units[i].id == recordID)
        {
            recordIndex = i;
            break;
        }
    }

    if (recordIndex == -1)
    {
        cout << "Record ID not found!\n";
        return;
    }

    cout << "Enter New Quantity: ";
    cin >> newQuantity;

    if (newQuantity < 0)
    {
        cout << "Quantity cannot be negative!\n";
        return;
    }

    banks[bankIndex].units[recordIndex].quantity = newQuantity;

    cout << "\nBlood quantity updated successfully!\n";
}

void displayRoadNetwork()
{
    cout << "\n========== ROAD NETWORK ==========\n";

    cout << "All distances are in kilometres.\n\n";

    for (int i = 0; i < MAX_LOCATIONS; i++)
    {
        for (int j = i + 1; j < MAX_LOCATIONS; j++)
        {
            if (graph[i][j] > 0)
            {
                cout << "Vertex ";

                if (i < NUM_BANKS)
                    cout << "BB" << i + 1;
                else
                    cout << "H" << i - NUM_BANKS + 1;

                cout << " <--> ";

                if (j < NUM_BANKS)
                    cout << "BB" << j + 1;
                else
                    cout << "H" << j - NUM_BANKS + 1;

                cout << " : " << graph[i][j] << " km\n";
            }
        }
    }
}

void pauseScreen()
{
    cout << "\n\nPress Enter to continue...";

    cin.ignore(10000, '\n');
    cin.get();

    cout << "\n\n";
}

int main()
{
    initializeData();

    int choice;

    do
    {
        cout << "\n========================================\n";
        cout << "   BLOOD-BANK LOGISTICS MANAGEMENT SYSTEM\n";
        cout << "             THIRUVARUR CITY\n";
        cout << "========================================\n";

        cout << "Van Capacity: "
             << VAN_CAPACITY << " units\n\n";

        cout << "1. Submit Hospital Request\n";
        cout << "2. View Pending Requests\n";
        cout << "3. Process Van Trip (Knapsack Allocation)\n";
        cout << "4. View Blood Inventory\n";
        cout << "5. Update Blood Quantity\n";
        cout << "6. Display Road Network\n";
        cout << "7. Exit\n";

        cout << "\nEnter your choice: ";
        cin >> choice;

        switch (choice)
        {
            case 1:
                submitRequest();
                break;

            case 2:
                displayPendingRequests();
                break;

            case 3:
                processRequest();
                break;

            case 4:
                displayInventory();
                break;

            case 5:
                updateBloodUnit();
                break;

            case 6:
                displayRoadNetwork();
                break;

            case 7:
                cout << "\nExiting Blood-Bank Logistics System...\n";
                break;

            default:
                cout << "\nInvalid choice! Please try again.\n";
        }

        if (choice != 7)
        {
            pauseScreen();
        }

    } while (choice != 7);

    return 0;
}
