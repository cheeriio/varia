#include "system.hpp"
#include <chrono>
#include <set>
#include <iostream>

Order::Order(std::vector<std::string> products_arg)
    : status(Ordered)
{
    for (std::string p : products_arg)
        products.push_back(p);
}

[[nodiscard]] unsigned int CoasterPager::getId() const
{
    return id;
}

[[nodiscard]] bool CoasterPager::isReady() const
{
    return my_order->status == Waiting;
}

void CoasterPager::wait() const
{
    std::unique_lock<std::mutex> lo(my_order->waiting_mutex);
    if (my_order->status == Ordered)
        my_order->cv_waiting.wait(lo, [&] { return my_order->status != Ordered; });
    if (my_order->status == Failed)
        throw FulfillmentFailure();
}

void CoasterPager::wait(unsigned int timeout) const
{
    std::unique_lock<std::mutex> lo(my_order->waiting_mutex);
    if (my_order->status == Ordered)
        my_order->cv_waiting.wait_for(lo, std::chrono::milliseconds(timeout), [&] { return my_order->status != Ordered; });
    if (my_order->status == Failed)
        throw FulfillmentFailure();
}

MachineWrapper::MachineWrapper(std::shared_ptr<Machine> machine_arg)
    : machine(machine_arg)
    , taken(false)
{
}

std::unique_ptr<Product> MachineWrapper::getProduct()
{
    std::mutex mut;
    std::unique_lock<std::mutex> lo(mut);
    if (taken)
        cv.wait(lo, [&] { return !taken; });
    try {
        std::unique_ptr<Product> result = machine->getProduct();
        taken = false;
        cv.notify_one();
        return result;
    } catch (std::exception& ex) {
        taken = false;
        cv.notify_one();
        throw ex;
    }
}
void MachineWrapper::returnProduct(std::unique_ptr<Product> product)
{
    std::mutex mut;
    std::unique_lock<std::mutex> lo(mut);
    if (taken)
        cv.wait(lo, [&] { return !taken; });
    taken = true;
    try {
        machine->returnProduct(std::move(product));
        taken = false;
        cv.notify_one();
    } catch (std::exception& ex) {
        taken = false;
        cv.notify_one();
        throw ex;
    }
}
void MachineWrapper::start()
{
    machine->start();
}
void MachineWrapper::stop()
{
    machine->stop();
}

void run_helper(std::shared_ptr<Order> order, std::string& my_product,
    std::set<std::string>& failed_products, std::shared_ptr<MachineWrapper>& machine)
{
    try {
        std::unique_ptr<Product> ret = machine->getProduct();
        order->safety.lock();
        order->result.push_back(std::move(ret));
        order->source.emplace(std::make_pair((order->result.size() - 1), machine));
        order->safety.unlock();
    } catch (std::exception& e) {
        order->safety.lock();
        order->status = Failed;
        failed_products.insert(my_product);
        order->safety.unlock();
    }
}

void run_worker(WorkerReport& report, std::vector<std::shared_ptr<Order>>& orders,
    std::mutex& mutex_orders, std::condition_variable& cv_orders,
    std::vector<std::string>& menu, std::vector<unsigned int>& pending_menu,
    unsigned int& num_of_orders, unsigned int& new_orders, bool& working,
    std::unordered_map<std::string, std::shared_ptr<MachineWrapper>>& machines, unsigned int& timeout)
{
    std::shared_ptr<Order> my_order;
    std::set<std::string> failed_products;
    unsigned int num_of_products, order_id;
    std::vector<std::thread> helpers;
    while (working || new_orders > 0) {
        failed_products.clear();
        helpers.clear();
        {
            std::unique_lock<std::mutex> lo(mutex_orders);
            if (new_orders == 0 && working) {
                cv_orders.wait(lo, [&] { return ((new_orders > 0) || !working); });
            }
            if (!working && new_orders == 0) {
                break;
            }
            my_order = orders.at(num_of_orders - new_orders);
            order_id = num_of_orders - new_orders + 1;
            new_orders--;
            // włącz wątki zbierające produkty
            num_of_products = my_order->products.size();
            for (unsigned int i = 0; i < num_of_products; i++) {
                std::thread helper([&my_order, &failed_products, &machines, i] {
                    my_order->safety.lock();
                    auto machine = machines.at(my_order->products[i]);
                    my_order->safety.unlock();
                    run_helper(my_order, my_order->products[i], failed_products, machine);
                });
                my_order->safety.lock();
                helpers.push_back(std::move(helper));
                my_order->safety.unlock();
            }
            if (new_orders > 0)
                cv_orders.notify_one();
        }
        for (unsigned int i = 0; i < num_of_products; i++)
            helpers[i].join();
        // oddajemy zamowienie
        if (my_order->status != Failed) {
            my_order->status = Waiting;
            my_order->cv_waiting.notify_all();
            std::unique_lock<std::mutex> lo(my_order->workers_mutex);
            my_order->cv_worker.wait_for(lo, std::chrono::milliseconds(timeout), [&] { return my_order->status != Waiting; });
            if (my_order->status == Waiting) {
                my_order->status = Expired;
                for (unsigned int i = 0; i < my_order->result.size(); i++) {
                    my_order->source.at(i)->returnProduct(std::move(my_order->result.at(i)));
                }
            }
        } else {
            my_order->cv_waiting.notify_all();
            for (unsigned int i = 0; i < my_order->result.size(); i++) {
                my_order->source.at(i)->returnProduct(std::move(my_order->result.at(i)));
            }
        }

        if (my_order->status == Expired) {
            mutex_orders.lock();
            std::vector<unsigned int> new_pmenu;
            for (auto id : pending_menu) {
                if (id != order_id)
                    new_pmenu.push_back(id);
            }
            pending_menu = new_pmenu;
            mutex_orders.unlock();
            std::vector<std::string> products_copy;
            for (std::string s : my_order->products)
                products_copy.push_back(s);
            report.abandonedOrders.push_back(products_copy);
        } else if (my_order->status == Served) {
            std::vector<std::string> products_copy;
            for (std::string s : my_order->products)
                products_copy.push_back(s);
            report.collectedOrders.push_back(products_copy);
        } else {
            mutex_orders.lock();
            std::vector<unsigned int> new_pmenu;
            for (auto id : pending_menu) {
                if (id != order_id)
                    new_pmenu.push_back(id);
            }
            pending_menu = new_pmenu;
            mutex_orders.unlock();
            std::vector<std::string> products_copy;
            for (std::string s : my_order->products)
                products_copy.push_back(s);
            report.failedOrders.push_back(products_copy);
            for (std::string s : failed_products)
                report.failedProducts.push_back(s);
            mutex_orders.lock();
            std::vector<std::string> new_menu;
            for (auto it = menu.begin(); it != menu.end(); it++) {
                if (!failed_products.count(*it))
                    new_menu.push_back(*it);
            }
            menu = new_menu;
            mutex_orders.unlock();
        }
    }
}

