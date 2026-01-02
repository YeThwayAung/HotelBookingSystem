#include "Room.h"

Room::Room(string num, string type, double price, bool available)
    : roomNumber(num), roomType(type), pricePerNight(price),
      isAvailable(available), guestName(""), nightsBooked(0), specialAmenity("") {}

Room::~Room() {}

double Room::calculateBill(int nights) {
    return pricePerNight * nights;
}

double Room::getDiscount(int nights) {
    return 0.0;
}

double Room::getTax(int nights) {
    return 0.0;
}

bool Room::bookRoom(string guest, int nights) {
    if (!isAvailable) return false;
    guestName = guest;
    nightsBooked = nights;
    isAvailable = false;
    return true;
}

void Room::displayRoomDetails() const {
    cout << "\n----------------------------------------\n";
    cout << "Room Number: " << roomNumber << endl;
    cout << "Room Type: " << roomType << endl;
    cout << "Price per Night: $" << fixed << setprecision(2) << pricePerNight << endl;
    cout << "Status: " << (isAvailable ? "Available" : "Occupied") << endl;
    if (!isAvailable) {
        cout << "Guest Name: " << guestName << endl;
        cout << "Nights Booked: " << nightsBooked << endl;
    }
    displayAmenities();
    cout << "----------------------------------------\n";
}

void Room::displayAmenities() const {
    cout << "Standard amenities included.\n";
    if (!specialAmenity.empty()) {
        cout << "Special: " << specialAmenity << endl;
    }
}

string Room::toFileString() const {
    return roomNumber + "," + roomType + "," + to_string(pricePerNight) + "," +
           (isAvailable ? "1" : "0") + "," + guestName + "," + to_string(nightsBooked) +
           "," + specialAmenity;
}

string Room::getRoomNumber() const { return roomNumber; }
string Room::getRoomType() const { return roomType; }
double Room::getPricePerNight() const { return pricePerNight; }
bool Room::getAvailability() const { return isAvailable; }
string Room::getGuestName() const { return guestName; }
int Room::getNightsBooked() const { return nightsBooked; }
string Room::getSpecialAmenity() const { return specialAmenity; }

void Room::setAvailability(bool status) { isAvailable = status; }
void Room::setNightsBooked(int nights) { nightsBooked = nights; }
void Room::setPrice(double price) { pricePerNight = price; }
void Room::setSpecialAmenity(const string& amenity) { specialAmenity = amenity; }

void Room::checkOut() {
    isAvailable = true;
    guestName = "";
    nightsBooked = 0;
}

void Room::forceCheckOut() {
    isAvailable = true;
    guestName = "";
    nightsBooked = 0;
}