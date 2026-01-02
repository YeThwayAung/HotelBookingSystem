#include "HotelBookingSystem.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <limits>

HotelBookingSystem::HotelBookingSystem() {
    loadAdminsFromFile();
    loadRoomsFromFile();
}

HotelBookingSystem::~HotelBookingSystem() {
    for (Room* r : rooms) delete r;
}

void HotelBookingSystem::clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void HotelBookingSystem::loadAdminsFromFile() {
    ifstream file(adminFile);
    if (!file.is_open()) return;
    string line;
    while (getline(file, line)) {
        size_t pos = line.find(',');
        if (pos == string::npos) continue;
        string user = line.substr(0, pos);
        string pass = line.substr(pos + 1);
        admins.emplace_back(user, pass);
    }
    file.close();
}

bool HotelBookingSystem::requireAdmin() const {
    if (!adminLoggedIn) {
        cout << "\nAccess denied. Admin only.\n";
        return false;
    }
    return true;
}

bool HotelBookingSystem::roomNumberExists(const string& roomNum) const {
    for (Room* r : rooms) {
        if (r->getRoomNumber() == roomNum) {
            return true;
        }
    }
    return false;
}

void HotelBookingSystem::printDetailedBill(Room* room) {
    cout << "\n" << string(50, '=') << endl;
    cout << "         HOTEL BOOKING SYSTEM - INVOICE" << endl;
    cout << string(50, '=') << endl;
    cout << "\nRoom Number: " << room->getRoomNumber() << endl;
    cout << "Room Type: " << room->getRoomType() << endl;
    cout << "Guest Name: " << room->getGuestName() << endl;
    cout << string(50, '-') << endl;
    
    int nights = room->getNightsBooked();
    double pricePerNight = room->getPricePerNight();
    double subtotal = pricePerNight * nights;
    double discount = room->getDiscount(nights);
    double tax = room->getTax(nights);
    double total = room->calculateBill(nights);
    
    cout << fixed << setprecision(2);
    cout << "\nNumber of Nights:            " << nights << endl;
    cout << "Rate per Night:              $" << pricePerNight << endl;
    cout << "                             " << string(15, '-') << endl;
    cout << "Subtotal:                    $" << subtotal << endl;
    
    if (discount > 0) {
        cout << "Discount (10% - 3+ nights):  -$" << discount << endl;
    }
    if (tax > 0) {
        cout << "Luxury Tax (15%):            +$" << tax << endl;
    }
    
    cout << "                             " << string(15, '=') << endl;
    cout << "TOTAL AMOUNT DUE:            $" << total << endl;
    cout << string(50, '=') << endl;
    cout << "\nThank you for choosing our hotel!\n";
}

void HotelBookingSystem::adminAddRoom() {
    if (!requireAdmin()) return;
    string type, num;
    double price = 0.0;

    cout << "Room Number: "; cin >> num;
    
    if (roomNumberExists(num)) {
        cout << "\nError: A room with number '" << num << "' already exists!\n";
        cout << "Please use a different room number.\n";
        return;
    }
    
    cout << "Choose Room Type:\n";
    cout << "1. Deluxe (default $150)\n";
    cout << "2. Suite (default $300)\n";
    cout << "3. New/Custom Room Type (set price manually)\n";
    cout << "Enter choice (1-3): ";
    int choice; cin >> choice;
    cin.ignore();

    if (choice == 1) {
        rooms.push_back(new DeluxeRoom(num));
    } else if (choice == 2) {
        rooms.push_back(new Suite(num));
    } else if (choice == 3) {
        cout << "Enter Room Type Name: ";
        getline(cin, type);
        cout << "Enter Price for this Room Type: ";
        cin >> price;
        cin.ignore();
        cout << "Enter Special Amenity/Feature (optional): ";
        string special;
        getline(cin, special);

        Room* newRoom = new Room(num, type, price);
        newRoom->setSpecialAmenity(special);
        rooms.push_back(newRoom);
    } else {
        cout << "Invalid choice.\n";
        return;
    }

    saveRoomsToFile();
    cout << "Room added successfully.\n";
}

void HotelBookingSystem::adminRemoveRoom() {
    if (!requireAdmin()) return;
    string roomNum; cout << "Enter Room Number: "; cin >> roomNum;
    for (auto it = rooms.begin(); it != rooms.end(); ++it) {
        if ((*it)->getRoomNumber() == roomNum) {
            delete *it; rooms.erase(it);
            saveRoomsToFile(); cout << "Room removed successfully.\n";
            return;
        }
    }
    cout << "Room not found.\n";
}

