#ifndef HOTELBOOKINGSYSTEM_H
#define HOTELBOOKINGSYSTEM_H

#include <vector>
#include <string>
#include "Room.h"
#include "DeluxeRoom.h"
#include "Suite.h"
#include "Admin.h"

using namespace std;

class HotelBookingSystem {
private:
    vector<Room*> rooms;
    vector<Admin> admins;
    const string filename = "hotel_data.txt";
    const string adminFile = "admins.txt";
    bool adminLoggedIn = false;

    // Helper functions
    void clearInputBuffer();
    void loadAdminsFromFile();
    void loadRoomsFromFile();
    void saveRoomsToFile();
    bool requireAdmin() const;
    bool roomNumberExists(const string& roomNum) const;
    void printDetailedBill(Room* room);
    void initializeDefaultRooms();

    // Admin functions
    void adminAddRoom();
    void adminRemoveRoom();
    void adminChangeRoomPrice();
    void adminForceCheckout();
    void adminViewAllBookings();
    void adminViewStatistics();
    void adminMenu();
    void adminLogin();

    // Guest functions
    void viewAvailableRooms();
    void bookRoom();
    void calculateBill();
    void displayRoomDetails();
    void checkOutGuest();
    void searchRooms();
    void extendBooking();

public:
    HotelBookingSystem();
    ~HotelBookingSystem();
    void displayMenu();
    void run();
};

#endif
