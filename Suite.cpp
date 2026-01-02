#include "Suite.h"

Suite::Suite(string num, double price, bool available, bool jacuzzi, int rooms)
    : Room(num, "Suite", price, available), hasJacuzzi(jacuzzi), numRooms(rooms) {}

double Suite::getTax(int nights) {
    double baseCost = pricePerNight * nights;
    return baseCost * 0.15; // 15% luxury tax
}

double Suite::calculateBill(int nights) {
    double baseCost = pricePerNight * nights;
    double tax = getTax(nights);
    return baseCost + tax;
}

void Suite::displayAmenities() const {
    cout << "Luxury Suite Amenities:\n";
    cout << "  - " << numRooms << " separate rooms\n";
    cout << "  - Master bedroom with king-size bed\n";
    cout << "  - Living area with sofa\n";
    cout << "  - Full kitchen\n";
    cout << "  - Premium entertainment system\n";
    cout << "  - Complimentary breakfast\n";
    if (hasJacuzzi) cout << "  - Private jacuzzi\n";
    cout << "  - 24/7 concierge service\n";
}