#include "pre_scheme.h"
#include "network_utils.h"
#include <iostream>
#include <cstring>

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <server_ip> <owner_id> <user_id>" << std::endl;
        return 1;
    }

    PREContext pre("params/a.param");
    const char* server_ip = argv[1];
    const char* owner_id = argv[2];
    const char* user_id = argv[3];
    clock_t start, close;
    double cpu_time_used;
    
    // Generate owner keys
    start = clock();
    pre.generate_owner_keys();
    close = clock();
    std::cout << "------------------------------------------ Data Owner -----------------------------------------------------------" << std::endl;
    std::cout << "Owner private key (a): ";
    element_printf("%B\n", pre.get_sk_owner());
    std::cout << "Owner public key (g^a): ";
    element_printf("%B\n", pre.get_pk_owner());
    cpu_time_used = ((double)(close - start)) / CLOCKS_PER_SEC;
    printf("Keygen time used: %lf seconds\n", cpu_time_used);


    // Connect to cloud server
    TCPClient client(server_ip, 8080);
    client.connect_to_server();
   
    std::cout << "\n" << std::endl; 
    // Request user's public key
    start = clock();
    client.send_data("GET_PK", 7);
    client.send_data(user_id, strlen(user_id) + 1);
    element_t pk_user;
    element_init_G1(pk_user, pre.get_pairing());
    recv_element(client.get_sock(), pk_user, pre.get_pairing());
    close = clock();
    std::cout << "Received user public key (g^b): ";
    element_printf("%B\n", pk_user);
    cpu_time_used = ((double)(close - start)) / CLOCKS_PER_SEC;
    printf("Delegatee's public key received time used: %lf seconds\n", cpu_time_used);


    std::cout << "\n---------------------------------------------------------------------------------------------\n" << std::endl; 

    // Generate re-encryption key
    element_t rk;
    element_init_G1(rk, pre.get_pairing());
    start = clock();
    pre.generate_re_key(rk, pre.get_sk_owner(), pk_user);
    close = clock();
    std::cout << "Generated re-encryption key (g^{b/a}): ";
    element_printf("%B\n", rk);
    cpu_time_used = ((double)(close - start)) / CLOCKS_PER_SEC;
    printf("Re-encryption time gen: %lf seconds\n", cpu_time_used);

    // Encrypt message
    element_t m, c1, c2;
    element_init_GT(m, pre.get_pairing());
    element_init_G1(c1, pre.get_pairing());
    element_init_GT(c2, pre.get_pairing());
    std::cout << "\n-------------------------------------------------------------------------------------------\n" << std::endl; 
    start = clock();
    const char* fixed_msg = "HelloPRE123!";  
    element_from_hash(m, (void*)fixed_msg, strlen(fixed_msg));
    close = clock();
    cpu_time_used = ((double)(close - start)) / CLOCKS_PER_SEC;
    printf("Message generate time : %lf seconds\n", cpu_time_used);


    // Debug print
    std::cout << "Fixed message (from hash): ";   
    element_printf("%B\n", m);
    std::cout << "\n--------------------------------------------------------------------------------------------\n" << std::endl; 
    start = clock();
    pre.encrypt(c1, c2, m);
    close = clock(); 
    std::cout << "Ciphertext c1 (g^{a*k}): ";
    element_printf("%B\n", c1);
    std::cout << "Ciphertext c2 (m * e(g,g)^k): ";
    element_printf("%B\n", c2);
    cpu_time_used = ((double)(close - start)) / CLOCKS_PER_SEC; 
    printf("Ciphertext generate time : %lf seconds\n", cpu_time_used);


    // Send re-encryption key and ciphertext to server
    TCPClient client_key(server_ip, 8080);
    client_key.connect_to_server();
    client_key.send_data("UPLOAD_KEY", 11);
    client_key.send_data(user_id, strlen(user_id) + 1);
    send_element(client_key.get_sock(), rk);
    client_key.close_connection();
    
    TCPClient client_ct(server_ip, 8080);
    client_ct.connect_to_server();
    client_ct.send_data("UPLOAD_CT", 10);
    client_ct.send_data(user_id, strlen(user_id) + 1);
    send_element(client_ct.get_sock(), c1);
    send_element(client_ct.get_sock(), c2);
    client_ct.close_connection();
     
    

    // Cleanup
    element_clear(pk_user);
    return 0;
}
