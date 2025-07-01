#include "pre_scheme.h"
#include "network_utils.h"
#include <iostream>
#include <cstring>
#include <unistd.h>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <server_ip> <user_id>" << std::endl;
        return 1;
    }

    PREContext pre("params/a.param");
    const char* server_ip = argv[1];
    const char* user_id = argv[2];
    clock_t start, close;
    double cpu_time_used;

    // Generate user keys
    start = clock();
    pre.generate_user_keys();
    close = clock();
    std::cout << "=== Data User ===" << std::endl;
    std::cout << "User private key (b): ";
    element_printf("%B\n", pre.get_sk_user());
    std::cout << "User public key (g^b): ";
    element_printf("%B\n", pre.get_pk_user());
    cpu_time_used = ((double)(close - start)) / CLOCKS_PER_SEC;
    printf("Keygen time used: %lf seconds\n", cpu_time_used);

    // Connect to cloud server
    TCPClient client(server_ip, 8080);
    client.connect_to_server();
    
    // Upload public key
    client.send_data("UPLOAD_PK", 10);
    client.send_data(user_id, strlen(user_id) + 1);
    send_element(client.get_sock(), pre.get_pk_user());
    std::cout << "Uploaded user public key to server" << std::endl;
    client.close_connection();

    // Wait for data owner to process
    std::cout << "Waiting for data owner to encrypt data..." << std::endl;
    sleep(5);  // Give time for data owner to run

    // Reconnect to request re-encrypted ciphertext
    TCPClient client2(server_ip, 8080);
    client2.connect_to_server();
    std::cout << "\n" << std::endl; 
    start = clock();
    client2.send_data("REQUEST_CT", 11);
    element_t c1_re, c2_re;
    element_init_GT(c1_re, pre.get_pairing());
    element_init_GT(c2_re, pre.get_pairing());
    recv_element(client2.get_sock(), c1_re, pre.get_pairing());
    recv_element(client2.get_sock(), c2_re, pre.get_pairing());
    close = clock();
    std::cout << "------------------------------Received re-encrypted ciphertext------------------------------------------" << std::endl;
    std::cout << "Re-encrypted c1': ";
    element_printf("%B\n", c1_re);
    std::cout << "Re-encrypted c2': ";
    element_printf("%B\n", c2_re);
    cpu_time_used = ((double)(close - start)) / CLOCKS_PER_SEC;
    printf(" Received Re-key Ciphertext: %lf seconds\n", cpu_time_used);

    // Decrypt
    element_t m;
    std::cout << "\n" << std::endl; 
    element_init_GT(m, pre.get_pairing());
    start = clock();
    pre.decrypt(m, c1_re, c2_re, pre.get_sk_user());
    close = clock();
    std::cout << "--------------------------------------Decrypted message-------------------------------------------------\n";
    element_printf("%B\n", m);
    cpu_time_used = ((double)(close - start)) / CLOCKS_PER_SEC;
    printf("Message Decryption time: %lf seconds\n", cpu_time_used);

   
   
    
    // Cleanup
    element_clear(c1_re);
    element_clear(c2_re);
    element_clear(m);
    client2.close_connection();
    return 0;
}
