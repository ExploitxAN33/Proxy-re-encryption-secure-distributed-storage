#include "pre_scheme.h"
#include "network_utils.h"
#include <iostream>
#include <cstring>

// NOTE: For demo simplicity, we use global variables for storage.
// For production, use a custom wrapper or manage memory manually.

element_t pk_user, rk, ct_c1, ct_c2;

int main() {
    PREContext pre("params/a.param");
    element_init_G1(pk_user, pre.get_pairing());
    element_init_G1(rk, pre.get_pairing());
    element_init_G1(ct_c1, pre.get_pairing());
    element_init_GT(ct_c2, pre.get_pairing());
    clock_t start, close;
    double cpu_time_used;


    TCPServer server(8080);
    server.start();
    std::cout << "Cloud Server listening on port 8080..." << std::endl;

    while (true) {
        int client_fd = server.accept_connection();
        char command[20] = {0};
        server.recv_data(client_fd, command, 20);

        if (strcmp(command, "UPLOAD_PK") == 0) {
            char user_id[50] = {0};
            start = clock();
            server.recv_data(client_fd, user_id, 50);
            recv_element(client_fd , pk_user, pre.get_pairing());
            close = clock();
            cpu_time_used = ((double)(close - start)) / CLOCKS_PER_SEC;
            printf("Stored Public key %lf time\n", cpu_time_used);
            std::cout << "Stored public key for user: " << user_id << std::endl;
            std::cout << "User public key: ";
            element_printf("%B\n", pk_user);
        }
        else if (strcmp(command, "GET_PK") == 0) {
            char user_id[50] = {0};
            start = clock();
            server.recv_data(client_fd, user_id, 50);
            close = clock();
            cpu_time_used = ((double)(close - start)) / CLOCKS_PER_SEC;
            printf("send public key to data_owner %lf  time\n", cpu_time_used);
            send_element(client_fd, pk_user);
            std::cout << "Sent user public key to requester" << std::endl;
            std::cout << "Waiting 10 seconds for data owner..." << std::endl;
            sleep(10);
        }
        else if (strcmp(command, "UPLOAD_KEY") == 0) {
            char user_id[50] = {0};
            std::cout << "\n\n" << std::endl; 
            start = clock();
            server.recv_data(client_fd, user_id, 50); 
            recv_element(client_fd, rk, pre.get_pairing());
            close = clock();
            cpu_time_used = ((double)(close - start)) / CLOCKS_PER_SEC;
            printf("Received Reencryption %lf time \n", cpu_time_used);
            std::cout << "Received re-encryption key from data_owner=================================================" << std::endl;
            std::cout << "Re-encryption key: ";
            element_printf("%B\n", rk);
        }
        else if (strcmp(command, "UPLOAD_CT") == 0) {
            char user_id[50] = {0};
            std::cout << "\n\n" << std::endl; 
            start = clock();
            server.recv_data(client_fd, user_id, 50);  // Receive user ID first!
            recv_element(client_fd, ct_c1, pre.get_pairing());
            recv_element(client_fd, ct_c2, pre.get_pairing());
            close = clock();
            cpu_time_used = ((double)(close - start)) / CLOCKS_PER_SEC;
            printf("Received ciphertext %lf  time  \n", cpu_time_used);
            std::cout << "Received ciphertext" << std::endl;
            std::cout << "Ciphertext c1: ";
            element_printf("%B\n", ct_c1);
            std::cout << "Ciphertext c2: ";
            element_printf("%B\n", ct_c2);
        }
        else if (strcmp(command, "REQUEST_CT") == 0) {
            element_t c1_re, c2_re;
            element_init_GT(c1_re, pre.get_pairing());
            element_init_GT(c2_re, pre.get_pairing());
            std::cout << "\n\n" << std::endl; 
            start = clock();
            pre.re_encrypt(c1_re, c2_re, ct_c1, ct_c2, rk);
            close = clock();
            cpu_time_used = ((double)(close - start)) / CLOCKS_PER_SEC; 
            printf("Re-encrypted cipher text %lf  time  \n", cpu_time_used);
            // Debug: print pairing and operand
            std::cout << "=============== Cloud Server through generation of re-key CT ==================" << std::endl;
            std::cout << "Re-encrypted c1': ";
            element_printf("%B\n", c1_re);
            std::cout << "Re-encrypted c2': ";
            element_printf("%B\n", c2_re);

            send_element(client_fd, c1_re);
            send_element(client_fd, c2_re);

            element_clear(c1_re);
            element_clear(c2_re);
        }
        else 
        {
            std::cerr << "Unknown command received: " << command << std::endl;
        }
        server.close_connection(client_fd);
    }
    return 0;
}
