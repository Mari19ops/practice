#ifndef ORDER_H
#define ORDER_H

#include <string>
#include <vector>
#include <stack>
#include <utility>
#include "Person.h"
#include "Product.h"

enum class OrderStatus {
    CREATED,
    RESERVED,
    PAID,
    PACKED_AND_SHIPPED,
    DELIVERED,
    CANCELLED
};

class Order {
private:
    int orderId;
    Customer customer;
    std::vector<std::pair<Product, int>> items;
    OrderStatus currentStatus;
    std::stack<OrderStatus> statusHistory;
    double totalAmount;

    void updateStatus(OrderStatus newStatus);

public:
    Order(int orderId, const Customer& customer);

    int getOrderId() const { return orderId; }
    OrderStatus getStatus() const { return currentStatus; }
    double getTotalAmount() const { return totalAmount; }
    const Customer& getCustomer() const { return customer; }
    const std::vector<std::pair<Product, int>>& getItems() const { return items; }

    void addItem(const Product& product, int quantity);
    void calculateTotal();

    bool reserveOrder(std::vector<Product>& catalog);
    bool payOrder(double paymentAmount);
    bool packAndShip();
    bool markAsDelivered();
    bool cancelOrder(std::vector<Product>& catalog);

    std::string getStatusString() const;
    void printOrderDetails() const;
};

#endif
