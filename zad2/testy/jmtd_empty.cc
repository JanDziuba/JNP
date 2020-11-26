#include "../encstrset.h"

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <cassert>

using namespace ::jnp1;

#define NOT !

int main() {
    const unsigned int id = encstrset_new();
    assert(encstrset_size(id) == 0);
    encstrset_copy(id, id);
    assert(NOT encstrset_remove(id, "", ""));
    assert(NOT encstrset_test(id, "", ""));
    encstrset_clear(id);

    assert(encstrset_insert(id, "", ""));
    encstrset_delete(id);
}