System::System(machines_t machines_arg, unsigned int numberOfWorkers, unsigned int clientTimeout)
    : timeout(clientTimeout)
    , num_of_workers(numberOfWorkers)
    , num_of_orders(0)
    , new_orders(0)
    , working(true)
    , reports(numberOfWorkers)
{
    for (std::pair<std::string, std::shared_ptr<Machine>> p : machines_arg) {
        p.second->start();
        machines.emplace(std::make_pair(p.first, std::make_shared<MachineWrapper>(p.second)));
        menu.push_back(p.first);
    }
    for (unsigned int i = 0; i < numberOfWorkers; i++) {
        std::thread new_worker([this, i] {
            run_worker(reports[i], orders, mutex_orders, cv_new_orders, menu,
                pending_menu, num_of_orders, new_orders, working, machines, timeout);
        });
        workers.push_back(std::move(new_worker));
    }
}

std::vector<WorkerReport> System::shutdown()
{
    if (!working)
        return reports;
    mutex_orders.lock();
    working = false;
    cv_new_orders.notify_all();
    mutex_orders.unlock();
    for (unsigned int i = 0; i < num_of_workers; i++) {
        workers[i].join();
    }
    for (auto& p : machines)
        p.second->stop();
    return reports;
}

std::vector<std::string> System::getMenu() const
{
    if (!working)
        return std::vector<std::string>();
    return menu;
}

std::vector<unsigned int> System::getPendingOrders() const
{
    return pending_menu;
}

std::unique_ptr<CoasterPager> System::order(std::vector<std::string> products)
{
    for (std::string& s : products) {
        bool in_menu = false;
        for (std::string& s_menu : menu) {
            if (s == s_menu)
                in_menu = true;
        }
        if (!in_menu)
            throw BadOrderException();
    }
    if (!working)
        throw RestaurantClosedException();

    mutex_orders.lock();
    pending_menu.push_back(num_of_orders);
    std::shared_ptr<Order> my_order = std::make_shared<Order>(products);
    orders.push_back(my_order);
    num_of_orders++;
    new_orders++;
    if (new_orders == 1)
        cv_new_orders.notify_one();
    std::unique_ptr<CoasterPager> ret = std::make_unique<CoasterPager>();
    ret->id = num_of_orders - 1;
    ret->my_order = my_order;

    mutex_orders.unlock();

    std::unique_lock<std::mutex> lo(my_order->ordering_mutex);
    return ret;
}

std::vector<std::unique_ptr<Product>> System::collectOrder(std::unique_ptr<CoasterPager> CoasterPager)
{
    if (CoasterPager == nullptr)
        throw BadPagerException();
    unsigned int order_id = CoasterPager->getId();
    if (order_id >= num_of_orders)
        throw BadPagerException();
    mutex_orders.lock();
    std::shared_ptr<Order> my_order = orders.at(order_id);
    mutex_orders.unlock();
    my_order->safety.lock();
    if (my_order->status == Served) {
        my_order->safety.unlock();
        throw BadPagerException();
    } else if (my_order->status == Ordered) {
        my_order->safety.unlock();
        throw OrderNotReadyException();
    } else if (my_order->status == Expired) {
        my_order->safety.unlock();
        throw OrderExpiredException();
    } else if (my_order->status == Failed) {
        my_order->safety.unlock();
        throw FulfillmentFailure();
    }
    mutex_orders.lock();
    std::vector<unsigned int> new_pmenu;
    for (auto id : pending_menu) {
        if (id != order_id)
            new_pmenu.push_back(id);
    }
    pending_menu = new_pmenu;
    mutex_orders.unlock();
    my_order->status = Served;
    my_order->cv_worker.notify_all();
    my_order->safety.unlock();
    return std::move(my_order->result);
}

unsigned int System::getClientTimeout() const
{
    return timeout;
}