void HotelBookingSystem::adminChangeRoomPrice() {
    if (!requireAdmin()) return;
    string roomNum; double price;
    cout << "Enter Room Number: "; cin >> roomNum;
    cout << "Enter New Price: "; cin >> price;
    for (Room* r : rooms) {
        if (r->getRoomNumber() == roomNum) {
            r->setPrice(price); saveRoomsToFile();
            cout << "Price updated successfully.\n"; return;
        }
    }
    cout << "Room not found.\n";
}

void HotelBookingSystem::adminForceCheckout() {
    if (!requireAdmin()) return;
    string roomNum; cout << "Enter Room Number: "; cin >> roomNum;
    for (Room* r : rooms) {
        if (r->getRoomNumber() == roomNum) {
            r->forceCheckOut(); saveRoomsToFile();
            cout << "Guest forcibly checked out.\n"; return;
        }
    }
    cout << "Room not found.\n";
}

void HotelBookingSystem::adminViewAllBookings() {
    if (!requireAdmin()) return;
    cout << "\n========== ALL CURRENT BOOKINGS ==========\n";
    bool foundBooking = false;
    for (Room* r : rooms) {
        if (!r->getAvailability()) {
            foundBooking = true;
            cout << "\nRoom: " << r->getRoomNumber() 
                 << " | Type: " << r->getRoomType()
                 << " | Guest: " << r->getGuestName()
                 << " | Nights: " << r->getNightsBooked()
                 << " | Total Bill: $" << fixed << setprecision(2) 
                 << r->calculateBill(r->getNightsBooked()) << endl;
        }
    }
    if (!foundBooking) cout << "\nNo active bookings.\n";
}

void HotelBookingSystem::adminViewStatistics() {
    if (!requireAdmin()) return;
    int totalRooms = rooms.size();
    int occupied = 0;
    double totalRevenue = 0.0;
    
    for (Room* r : rooms) {
        if (!r->getAvailability()) {
            occupied++;
            totalRevenue += r->calculateBill(r->getNightsBooked());
        }
    }
    
    double occupancyRate = (totalRooms > 0) ? (occupied * 100.0 / totalRooms) : 0.0;
    
    cout << "\n========== HOTEL STATISTICS ==========\n";
    cout << "Total Rooms: " << totalRooms << endl;
    cout << "Occupied Rooms: " << occupied << endl;
    cout << "Available Rooms: " << (totalRooms - occupied) << endl;
    cout << "Occupancy Rate: " << fixed << setprecision(1) << occupancyRate << "%" << endl;
    cout << "Current Revenue: $" << fixed << setprecision(2) << totalRevenue << endl;
    cout << "======================================\n";
}

void HotelBookingSystem::adminMenu() {
    int choice;
    while (adminLoggedIn) {
        cout << "\n========== ADMIN MENU ==========\n";
        cout << "1. Add Room\n";
        cout << "2. Remove Room\n";
        cout << "3. Change Room Price\n";
        cout << "4. Force Check Out Guest\n";
        cout << "5. View All Bookings\n";
        cout << "6. View Hotel Statistics\n";
        cout << "7. Logout\n";
        cout << "Choice: ";
        cin >> choice;
        if (cin.fail()) { clearInputBuffer(); cout << "Invalid input.\n"; continue; }
        switch(choice) {
            case 1: adminAddRoom(); break;
            case 2: adminRemoveRoom(); break;
            case 3: adminChangeRoomPrice(); break;
            case 4: adminForceCheckout(); break;
            case 5: adminViewAllBookings(); break;
            case 6: adminViewStatistics(); break;
            case 7: adminLoggedIn = false; break;
            default: cout << "Invalid choice.\n";
        }
    }
}

void HotelBookingSystem::initializeDefaultRooms() {
    rooms.push_back(new DeluxeRoom("DR101", 150.0));
    rooms.push_back(new DeluxeRoom("DR102", 150.0));
    rooms.push_back(new DeluxeRoom("DR103", 150.0));
    rooms.push_back(new DeluxeRoom("DR104", 175.0));
    rooms.push_back(new DeluxeRoom("DR105", 175.0));
    rooms.push_back(new Suite("ST201", 300.0));
    rooms.push_back(new Suite("ST202", 300.0));
    rooms.push_back(new Suite("ST203", 350.0, true, true, 3));
    rooms.push_back(new Suite("ST204", 350.0, true, true, 3));
}

