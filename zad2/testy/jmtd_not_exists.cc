#include "../encstrset.h"

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <cassert>

using namespace ::jnp1;

#define NOT !

int main() {
    const unsigned int not_id = 2137;
    encstrset_delete(not_id);
    assert(encstrset_size(not_id) == 0);
    assert(NOT encstrset_insert(not_id, "", ""));
    assert(NOT encstrset_remove(not_id, "", ""));
    assert(NOT encstrset_test(not_id, "", ""));
    encstrset_clear(not_id);
    encstrset_copy(not_id, not_id);
    const unsigned int id = encstrset_new();
    encstrset_copy(not_id, id);
    encstrset_copy(id, not_id);
}
