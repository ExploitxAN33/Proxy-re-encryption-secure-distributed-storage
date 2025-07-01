#include "pre_scheme.h"
#include <cstdio>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <time.h>
#include <chrono>

void print_timestamp(const char* label) {
    // Get system time in nanoseconds
    auto now = std::chrono::system_clock::now();
    auto now_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();

    // Convert to human-readable time
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm* local_time = std::localtime(&now_time);

    // Print label + human-readable + nanosecond timestamp
    std::cout << "[" << label << "] "
              << std::put_time(local_time, "%Y-%m-%d %H:%M:%S")
              << " | Timestamp (ns): " << now_ns << std::endl;
}

PREContext::PREContext(const char* param_path) {
    FILE* fp = fopen(param_path, "r");
    if (!fp) {
        perror("Error opening param file");
        exit(1);
    }
    char param[4096];
    size_t count = fread(param, 1, 4096, fp);
    fclose(fp);
    if (!count) {
        std::cerr << "Error: Failed to read param file" << std::endl;
        exit(1);
    }
    pairing_init_set_buf(pairing, param, count);
    element_init_G1(g, pairing);
    element_init_GT(Z, pairing);
    element_init_Zr(sk_owner, pairing);
    element_init_G1(pk_owner, pairing);
    element_init_Zr(sk_user, pairing);
    element_init_G1(pk_user, pairing);
    element_init_G1(rk, pairing);

    const char* fixed_seed = "fixed_generator_seed_for_project";
    element_from_hash(g, (void*)fixed_seed, strlen(fixed_seed));
    element_pairing(Z, g, g);
    print_timestamp("SETUP END");

}

PREContext::~PREContext() {
    element_clear(g);
    element_clear(Z);
    element_clear(sk_owner);
    element_clear(pk_owner);
    element_clear(sk_user);
    element_clear(pk_user);
    element_clear(rk);
    pairing_clear(pairing);

}

void PREContext::generate_owner_keys() {
    element_random(sk_owner);
    element_pow_zn(pk_owner, g, sk_owner);
    print_timestamp("KEYGEN END");

}

void PREContext::generate_user_keys() {
    element_random(sk_user);
    element_pow_zn(pk_user, g, sk_user);
    print_timestamp("KEYGEN END");

}

void PREContext::generate_re_key(element_t& rk, element_t sk_owner, element_t pk_user) {
    element_t inv_a;
    element_init_Zr(inv_a, pairing);
    element_invert(inv_a, sk_owner);
    element_pow_zn(rk, pk_user, inv_a);
    element_clear(inv_a);
    print_timestamp("REKEYGEN END");

}

void PREContext::encrypt(element_t& c1, element_t& c2, element_t m) {
    element_t k, ak, Zk;
    element_init_Zr(k, pairing);
    element_init_Zr(ak, pairing);
    element_init_GT(Zk, pairing);
    
    // Ensure k is non-zero
    do {
        element_random(k);
    } while (element_is0(k));
    
    // Compute ak = a * k
    element_mul(ak, sk_owner, k);
    
    // c1 = g^{a*k}
    element_pow_zn(c1, g, ak);
    
    // Zk = Z^k
    element_pow_zn(Zk, Z, k);
    
    // c2 = m * Z^k
    element_mul(c2, m, Zk);
    
    // Cleanup
    element_clear(k);
    element_clear(ak);
    print_timestamp("ENCRYPTION  END");

}


void PREContext::re_encrypt(element_t& c1_re, element_t& c2_re, element_t c1, element_t c2, element_t rk) {
    element_pairing(c1_re, c1, rk);
    element_set(c2_re, c2);
    print_timestamp("REENCRYPTION END");

}

void PREContext::decrypt(element_t& m, element_t c1_re, element_t c2_re, element_t sk_user) {
    element_t inv_b, Zk , Zk_inv;
    element_init_Zr(inv_b, pairing);
    element_init_GT(Zk, pairing);
    element_init_GT(Zk_inv , pairing);
        
    // Compute 1/b
    element_invert(inv_b, sk_user);
    
    // Compute Zk = (c1_re)^{1/b} = e(g,g)^k
    element_pow_zn(Zk, c1_re, inv_b);
    
    // Compute m = c2_re * (Zk)^{-1}
    element_invert(Zk_inv, Zk);
    element_mul(m, c2_re, Zk_inv);
    
    element_clear(inv_b);
    element_clear(Zk);
    element_clear(Zk_inv);
    print_timestamp("DECRYPTION REENCRYPTED END");

}

