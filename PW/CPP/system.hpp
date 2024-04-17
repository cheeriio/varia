#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include <exception>
#include <future>
#include <unordered_map>
#include <vector>
#include <iostream>

#include "machine.hpp"

class FulfillmentFailure : public std::exception {
};

class OrderNotReadyException : public std::exception {
};

class BadOrderException : public std::exception {
};

class BadPagerException : public std::exception {
};

class OrderExpiredException : public std::exception {
};

class RestaurantClosedException : public std::exception {
};

struct WorkerReport {
    std::vector<std::vector<std::string>> collectedOrders;
    std::vector<std::vector<std::string>> abandonedOrders;
    std::vector<std::vector<std::string>> failedOrders;
    std::vector<std::string> failedProducts;
};

class MachineWrapper {
public:
    MachineWrapper(std::shared_ptr<Machine> machine_arg);
    std::unique_ptr<Product> getProduct();
    void returnProduct(std::unique_ptr<Product> product);
    void start();
    void stop();

private:
    std::shared_ptr<Machine> machine;
    std::condition_variable cv;
    bool taken;
};

enum Order_status {
    Ordered,
    Prepared,
    Waiting,
    Served,
    Expired,
    Failed
};

class Order {
public:
    Order(std::vector<std::string> products_arg);
    std::vector<std::string> products;
    std::vector<std::unique_ptr<Product>> result;
    std::unordered_map<unsigned long int, std::shared_ptr<MachineWrapper>> source;
    Order_status status;
    std::mutex ordering_mutex, workers_mutex, waiting_mutex, safety;
    std::condition_variable cv_ordering, cv_worker, cv_waiting;
};

class CoasterPager {
public:
    void wait() const;

    void wait(unsigned int timeout) const;

    [[nodiscard]] unsigned int getId() const;

    [[nodiscard]] bool isReady() const;
private:
    int id;
    std::shared_ptr<Order> my_order;
    
    friend class System;
};


class System {
public:
    typedef std::unordered_map<std::string, std::shared_ptr<Machine>> machines_t;

    System(machines_t machines, unsigned int numberOfWorkers, unsigned int clientTimeout);

    std::vector<WorkerReport> shutdown();

    std::vector<std::string> getMenu() const;

    std::vector<unsigned int> getPendingOrders() const;

    std::unique_ptr<CoasterPager> order(std::vector<std::string> products);

    std::vector<std::unique_ptr<Product>> collectOrder(std::unique_ptr<CoasterPager> CoasterPager);

    unsigned int getClientTimeout() const;

private:
    unsigned int timeout, num_of_workers, num_of_orders, new_orders;
    bool working;
    std::vector<WorkerReport> reports;
    std::vector<std::shared_ptr<Order>> orders;
    std::vector<std::thread> workers;
    std::mutex mutex_orders;
    std::condition_variable cv_new_orders;
    std::vector<std::string> menu;
    std::vector<unsigned int> pending_menu;
    std::unordered_map<std::string, std::shared_ptr<MachineWrapper>> machines;
};

#endif // SYSTEM_HPP