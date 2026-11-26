#ifndef PERSON_H
#define PERSON_H

#include <string>

class Person {
protected:
    int id;
    std::string name;

public:
    Person(int id, const std::string& name) : id(id), name(name) {}
    virtual ~Person() = default;

    int getId() const { return id; }
    std::string getName() const { return name; }

    virtual std::string getRoleInfo() const = 0;
};

class Customer : public Person {
private:
    std::string address;

public:
    Customer(int id, const std::string& name, const std::string& address)
        : Person(id, name), address(address) {}

    std::string getAddress() const { return address; }

    std::string getRoleInfo() const override {
        return "Customer: " + name + " (Delivery Address: " + address + ")";
    }
};

class WarehouseWorker : public Person {
private:
    std::string shift;

public:
    WarehouseWorker(int id, const std::string& name, const std::string& shift)
        : Person(id, name), shift(shift) {}

    std::string getRoleInfo() const override {
        return "Staff: " + name + " (Shift: " + shift + ")";
    }
};

#endif
