#ifndef KVFIFO_H
#define KVFIFO_H

#include <list>
#include <map>
#include <memory>

namespace {
using std::list;
using std::make_pair;
using std::make_shared;
using std::map;
using std::pair;
using std::prev;
using std::shared_ptr;
using std::weak_ptr;
} // namespace

template <typename K, typename V>
requires std::totally_ordered<K>
class kvfifo {

  using queue = list<pair<K, V>>;
  using queue_iterator = typename queue::iterator;
  using list_of_iterators = list<queue_iterator>;
  using map_of_list_of_iterators_to_list = map<K, list_of_iterators>;
  using no_element = std::invalid_argument;
  using smart_ptr_queue = shared_ptr<queue>;
  using smart_ptr_map = shared_ptr<map_of_list_of_iterators_to_list>;
  using weak_fifo = weak_ptr<queue>;
  using weak_map = weak_ptr<map_of_list_of_iterators_to_list>;

public:
  kvfifo() { initialize_new_data(); }
  kvfifo(kvfifo const &k)
      : fifo(k.fifo), position_map(k.position_map), fifo_w(k.fifo),
        position_map_w(k.position_map), has_to_copy(k.has_to_copy) {
    if (has_to_copy)
      copy();

    end_successful();
  }
  kvfifo(kvfifo &&k)
      : fifo(k.fifo), position_map(k.position_map), fifo_w(k.fifo),
        position_map_w(k.position_map), has_to_copy(k.has_to_copy) {
    k.clear();
  }
  ~kvfifo() noexcept = default;
  /*
   * - Operator przypisania przyjmujący argument przez wartość.
   * Złożoność O(1) plus czas niszczenia nadpisywanego obiektu.
   */
  kvfifo &operator=(kvfifo other) {
    position_map = other.position_map;
    fifo = other.fifo;
    has_to_copy = other.has_to_copy;
    end_successful();
    return *this;
  }

  /*
   *  Metoda push wstawia wartość v na koniec kolejki, nadając jej klucz k.
   *  Złożoność O(log n).
   */
  void push(K const &k, V const &v) {
    copy_if_needed();
    fifo->push_back(make_pair(k, v));
    auto it = prev(fifo->end());

    try {
      if (position_map->count(k) == 0) {
        list_of_iterators new_list;
        position_map->insert(make_pair(k, new_list));
      }

      position_map->at(k).push_back(it);
    } catch (std::exception &exception) // cofamy zmiany
    {
      fifo->pop_back();
      if (position_map->count(k) > 0 && position_map->at(k).size() == 0)
        position_map->erase(k);
      rollback();
      throw exception;
    }
    has_to_copy = false;
    end_successful();
  }

  /*
   * - Metoda pop() usuwa pierwszy element z kolejki.
   * Jeśli kolejka jest pusta, to
   * podnosi wyjątek std::invalid_argument. Złożoność O(log n).
   */
  void pop() {

    if (fifo->empty())
      throw no_element(queue_empty_message);

    copy_if_needed();

    K k = (fifo->begin())->first;
    fifo->pop_front();
    position_map->at(k).pop_front();
    if (position_map->at(k).size() == 0)
      position_map->erase(k);

    has_to_copy = false;
    end_successful();
  }

  /*
   * Metoda pop(k) usuwa pierwszy element o podanym kluczu z kolejki. Jeśli
   * podanego klucza nie ma w kolejce,
   * to podnosi wyjątek std::invalid_argument.
   * Złożoność O(log n).
   */
  void pop(K const &k) {
    if (position_map->count(k) == 0)
      throw no_element(invalid_argument_message);

    copy_if_needed();

    fifo->erase(*(position_map->at(k).begin()));
    position_map->at(k).pop_front();

    if (position_map->at(k).size() == 0)
      position_map->erase(k);

    has_to_copy = false;

    end_successful();
  }

