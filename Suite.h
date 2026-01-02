#ifndef SUITE_H
#define SUITE_H

#include "Room.h"

class Suite : public Room {
private:
    bool hasJacuzzi;
    int numRooms;

public:
    Suite(string num = "", double price = 300.0, bool available = true,
          bool jacuzzi = true, int rooms = 2);
    
    double getTax(int nights) override;
    double calculateBill(int nights) override;
    void displayAmenities() const override;
};

#endif