#ifndef PRODUCT_H
#define PRODUCT_H

#include <string>

class Product {
private:
    int id;
    std::string name;
    double price;
    int stockQuantity;

public:
    Product(int id, const std::string& name, double price, int stockQuantity)
        : id(id), name(name), price(price), stockQuantity(stockQuantity) {}

    int getId() const { return id; }
    std::string getName() const { return name; }
    double getPrice() const { return price; }
    int getStockQuantity() const { return stockQuantity; }

    bool isAvailable(int requestedQty) const {
        return stockQuantity >= requestedQty && requestedQty > 0;
    }

    void reserveStock(int qty) {
        if (isAvailable(qty)) {
            stockQuantity -= qty;
        }
    }

    void restoreStock(int qty) {
        stockQuantity += qty;
    }
};

#endif
