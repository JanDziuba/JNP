#include "encstrset.h"

#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <optional>
#include <functional>
#include <cstring>
#include <sstream>
#include <iomanip>

using namespace std;

#ifdef NDEBUG
static const bool debug_enabled = false;
#else
static const bool debug_enabled = true;
#endif

#define debug_stream if (!debug_enabled) {} else cerr << __func__

namespace {

    using encrypted_string_t = string;
    using encrypted_set_t = unordered_set<encrypted_string_t>;

    // Zmienne globalne.

    unordered_map<unsigned long, encrypted_set_t> &encrypted_sets() {
        static auto *result = new unordered_map<unsigned long, encrypted_set_t>();
        return *result;
    }

    unsigned long &next_new_id() {
        static auto *result = new unsigned long();
        return *result;
    }

    // Funkcje pomocnicze.

    // Zwraca std::string "NULL", jeśli [c_string] jest nullem,
    // w przeciwnym przypadku zwraca [c_string] jako std::string otoczony
    // cudzysłowami.
    string get_quoted_string(const char *c_string) {
        if (c_string == nullptr)
            return "NULL";
        else
            return "\"" + string(c_string) + "\"";
    }

    // Zwraca string w postaci ciągu charów z encrypted_string zapisanych jako
    // liczby w systemie szesnastkowym oddzielone spacją.
    // Ciąg jest otoczony cudzysłowami.

    string get_hex_str(const encrypted_string_t &encrypted_str) {
        stringstream s_stream;

        s_stream << "\"";
        s_stream << uppercase << hex;

        bool first_char = true;

        for (char c: encrypted_str) {
            if (first_char)
                first_char = false;
            else
                s_stream << ' ';

            // Fragment "setw(2)" ustawia szerokość reprezentacji char'a w
            // systemie heksadecymalnym na dwie cyfry.
            // Fragment "0xffu & ..." chroni przed overflow spowodowanym przez
            // ujemne wartości char'a.
            s_stream << setfill('0') << setw(2) << (0xffu & uint32_t(c));

        }

        s_stream << "\"";

        return s_stream.str();
    }

    // Szyfruje ciąg znaków value kluczem key za pomocą operacji bitowej XOR.
    // Zwraca zaszyfrowany ciąg znaków.
    // Wartość value musi być różna od nullptr.
    encrypted_string_t encrypt(const char *value, const char *key) {

        if (value == nullptr) {
            exit(EXIT_FAILURE);
        }

        if (key == nullptr || strlen(key) == 0) {
            return string(value);
        }

        stringstream result;

        size_t key_len = strlen(key);
        size_t key_index = 0;

        for (; *value != '\0'; value++) {
            result << char(*value ^ key[key_index]);
            key_index = (key_index + 1) % key_len;
        }

        return result.str();
    }

    // Jeśli istnieje zbiór o podanym [id] to zwraca referencję do niego
    // owiniętą w optional'a. W przeciwnym wypadku zwraca pustego optional'a.
    optional<reference_wrapper<encrypted_set_t>> get_by_id(unsigned long id) {
        auto iterator = encrypted_sets().find(id);

        if (iterator != encrypted_sets().end())
            return iterator->second;
        else
            return nullopt;
    }
}

namespace jnp1 {

    unsigned long encstrset_new() {
        debug_stream << "()" << endl;
        encrypted_sets().emplace(next_new_id(), encrypted_set_t{});
        debug_stream << ": set #" << next_new_id() << " created" << endl;
        return next_new_id()++;
    }

    void encstrset_delete(unsigned long id) {
        debug_stream << "(" << id << ")" << endl;
        if (encrypted_sets().erase(id) != 0) {
            debug_stream << ": set #" << id << " deleted" << endl;
        } else {
            debug_stream << ": set #" << id << " does not exist" << endl;
        }
    }

    size_t encstrset_size(unsigned long id) {
        debug_stream << "(" << id << ")" << endl;
        auto optional_encrypted_set = get_by_id(id);

        if (optional_encrypted_set.has_value()) {
            size_t set_size = optional_encrypted_set->get().size();
            debug_stream << ": set #" << id << " contains " << set_size
                         << " element(s)" << endl;
            return set_size;
        } else {
            debug_stream << ": set #" << id << " does not exist" << endl;
            return 0;
        }
    }

