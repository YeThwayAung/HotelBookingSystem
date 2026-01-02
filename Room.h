#ifndef ROOM_H
#define ROOM_H

#include <string>
#include <iostream>
#include <iomanip>

using namespace std;

class Room {
protected:
    string roomNumber;
    string roomType;
    double pricePerNight;
    bool isAvailable;
    string guestName;
    int nightsBooked;
    string specialAmenity;

public:
    Room(string num = "", string type = "", double price = 0.0, bool available = true);
    virtual ~Room();

    virtual double calculateBill(int nights);
    virtual double getDiscount(int nights);
    virtual double getTax(int nights);
    virtual bool bookRoom(string guest, int nights);
    virtual void displayRoomDetails() const;
    virtual void displayAmenities() const;
    virtual string toFileString() const;

    // Getters
    string getRoomNumber() const;
    string getRoomType() const;
    double getPricePerNight() const;
    bool getAvailability() const;
    string getGuestName() const;
    int getNightsBooked() const;
    string getSpecialAmenity() const;

    // Setters
    void setAvailability(bool status);
    void setNightsBooked(int nights);
    void setPrice(double price);
    void setSpecialAmenity(const string& amenity);

    void checkOut();
    void forceCheckOut();
};

#endif