void HotelBookingSystem::loadRoomsFromFile() {
    rooms.clear();
    ifstream file(filename);
    if (!file.is_open()) { initializeDefaultRooms(); return; }

    string line;
    while (getline(file, line)) {
        vector<string> tokens; string token; size_t pos;
        while ((pos = line.find(',')) != string::npos) {
            token = line.substr(0, pos); tokens.push_back(token);
            line.erase(0, pos + 1);
        }
        tokens.push_back(line);

        if (tokens.size() >= 6) {
            string roomNum = tokens[0];
            string roomType = tokens[1];
            double price = stod(tokens[2]);
            bool available = (tokens[3] == "1");
            string guest = tokens[4];
            int nights = stoi(tokens[5]);
            string specialAmenity = "";
            if(tokens.size() >= 7) specialAmenity = tokens[6];

            Room* room;
            if (roomType == "Deluxe Room") room = new DeluxeRoom(roomNum, price, available);
            else if (roomType == "Suite") room = new Suite(roomNum, price, available);
            else room = new Room(roomNum, roomType, price, available);

            room->setSpecialAmenity(specialAmenity);

            if (!available) room->bookRoom(guest, nights);
            rooms.push_back(room);
        }
    }

    if (rooms.empty()) initializeDefaultRooms();
    file.close();
}

void HotelBookingSystem::saveRoomsToFile() {
    ofstream file(filename);
    for (Room* r : rooms) file << r->toFileString() << endl;
    file.close();
}

void HotelBookingSystem::displayMenu() {
    cout << "\n*****************************************\n";
    cout << "|    HOTEL BOOKING SYSTEM - DEMO        |\n";
    cout << "*****************************************\n";
    cout << "\n1. View Available Rooms\n";
    cout << "2. Book a Room\n";
    cout << "3. Calculate Bill\n";
    cout << "4. Display Room Details\n";
    cout << "5. Check Out Guest\n";
    cout << "6. Search Rooms\n";
    cout << "7. Extend Booking\n";
    cout << "8. Exit\n";
    cout << "9. Admin Login\n";
    cout << "\nEnter your choice (1-9): ";
}

void HotelBookingSystem::run() {
    int choice;
    while(true) {
        displayMenu();
        cin >> choice;
        if(cin.fail()) { clearInputBuffer(); cout << "Invalid input.\n"; continue; }

        switch(choice) {
            case 1: viewAvailableRooms(); break;
            case 2: bookRoom(); break;
            case 3: calculateBill(); break;
            case 4: displayRoomDetails(); break;
            case 5: checkOutGuest(); break;
            case 6: searchRooms(); break;
            case 7: extendBooking(); break;
            case 8: saveRoomsToFile(); return;
            case 9: adminLogin(); break;
            default: cout<<"Invalid choice.\n";
        }
    }
}

void HotelBookingSystem::viewAvailableRooms() {
    cout << "\n========== AVAILABLE ROOMS ==========\n";
    bool foundAvailable = false;
    for (Room* room : rooms) {
        if (room->getAvailability()) {
            foundAvailable = true;
            room->displayRoomDetails();
        }
    }
    if (!foundAvailable) cout << "\nSorry, no rooms are currently available.\n";
}

void HotelBookingSystem::bookRoom() {
    string roomNum, guestName;
    int nights;
    cout << "\nEnter Room Number: ";
    cin >> roomNum;
    cin.ignore();
    Room* selectedRoom = nullptr;
    for (Room* room : rooms) if (room->getRoomNumber() == roomNum) { selectedRoom = room; break; }
    if (!selectedRoom) { cout << "Room not found.\n"; return; }
    if (!selectedRoom->getAvailability()) { cout << "Room unavailable.\n"; return; }
    cout << "Enter Guest Name: "; getline(cin, guestName);
    cout << "Enter Number of Nights: "; cin >> nights;
    if (selectedRoom->bookRoom(guestName, nights)) {
        cout << "\n" << string(50, '=') << endl;
        cout << "         BOOKING CONFIRMED!" << endl;
        cout << string(50, '=') << endl;
        printDetailedBill(selectedRoom);
        saveRoomsToFile();
    } else cout << "Booking failed.\n";
}

