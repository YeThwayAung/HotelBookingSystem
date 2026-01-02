#ifndef DELUXEROOM_H
#define DELUXEROOM_H

#include "Room.h"

class DeluxeRoom : public Room {
private:
    bool hasBalcony;

public:
    DeluxeRoom(string num = "", double price = 150.0, bool available = true, bool balcony = true);
    
    double getDiscount(int nights) override;
    double calculateBill(int nights) override;
    void displayAmenities() const override;
};

#endif