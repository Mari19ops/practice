#ifndef STORESYSTEM_H
#define STORESYSTEM_H

#include <vector>
#include <queue>
#include "Product.h"
#include "Order.h"

class StoreSystem {
private:
    std::vector<Product> catalog;
    std::vector<Order> allOrders;
    std::queue<int> processingQueue;
    WarehouseWorker activeWorker;
    int nextOrderId;

    Order* findOrderById(int id);

public:
    StoreSystem();

    void displayCatalog(bool sortByPrice = false);
    void createCustomerOrder();
    void processNextInQueue();
    void manageSpecificOrder();
    void viewAllOrders() const;
    void runMenu();
};

#endif