    bool
    encstrset_insert(unsigned long id, const char *value, const char *key) {
        debug_stream << "(" << id << ", " << get_quoted_string(value) << ", "
                     << get_quoted_string(key) << ")" << endl;

        if (value == nullptr) {
            debug_stream << ": invalid value (NULL)" << endl;
            return false;
        }

        auto optional_encrypted_set = get_by_id(id);

        if (optional_encrypted_set.has_value()) {
            encrypted_set_t &encrypted_set = optional_encrypted_set->get();
            encrypted_string_t encrypted_string = encrypt(value, key);

            if (encrypted_set.count(encrypted_string)) {
                debug_stream << ": set #" << id << ", cypher "
                             << get_hex_str(encrypted_string)
                             << " was already present" << endl;
                return false;
            } else {
                encrypted_set.emplace(encrypted_string);
                debug_stream << ": set #" << id << ", cypher "
                             << get_hex_str(encrypted_string) << " inserted"
                             << endl;
                return true;
            }
        } else {
            debug_stream << ": set #" << id << " does not exist" << endl;
            return false;
        }
    }

    bool
    encstrset_remove(unsigned long id, const char *value, const char *key) {
        debug_stream << "(" << id << ", " << get_quoted_string(value) << ", "
                     << get_quoted_string(key) << ")" << endl;

        if (value == nullptr) {
            debug_stream << ": invalid value (NULL)" << endl;
            return false;
        }

        auto optional_encrypted_set = get_by_id(id);

        if (optional_encrypted_set.has_value()) {
            encrypted_set_t &encrypted_set = optional_encrypted_set->get();
            encrypted_string_t encrypted_string = encrypt(value, key);

            if (encrypted_set.count(encrypted_string)) {
                encrypted_set.erase(encrypted_string);
                debug_stream << ": set #" << id << ", cypher "
                             << get_hex_str(encrypted_string) << " removed"
                             << endl;
                return true;
            } else {
                debug_stream << ": set #" << id << ", cypher "
                             << get_hex_str(encrypted_string)
                             << " was not present" << endl;
                return false;
            }
        } else {
            debug_stream << ": set #" << id << " does not exist" << endl;
            return false;
        }
    }

    bool encstrset_test(unsigned long id, const char *value, const char *key) {
        debug_stream << "(" << id << ", " << get_quoted_string(value) << ", "
                     << get_quoted_string(key) << ")" << endl;

        if (value == nullptr) {
            debug_stream << ": invalid value (NULL)" << endl;
            return false;
        }

        auto optional_encrypted_set = get_by_id(id);

        if (optional_encrypted_set.has_value()) {
            const encrypted_set_t &encrypted_set = optional_encrypted_set->get();
            encrypted_string_t encrypted_string = encrypt(value, key);

            if (encrypted_set.count(encrypted_string)) {
                debug_stream << ": set #" << id << ", cypher "
                             << get_hex_str(encrypted_string) << " is present"
                             << endl;
                return true;
            } else {
                debug_stream << ": set #" << id << ", cypher "
                             << get_hex_str(encrypted_string)
                             << " is not present" << endl;
                return false;
            }
        } else {
            debug_stream << ": set #" << id << " does not exist" << endl;
            return false;
        }
    }

    void encstrset_clear(unsigned long id) {
        debug_stream << "(" << id << ")" << endl;

        auto optional_encrypted_set = get_by_id(id);

        if (optional_encrypted_set.has_value()) {
            encrypted_set_t &encrypted_set = optional_encrypted_set->get();
            encrypted_set.clear();
            debug_stream << ": set #" << id << " cleared" << endl;
        } else {
            debug_stream << ": set #" << id << " does not exist" << endl;
        }
    }

    void encstrset_copy(unsigned long src_id, unsigned long dst_id) {
        debug_stream << "(" << src_id << ", " << dst_id << ")" << endl;

        auto optional_enc_src_set = get_by_id(src_id);
        auto optional_enc_dst_set = get_by_id(dst_id);

        if (!optional_enc_src_set.has_value()) {
            debug_stream << ": set #" << src_id << " does not exist" << endl;
        } else if (!optional_enc_dst_set.has_value()) {
            debug_stream << ": set #" << dst_id << " does not exist" << endl;
        } else {
            const encrypted_set_t &enc_src_set = optional_enc_src_set->get();
            encrypted_set_t &enc_dst_set = optional_enc_dst_set->get();

            for (const encrypted_string_t &enc_str : enc_src_set) {
                if (enc_dst_set.count(enc_str)) {
                    debug_stream << ": copied cypher " << get_hex_str(enc_str)
                                 << " was already present in set #" << dst_id
                                 << endl;
                } else {
                    enc_dst_set.insert(enc_str);
                    debug_stream << ": cypher " << get_hex_str(enc_str)
                                 << " copied from set #" << src_id
                                 << " to set #" << dst_id << endl;
                }
            }
        }
    }
}
