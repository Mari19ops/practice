#include "StoreSystem.h"
#include <iostream>
#include <algorithm>
#include <limits>

StoreSystem::StoreSystem() 
    : activeWorker(101, "Alex Mercer", "Day Shift"), nextOrderId(1001) {
    // Seed initial warehouse inventory
    catalog.emplace_back(1, "Laptop Pro 14", 1200.00, 5);
    catalog.emplace_back(2, "Wireless Mouse", 45.50, 20);
    catalog.emplace_back(3, "Mechanical Keyboard", 110.00, 10);
    catalog.emplace_back(4, "27-inch 4K Monitor", 350.00, 3);
    catalog.emplace_back(5, "USB-C Hub", 30.00, 15);
}

Order* StoreSystem::findOrderById(int id) {
    auto it = std::find_if(allOrders.begin(), allOrders.end(),
        [id](const Order& o) { return o.getOrderId() == id; });
    if (it != allOrders.end()) {
        return &(*it);
    }
    return nullptr;
}

void StoreSystem::displayCatalog(bool sortByPrice) {
    std::vector<Product> displayList = catalog;
    if (sortByPrice) {
        std::sort(displayList.begin(), displayList.end(),
            [](const Product& a, const Product& b) {
                return a.getPrice() < b.getPrice();
            });
    }

    std::cout << "\n=== WAREHOUSE CATALOG ===\n";
    for (const auto& p : displayList) {
        std::cout << "ID: " << p.getId() << " | " << p.getName()
                  << " | Price: $" << p.getPrice()
                  << " | In Stock: " << p.getStockQuantity() << "\n";
    }
    std::cout << "=========================\n";
}

void StoreSystem::createCustomerOrder() {
    std::string name, address;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    std::cout << "Enter Customer Name: ";
    std::getline(std::cin, name);
    std::cout << "Enter Delivery Address: ";
    std::getline(std::cin, address);

    Customer customer(nextOrderId + 500, name, address);
    Order newOrder(nextOrderId, customer);

    while (true) {
        displayCatalog(false);
        std::cout << "Enter Product ID to add (or 0 to finish): ";
        int prodId;
        if (!(std::cin >> prodId)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        if (prodId == 0) break;

        auto it = std::find_if(catalog.begin(), catalog.end(),
            [prodId](const Product& p) { return p.getId() == prodId; });

        if (it == catalog.end()) {
            std::cout << "[Error] Product not found.\n";
            continue;
        }

        std::cout << "Enter Quantity: ";
        int qty;
        std::cin >> qty;
        if (qty <= 0) {
            std::cout << "[Error] Quantity must be positive.\n";
            continue;
        }

        newOrder.addItem(*it, qty);
        std::cout << "Added " << qty << "x " << it->getName() << " to cart.\n";
    }

    if (newOrder.getItems().empty()) {
        std::cout << "[Info] Empty order discarded.\n";
        return;
    }

    allOrders.push_back(newOrder);
    processingQueue.push(nextOrderId);
    std::cout << "\n[Success] Order #" << nextOrderId << " created and added to processing queue!\n";
    nextOrderId++;
}

void StoreSystem::processNextInQueue() {
    if (processingQueue.empty()) {
        std::cout << "\n[Info] Processing queue is empty.\n";
        return;
    }

    int currentId = processingQueue.front();
    Order* order = findOrderById(currentId);

    if (!order) {
        processingQueue.pop();
        return;
    }

    std::cout << "\n>>> Warehouse Queue Processing (" << activeWorker.getRoleInfo() << ") <<<\n";
    order->printOrderDetails();

    std::cout << "Select action for queued Order #" << currentId << ":\n";
    std::cout << "1. Check Stock & Reserve\n";
    std::cout << "2. Process Payment\n";
    std::cout << "3. Pack & Hand Over to Delivery\n";
    std::cout << "4. Mark as Delivered (Removes from active queue)\n";
    std::cout << "5. Cancel Order (Removes from active queue)\n";
    std::cout << "0. Back to Main Menu\n";
    std::cout << "Choice: ";

    int choice;
    std::cin >> choice;

    switch (choice) {
        case 1:
            order->reserveOrder(catalog);
            break;
        case 2: {
            std::cout << "Enter payment amount ($" << order->getTotalAmount() << " due): ";
            double amount;
            std::cin >> amount;
            order->payOrder(amount);
            break;
        }
        case 3:
            order->packAndShip();
            break;
        case 4:
            if (order->markAsDelivered()) {
                processingQueue.pop();
            }
            break;
        case 5:
            if (order->cancelOrder(catalog)) {
                processingQueue.pop();
            }
            break;
        case 0:
            break;
        default:
            std::cout << "[Error] Invalid option.\n";
    }
}

void StoreSystem::manageSpecificOrder() {
    std::cout << "Enter Order ID: ";
    int id;
    std::cin >> id;

    Order* order = findOrderById(id);
    if (!order) {
        std::cout << "[Error] Order #" << id << " not found.\n";
        return;
    }

    order->printOrderDetails();
    std::cout << "1. Reserve Stock | 2. Pay | 3. Pack & Ship | 4. Deliver | 5. Cancel | 0. Exit\nChoice: ";
    int choice;
    std::cin >> choice;

    switch (choice) {
        case 1: order->reserveOrder(catalog); break;
        case 2: {
            std::cout << "Enter payment amount: ";
            double amt;
            std::cin >> amt;
            order->payOrder(amt);
            break;
        }
        case 3: order->packAndShip(); break;
        case 4: order->markAsDelivered(); break;
        case 5: order->cancelOrder(catalog); break;
        default: break;
    }
}

void StoreSystem::viewAllOrders() const {
    if (allOrders.empty()) {
        std::cout << "\n[Info] No orders in the system yet.\n";
        return;
    }
    std::cout << "\n=== ALL SYSTEM ORDERS ===\n";
    for (const auto& o : allOrders) {
        o.printOrderDetails();
    }
}

void StoreSystem::runMenu() {
    while (true) {
        std::cout << "\n==========================================\n";
        std::cout << "   E-COMMERCE ORDER PROCESSING SYSTEM\n";
        std::cout << "   Orders in Active Queue: " << processingQueue.size() << "\n";
        std::cout << "==========================================\n";
        std::cout << "1. View Product Catalog (Default)\n";
        std::cout << "2. View Product Catalog (Sorted by Price)\n";
        std::cout << "3. Create New Customer Order\n";
        std::cout << "4. Process Next Order in Queue (Step-by-Step)\n";
        std::cout << "5. Manage Order by ID\n";
        std::cout << "6. View All Orders History\n";
        std::cout << "0. Exit\n";
        std::cout << "Select option: ";

        int choice;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "[Error] Please enter a valid number.\n";
            continue;
        }

        switch (choice) {
            case 1: displayCatalog(false); break;
            case 2: displayCatalog(true); break;
            case 3: createCustomerOrder(); break;
            case 4: processNextInQueue(); break;
            case 5: manageSpecificOrder(); break;
            case 6: viewAllOrders(); break;
            case 0:
                std::cout << "Exiting system. Goodbye!\n";
                return;
            default:
                std::cout << "[Error] Invalid choice.\n";
        }
    }
}
