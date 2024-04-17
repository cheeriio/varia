/** @file
 * Biblioteka obsługująca nieuporządkowane zbiory ciągów liczb.
 *
 * @authors Michał Płachta, Mikołaj Szymański
 */
#include "hash.h"
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <vector>

namespace {
    
#ifdef NDEBUG
bool f_debug() {
	return false;
}
#else
bool f_debug()
{
	std::ios_base::Init();
	return true;
}
#endif

const bool & debug()
{
	static const bool d = f_debug();
	return d;
}

using sequence_t = std::vector<uint64_t>;

struct Hash_seq {
    jnp1::hash_function_t hash_function;
    explicit Hash_seq(jnp1::hash_function_t p_hash_function) {
        hash_function = p_hash_function;
    }
    uint64_t operator()(const sequence_t & s) const {
        return hash_function(s.data(), s.size());
    }
};

using hash_table_t = std::unordered_set<sequence_t, Hash_seq>;
using hash_tables_t = std::unordered_map<unsigned long, hash_table_t>;

hash_tables_t & hash_tables() {
    static hash_tables_t htables;
    return htables;
}

unsigned long & hash_tables_cnt() {
    static unsigned long htables_cnt = 1;
    return htables_cnt;
}

/* tests, whether seq is NULL, size is 0 ie. logical correctness of arguments */
bool test_data(const std::string & function, unsigned long id, uint64_t const * seq, size_t size) {
    bool correct = true;

    if (seq == NULL) {
        if (debug()) {
            std::cerr << function << "(" << id << ", NULL, " << size << ")\n";
            std::cerr << function << ": invalid pointer (NULL)\n";
        }
        correct = false;
    }
    else if (debug()) {
        std::cerr << function << "(" << id << ", \"";

        for (size_t i = 0; i + 1 < size; i++)
            std::cerr << seq[i] << " ";

        if (size > 0)
            std::cerr << seq[size - 1] << "\", " << size << ")\n";
        else
            std::cerr << "\", " << size << ")\n";
    }
    if (size == 0) {
        if (debug())
            std::cerr << function << ": invalid size (0)\n";
        correct = false;
    }
    return correct;
}

} // namespace

namespace jnp1{

unsigned long hash_create(hash_function_t hash_function) {
    if (hash_function == NULL) {
        if (debug()) {
            std::cerr << "hash_create(NULL)\n";
            std::cerr << "hash_create: invalid pointer (NULL)\n";
        }
        return 0;
    }
    if (debug())
        std::cerr << "hash_create(" << hash_function << ")\n";

    Hash_seq hash_seq(hash_function);
    hash_table_t new_hash_table(0, hash_seq);
    hash_tables().emplace(hash_tables_cnt(), new_hash_table);

    if (debug())
        std::cerr << "hash_create: hash table #" << hash_tables_cnt() << " created";
    return hash_tables_cnt()++;
}

void hash_delete(unsigned long id) {
    if (debug())
        std::cerr << "hash_delete(" << id << ")\n";
    if (hash_tables().count(id) == 0) {
        if (debug())
            std::cerr << "hash_delete: hash table #" << id << " does not exist\n";
        return;
    }
    hash_tables().erase(id);

    if (debug())
        std::cerr << "hash_delete: hash table #" << id << " deleted\n";
}

size_t hash_size(unsigned long id) {
    if (debug())
        std::cerr << "hash_size(" << id << ")\n";

    if (hash_tables().count(id) == 0) {
        if (debug())
            std::cerr << "hash_size: hash table #" << id << " does not exist\n";
        return 0;
    }
    size_t ans = hash_tables().at(id).size();

    if (debug())
        std::cerr << "hash_size: hash table #" << id << " contains " << ans << " element(s)\n";
    return ans;

}

bool hash_insert(unsigned long id, uint64_t const * seq, size_t size) {
    if (!test_data("hash_insert", id, seq, size))
        return false;

    if (hash_tables().count(id) == 0) {
        if (debug())
            std::cerr << "hash_insert: hash table #" << id << " does not exist\n";
        return false;
    }
    sequence_t s;

    for (size_t i = 0; i < size; i++)
        s.emplace_back(seq[i]);

    if (hash_tables().at(id).count(s) != 0) {
        if (debug()) {
            std::cerr << "hash_insert: hash table #" << id << ", sequence \"";

            for (size_t i = 0; i + 1 < size; i++)
                std::cerr << seq[i] << " ";

            std::cerr << seq[size - 1] << "\" was present\n";
        }
        return false;
    }

    hash_tables().at(id).insert(s);

    if (debug()) {
        std::cerr << "hash_insert: hash table #" << id << ", sequence \"";

        for (size_t i = 0; i + 1 < size; i++)
            std::cerr << seq[i] << " ";

        std::cerr << seq[size - 1] << "\" inserted\n";
    }
    return true;
}

bool hash_remove(unsigned long id, uint64_t const * seq, size_t size) {
    if (!test_data("hash_remove", id, seq, size))
        return false;

    if (hash_tables().count(id) == 0) {
        if (debug())
            std::cerr << "hash_remove: hash table #" << id << " does not exist\n";
        return false;
    }
    sequence_t s;

    for (size_t i = 0; i < size; i++)
        s.emplace_back(seq[i]);

    size_t ans = hash_tables().at(id).erase(s);

    if (debug()) {
        std::cerr << "hash_remove: hash table #" << id << ", sequence \"";

        for (size_t i = 0; i + 1 < size; i++)
            std::cerr << seq[i] << " ";

        std::cerr << seq[size - 1] << '"';

        if (ans == 1)
            std::cerr << " removed\n";
        else
            std::cerr << " was not present\n";
    }
    return bool(ans > 0);
}

void hash_clear(unsigned long id) {
    if (debug())
        std::cerr << "hash_clear(" << id << ")\n";

    if (hash_tables().count(id) == 0) {
        if(debug())
            std::cerr << "hash_clear: hash table #" << id << " does not exist\n";
        return;
    }
    if (hash_tables().at(id).size() == 0) {
        if (debug())
            std::cerr << "hash_clear: hash table #" << id << " was empty\n";
        return;
    }
    hash_tables().at(id).clear();

    if (debug())
        std::cerr << "hash_clear: hash table #" << id << " cleared\n";
}

bool hash_test(unsigned long id, uint64_t const * seq, size_t size) {
    if (!test_data("hash_test", id, seq, size))
        return false;

    if (hash_tables().count(id) == 0) {
        if (debug())
            std::cerr << "hash_test: hash table #" << id << " does not exist\n";
        return false;
    }
    sequence_t s;

    for (size_t i = 0; i < size; i++)
        s.emplace_back(seq[i]);

    size_t ans = hash_tables().at(id).count(s);

    if (debug()) {
        std::cerr << "hash_test: hash table #" << id << ", sequence \"";

        for (size_t i = 0; i + 1 < size; i++)
            std::cerr << seq[i] << " ";

        std::cerr << seq[size - 1] << '"';

        if (ans == 1)
            std::cerr << " is present\n";
        else
            std::cerr << " is not present\n";
    }

    return bool(ans > 0);
}
} // namespace jnp1