  /*
   * Metoda move_to_back przesuwa elementy o kluczu k na koniec kolejki,
   * zachowując ich kolejność względem siebie.
   * Zgłasza wyjątek std::invalid_argument, gdy
   * elementu o podanym kluczu nie ma w kolejce.
   * Złożoność O(m + log n), gdzie m to
   * liczba przesuwanych elementów.
   */
  void move_to_back(K const &k) {
    if (position_map->count(k) == 0)
      throw no_element(invalid_argument_message);

    copy_if_needed();

    list_of_iterators &k_position_list = position_map->at(k);
    int s = k_position_list.size();
    int how_many_added_to_fifo = 0;
    int how_many_added_to_k_list = 0;

    auto it = k_position_list.begin();
    try {
      for (int i = 0; i < s; i++) {
        fifo->push_back(make_pair((*it)->first, (*it)->second));
        how_many_added_to_fifo++;
        k_position_list.push_back(prev(fifo->end()));
        how_many_added_to_k_list++;
        it++;
      }
    } catch (std::exception &exception) {
      while (how_many_added_to_fifo-- > 0)
        fifo->pop_back();

      while (how_many_added_to_k_list-- > 0)
        k_position_list.pop_back();

      rollback();
      throw exception;
    }
    it = k_position_list.begin();
    for (int i = 0; i < s; i++) {
      fifo->erase(*it);
      it++;
      k_position_list.pop_front();
    }

    has_to_copy = false;
    end_successful();
  }

  /*
   * Metody front i back zwracają parę referencji do klucza
   * i wartości znajdującej
   * się odpowiednio na początku i końcu kolejki.
   * W wersji nie-const zwrócona para
   * powinna umożliwiać modyfikowanie wartości, ale nie klucza.
   * Dowolna operacja
   * modyfikująca kolejkę może unieważnić zwrócone referencje.
   * Jeśli kolejka jest
   * pusta, to podnosi wyjątek std::invalid_argument. Złożoność O(1).
   */

  pair<K const &, V &> front() {
    if (fifo->empty())
      throw no_element(queue_empty_message);

    copy_if_needed();

    pair<K const &, V &> ret(fifo->begin()->first, fifo->begin()->second);
    has_to_copy = true; // referencja została pobrana, musimy kopiować

    end_successful();
    return ret;
  }

  pair<K const &, V const &> front() const {
    if (fifo->empty())
      throw no_element(queue_empty_message);

    pair<K const &, V const &> ret(fifo->begin()->first, fifo->begin()->second);

    return ret;
  }

  pair<K const &, V &> back() {
    if (fifo->empty())
      throw no_element(queue_empty_message);

    copy_if_needed();

    pair<K const &, V &> ret(prev(fifo->end())->first,
                             prev(fifo->end())->second);
    has_to_copy = true;
    end_successful();

    return ret;
  }
  pair<K const &, V const &> back() const {
    if (fifo->empty())
      throw no_element(queue_empty_message);
    pair<K const &, V const &> ret(prev(fifo->end())->first,
                                   prev(fifo->end())->second);
    return ret;
  }

  /*
   * Metody first i last zwracają odpowiednio pierwszą i ostatnią parę
   * klucz-wartość o danym kluczu, podobnie jak front i back.
   * Jeśli podanego klucza
   * nie ma w kolejce, to podnosi wyjątek std::invalid_argument.
   * Złożoność O(log n).-
   */

  pair<K const &, V &> first(K const &key) {
    check_key(key);

    copy_if_needed();

    list_of_iterators &k_list = get_key_list(key);
    auto elem = k_list.front();
    pair<K const &, V &> ret(elem->first, elem->second);
    has_to_copy = true;
    end_successful();

    return ret;
  }
  pair<K const &, V const &> first(K const &key) const {
    list_of_iterators &k_list = get_key_list(key);
    auto elem = k_list.front();
    pair<K const &, V const &> ret(elem->first, elem->second);
    return ret;
  }
  pair<K const &, V &> last(K const &key) {
    check_key(key);
    copy_if_needed();

    list_of_iterators &k_list = get_key_list(key);
    auto elem = k_list.back();
    pair<K const &, V &> ret(elem->first, elem->second);
    has_to_copy = true;

    end_successful();
    return ret;
  }
  pair<K const &, V const &> last(K const &key) const {
    list_of_iterators &k_list = get_key_list(key);
    auto elem = k_list.back();
    pair<K const &, V const &> ret(elem->first, elem->second);
    return ret;
  }

  /*
   * Metoda size zwraca liczbę elementów w kolejce. Złożoność O(1).
   */