void HotelBookingSystem::calculateBill() {
    string roomNum;
    cout << "Enter Room Number: "; cin >> roomNum;
    for (Room* room : rooms) {
        if (room->getRoomNumber() == roomNum) {
            if (room->getAvailability()) { cout << "Room is not booked.\n"; return; }
            printDetailedBill(room);
            return;
        }
    }
    cout << "Room not found.\n";
}

void HotelBookingSystem::displayRoomDetails() {
    string roomNum;
    cout << "Enter Room Number (or ALL): "; cin >> roomNum;
    if (roomNum == "ALL" || roomNum == "all") for (Room* r : rooms) r->displayRoomDetails();
    else {
        bool found = false;
        for (Room* r : rooms) {
            if (r->getRoomNumber() == roomNum) {
                r->displayRoomDetails();
                found = true;
                break;
            }
        }
        if(!found) cout << "Room not found.\n";
    }
}

void HotelBookingSystem::checkOutGuest() {
    string roomNum;
    cout << "Enter Room Number: "; cin >> roomNum;
    for (Room* r : rooms) {
        if (r->getRoomNumber() == roomNum) {
            if (r->getAvailability()) { cout << "Room is not occupied.\n"; return; }
            cout << "\n========== CHECK OUT ==========\n";
            printDetailedBill(r);
            r->checkOut();
            saveRoomsToFile();
            cout << "\nCheck out completed successfully!\n";
            return;
        }
    }
    cout << "Room not found.\n";
}

void HotelBookingSystem::searchRooms() {
    cout << "\nSearch by:\n";
    cout << "1. Room Type\n";
    cout << "2. Price Range\n";
    cout << "3. Availability Status\n";
    cout << "Choice: ";
    int choice; cin >> choice;
    cin.ignore();
    
    if (choice == 1) {
        string type;
        cout << "Enter Room Type: "; getline(cin, type);
        cout << "\n========== SEARCH RESULTS ==========\n";
        bool found = false;
        for (Room* r : rooms) {
            if (r->getRoomType().find(type) != string::npos) {
                r->displayRoomDetails();
                found = true;
            }
        }
        if (!found) cout << "No rooms found matching that type.\n";
    } else if (choice == 2) {
        double minPrice, maxPrice;
        cout << "Enter Minimum Price: "; cin >> minPrice;
        cout << "Enter Maximum Price: "; cin >> maxPrice;
        cout << "\n========== SEARCH RESULTS ==========\n";
        bool found = false;
        for (Room* r : rooms) {
            if (r->getPricePerNight() >= minPrice && r->getPricePerNight() <= maxPrice) {
                r->displayRoomDetails();
                found = true;
            }
        }
        if (!found) cout << "No rooms found in that price range.\n";
    } else if (choice == 3) {
        cout << "1. Available Only\n2. Occupied Only\nChoice: ";
        int statusChoice; cin >> statusChoice;
        cout << "\n========== SEARCH RESULTS ==========\n";
        bool found = false;
        for (Room* r : rooms) {
            if ((statusChoice == 1 && r->getAvailability()) || 
                (statusChoice == 2 && !r->getAvailability())) {
                r->displayRoomDetails();
                found = true;
            }
        }
        if (!found) cout << "No rooms found with that status.\n";
    }
}

void HotelBookingSystem::extendBooking() {
    string roomNum;
    int additionalNights;
    cout << "Enter Room Number: "; cin >> roomNum;
    for (Room* r : rooms) {
        if (r->getRoomNumber() == roomNum) {
            if (r->getAvailability()) { cout << "Room is not currently booked.\n"; return; }
            cout << "Current nights booked: " << r->getNightsBooked() << endl;
            cout << "Enter additional nights: "; cin >> additionalNights;
            r->setNightsBooked(r->getNightsBooked() + additionalNights);
            saveRoomsToFile();
            cout << "\nBooking extended successfully!\n";
            cout << "New total nights: " << r->getNightsBooked() << endl;
            cout << "New total bill: $" << fixed << setprecision(2) 
                 << r->calculateBill(r->getNightsBooked()) << endl;
            return;
        }
    }
    cout << "Room not found.\n";
}

void HotelBookingSystem::adminLogin() {
    string user, pass;
    cout << "\n========== ADMIN LOGIN ==========\nUsername: "; cin >> user;
    cout << "Password: "; cin >> pass;
    for (const Admin& a : admins) {
        if (a.login(user, pass)) { adminLoggedIn = true; adminMenu(); return; }
    }
    cout << "Invalid admin credentials.\n";
}