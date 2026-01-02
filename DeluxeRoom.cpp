#include "DeluxeRoom.h"

DeluxeRoom::DeluxeRoom(string num, double price, bool available, bool balcony)
    : Room(num, "Deluxe Room", price, available), hasBalcony(balcony) {}

double DeluxeRoom::getDiscount(int nights) {
    if (nights > 3) {
        double baseCost = pricePerNight * nights;
        return baseCost * 0.10; // 10% discount
    }
    return 0.0;
}

double DeluxeRoom::calculateBill(int nights) {
    double baseCost = pricePerNight * nights;
    double discount = getDiscount(nights);
    return baseCost - discount;
}

void DeluxeRoom::displayAmenities() const {
    cout << "Deluxe Amenities:\n";
    cout << "  - King-size bed\n";
    cout << "  - Mini bar\n";
    cout << "  - Smart TV\n";
    cout << "  - Premium toiletries\n";
    if (hasBalcony) cout << "  - Private balcony\n";
}