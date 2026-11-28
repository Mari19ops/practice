#include "Order.h"
#include <iostream>
#include <numeric>
#include <algorithm>

Order::Order(int orderId, const Customer& customer)
    : orderId(orderId), customer(customer), currentStatus(OrderStatus::CREATED), totalAmount(0.0) {
    statusHistory.push(OrderStatus::CREATED);
}

void Order::updateStatus(OrderStatus newStatus) {
    currentStatus = newStatus;
    statusHistory.push(newStatus);
}

void Order::addItem(const Product& product, int quantity) {
    if (currentStatus != OrderStatus::CREATED) {
        std::cout << "[Error] Cannot add items after order processing has started.\n";
        return;
    }
    items.push_back(std::make_pair(product, quantity));
    calculateTotal();
}

void Order::calculateTotal() {
    totalAmount = std::accumulate(items.begin(), items.end(), 0.0,
        [](double sum, const std::pair<Product, int>& item) {
            return sum + (item.first.getPrice() * item.second);
        });
}

bool Order::reserveOrder(std::vector<Product>& catalog) {
    if (currentStatus != OrderStatus::CREATED) {
        std::cout << "[Error] Order must be in CREATED state to reserve items.\n";
        return false;
    }
    if (items.empty()) {
        std::cout << "[Error] Cannot reserve an empty order.\n";
        return false;
    }

    // 1. Check availability for all items first
    for (const auto& item : items) {
        auto it = std::find_if(catalog.begin(), catalog.end(),
            [&](const Product& p) { return p.getId() == item.first.getId(); });

        if (it == catalog.end() || !it->isAvailable(item.second)) {
            std::cout << "[Failed] Insufficient stock for product: " << item.first.getName() << "\n";
            return false;
        }
    }

    // 2. Deduct stock
    for (const auto& item : items) {
        auto it = std::find_if(catalog.begin(), catalog.end(),
            [&](const Product& p) { return p.getId() == item.first.getId(); });
        it->reserveStock(item.second);
    }

    updateStatus(OrderStatus::RESERVED);
    std::cout << "[Success] Stock reserved for Order #" << orderId << ".\n";
    return true;
}

bool Order::payOrder(double paymentAmount) {
    if (currentStatus != OrderStatus::RESERVED) {
        std::cout << "[Error] Order cannot be paid. Current status: " << getStatusString() 
                  << " (Must be RESERVED first).\n";
        return false;
    }
    if (paymentAmount < totalAmount) {
        std::cout << "[Error] Insufficient funds. Required: $" << totalAmount 
                  << ", Provided: $" << paymentAmount << "\n";
        return false;
    }

    updateStatus(OrderStatus::PAID);
    std::cout << "[Success] Order #" << orderId << " paid. Change: $" << (paymentAmount - totalAmount) << "\n";
    return true;
}

bool Order::packAndShip() {
    if (currentStatus != OrderStatus::PAID) {
        std::cout << "[Error] Cannot pack/ship unpaid order. Current status: " << getStatusString() << "\n";
        return false;
    }

    updateStatus(OrderStatus::PACKED_AND_SHIPPED);
    std::cout << "[Success] Order #" << orderId << " packed and handed over to courier.\n";
    return true;
}

bool Order::markAsDelivered() {
    if (currentStatus != OrderStatus::PACKED_AND_SHIPPED) {
        std::cout << "[Error] Order is not in transit yet. Current status: " << getStatusString() << "\n";
        return false;
    }

    updateStatus(OrderStatus::DELIVERED);
    std::cout << "[Success] Order #" << orderId << " has been DELIVERED!\n";
    return true;
}

bool Order::cancelOrder(std::vector<Product>& catalog) {
    if (currentStatus == OrderStatus::DELIVERED || currentStatus == OrderStatus::CANCELLED) {
        std::cout << "[Error] Cannot cancel order in status: " << getStatusString() << "\n";
        return false;
    }

    // Restore stock if it was already reserved, paid, or shipped
    if (currentStatus == OrderStatus::RESERVED || 
        currentStatus == OrderStatus::PAID || 
        currentStatus == OrderStatus::PACKED_AND_SHIPPED) {
        
        for (const auto& item : items) {
            auto it = std::find_if(catalog.begin(), catalog.end(),
                [&](const Product& p) { return p.getId() == item.first.getId(); });
            if (it != catalog.end()) {
                it->restoreStock(item.second);
            }
        }
        std::cout << "[Info] Reserved stock returned to warehouse.\n";
    }

    updateStatus(OrderStatus::CANCELLED);
    std::cout << "[Success] Order #" << orderId << " has been CANCELLED.\n";
    return true;
}

std::string Order::getStatusString() const {
    switch (currentStatus) {
        case OrderStatus::CREATED: return "CREATED";
        case OrderStatus::RESERVED: return "RESERVED";
        case OrderStatus::PAID: return "PAID";
        case OrderStatus::PACKED_AND_SHIPPED: return "PACKED & SHIPPED";
        case OrderStatus::DELIVERED: return "DELIVERED";
        case OrderStatus::CANCELLED: return "CANCELLED";
        default: return "UNKNOWN";
    }
}

void Order::printOrderDetails() const {
    std::cout << "----------------------------------------\n";
    std::cout << "Order #" << orderId << " | Status: [" << getStatusString() << "]\n";
    std::cout << customer.getRoleInfo() << "\n";
    std::cout << "Items:\n";
    for (const auto& item : items) {
        std::cout << "  - " << item.first.getName() << " x" << item.second 
                  << " ($" << item.first.getPrice() * item.second << ")\n";
    }
    std::cout << "Total Amount: $" << totalAmount << "\n";
    std::cout << "Status History Steps Count: " << statusHistory.size() << "\n";
    std::cout << "----------------------------------------\n";
}