  [[nodiscard]] size_t size() const noexcept { return fifo->size(); }

  /*
   * Metoda empty zwraca true, gdy kolejka jest pusta, a false w przeciwnym
   * przypadku. Złożoność O(1).
   */

  [[nodiscard]] bool empty() const noexcept { return (size() == 0); }

  /*
   * Metoda count zwraca liczbę elementów w kolejce o podanym kluczu.
   * Złożoność O(log n).
   */
  size_t count(K const &k) const noexcept {
    if (position_map->count(k) == 0)
      return 0;
    return position_map->at(k).size();
  }

  /* Metoda clear usuwa wszystkie elementy z kolejki. Złożoność O(n).*/
  void clear() { initialize_new_data(); }

  class k_iterator {
  public:
    // Typy potrzebne do spełnienia konceptu bidirectional_iterator
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = const K;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type *;
    using reference = value_type &;

    k_iterator(typename map_of_list_of_iterators_to_list::const_iterator p)
        : it(p) {}

    reference operator*() noexcept { return it->first; }
    pointer operator->() noexcept { return *it->first; }
    k_iterator &operator++() noexcept { // ++it
      it++;
      return *this;
    }
    k_iterator operator++(int) noexcept { // it++
      k_iterator result(*this);
      operator++();
      return result;
    }

    k_iterator &operator--() noexcept { // ++it
      it--;
      return *this;
    }
    k_iterator operator--(int) noexcept { // it++
      k_iterator result(*this);
      operator--();
      return result;
    }

    friend bool operator==(k_iterator const &a, k_iterator const &b) noexcept {
      return a.it == b.it;
    }
    friend bool operator!=(k_iterator const &a, k_iterator const &b) noexcept {
      return !(a == b);
    }

  private:
    typename map_of_list_of_iterators_to_list::const_iterator it;
  };

  k_iterator k_begin() const noexcept {
    return k_iterator(position_map->begin());
  }
  k_iterator k_end() const noexcept { return k_iterator(position_map->end()); }

private:
  smart_ptr_queue fifo;
  smart_ptr_map position_map;
  weak_fifo fifo_w;
  weak_map position_map_w;

  bool has_to_copy;

  static constexpr char invalid_argument_message[] =
      "no element with the given key";

  static constexpr char queue_empty_message[] = "queue empty";

  void copy() {
    smart_ptr_queue new_fifo = make_shared<queue>();
    smart_ptr_map new_position_map =
        make_shared<map_of_list_of_iterators_to_list>();

    for (auto iter = fifo->begin(); iter != fifo->end(); iter++)
      new_fifo->push_back({iter->first, iter->second});

    for (auto iter = new_fifo->begin(); iter != new_fifo->end(); iter++) {
      if (new_position_map->count(iter->first) == 0) {
        list_of_iterators new_list;
        new_position_map->insert(make_pair(iter->first, new_list));
      }
      new_position_map->at(iter->first).push_back(iter);
    }

    fifo = new_fifo;
    position_map = new_position_map;
    has_to_copy = false;
  }

  void copy_if_needed() {
    if (fifo.use_count() > 1)
      copy();
  }

  void end_successful() noexcept {
    fifo_w = fifo;
    position_map_w = position_map;
  }

  void rollback() noexcept {
    fifo = fifo_w.lock();
    position_map = position_map_w.lock();
  }

  void initialize_new_data() {
    smart_ptr_queue new_fifo = make_shared<queue>();
    smart_ptr_map new_position_map = 
        make_shared<map_of_list_of_iterators_to_list>();

    weak_fifo new_fifo_w(new_fifo);
    weak_map new_position_map_w(new_position_map);

    fifo = new_fifo;
    position_map = new_position_map;
    fifo_w = new_fifo_w;
    position_map_w = new_position_map_w;
    has_to_copy = false;
    end_successful();
  }
  list_of_iterators &get_key_list(K const &key) const {
    auto it = position_map->find(key);
    if (it == position_map->end())
      throw no_element(invalid_argument_message);
    return (*it).second;
  }
  void check_key(K const &key) const {
    auto it = position_map->find(key);
    if (it == position_map->end())
      throw no_element(invalid_argument_message);
  }
};

#endif /* KVFIFO_H */