#ifndef PRE_SCHEME_H
#define PRE_SCHEME_H

#include <pbc/pbc.h>
#include <iostream>

class PREContext {
public:
    PREContext(const char* param_path);
    ~PREContext();
    
    void generate_owner_keys();
    void generate_user_keys();
    void generate_re_key(element_t& rk, element_t sk_owner, element_t pk_user);
    void encrypt(element_t& c1, element_t& c2, element_t m);
    void re_encrypt(element_t& c1_re, element_t& c2_re, element_t c1, element_t c2 , element_t rk);
    void decrypt(element_t& m, element_t c1_re, element_t c2_re, element_t sk_user);
    
    element_t& get_sk_owner() { return sk_owner; }
    element_t& get_pk_owner() { return pk_owner; }
    element_t& get_sk_user() { return sk_user; }
    element_t& get_pk_user() { return pk_user; }
    element_t& get_rk() { return rk; }
    element_t& get_g() { return g; }
    element_t& get_Z() { return Z; }

    pairing_ptr get_pairing() { return pairing; }

private:
    pairing_t pairing;
    element_t g;
    element_t Z;
    element_t sk_owner;
    element_t pk_owner;
    element_t sk_user;
    element_t pk_user;
    element_t rk;
};

#endif // PRE_SCHEME_H
