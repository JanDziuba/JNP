#include "../encstrset.h"

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <cassert>
#include <string>

using namespace ::jnp1;

#define NOT !

int main() {
    const unsigned int id1 = encstrset_new(), id2 = encstrset_new(), id3 = encstrset_new();
    encstrset_delete(id3);
    encstrset_delete(id3);

    std::string s1_cpp = "ala", s2_cpp = "ma", s3_cpp = "kot";
    const char *s1 = s1_cpp.c_str(), *s2 = s2_cpp.c_str(), *s3 = s3_cpp.c_str();

    encstrset_insert(id1, s1, s1);
    assert(encstrset_test(id1, s1, s1));
    assert(encstrset_test(id1, s3, s3));
    encstrset_insert(id1, s3, s3);
    assert(encstrset_size(id1) == 1);
    assert(encstrset_remove(id1, s3, s3));
    assert(encstrset_size(id1) == 0);

    encstrset_insert(id1, s3, s3);
    encstrset_insert(id1, s3, s1);
    encstrset_insert(id1, s1, s3);
    encstrset_insert(id1, s1, s1);
    assert(encstrset_size(id1) == 2);
    encstrset_clear(id1);
    assert(encstrset_size(id1) == 0);

    encstrset_insert(id1, s1, s1);
    encstrset_insert(id1, s2, s1);
    encstrset_insert(id1, s1, s2);
    encstrset_insert(id1, s2, s2);
    assert(encstrset_size(id1) == 4);

    encstrset_insert(id2, s1, s3);
    assert(encstrset_size(id2) == 1);

    s1_cpp[2] = 'k';
    assert(encstrset_test(id1, s1, s1));
    assert(NOT encstrset_test(id1, s1, s2));
    assert(encstrset_test(id1, s2, s1));

    encstrset_clear(id2);
    assert(encstrset_insert(id2, s1, s2));
    assert(NOT encstrset_insert(id2, s1, (s2_cpp + s2_cpp).c_str()));

    assert(encstrset_size(id1) == 4);
    assert(encstrset_size(id2) == 1);
    assert(encstrset_insert(id1, s1, s2));
    assert(encstrset_size(id1) == 5);

    encstrset_copy(id2, id1);
    assert(encstrset_size(id1) == 5);
    assert(encstrset_size(id2) == 1);

    assert(encstrset_remove(id1, s1, s2));
    encstrset_copy(id1, id2);
    assert(encstrset_size(id1) == 4);
    assert(encstrset_size(id2) == 5);
}
