# Proxy-re-encryption-secure-communication-cloud-computing (Alice-Bob)

A secure, distributed storage system using proxy re-encryption and TCP connections.  
**Roles:** Cloud Server, Data User, Data Owner.

---

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Requirements](#requirements)
- [Installation](#installation)
- [How to Run](#how-to-run)
- [Decryption Guide](#decryption-guide)

---

## Overview

This project implements a secure distributed storage system using proxy re-encryption.  
**All communication between modules is handled using TCP sockets.**

- **Cloud Server:** Manages encrypted file storage and re-encryption.
- **Data Owner:** Uploads and manages encrypted data.
- **Data User:** Requests, receives, and decrypts data.

---

## Features

- Proxy re-encryption for secure, flexible data sharing
- End-to-end encryption: Only authorized users can decrypt data
- Modular design with clear roles (Cloud Server, Data Owner, Data User)
- All communication over TCP sockets

---

## Requirements

- C++17 or higher
- CMake (3.10+)
- OpenSSL (for cryptography)
- Linux or Windows (tested on Ubuntu 20.04+)
- Git

---

## Installation

1. **Clone the repository:**
    ```
    git clone https://github.com/ExploitxAN33/elgamal-PRE-secure-cloud-storage-attempt2.git
    cd elgamal-PRE-secure-cloud-storage-attempt2
    ```

2. **Install dependencies:**
    On Ubuntu/Debian:
   ```
     sudo apt-get update
     sudo apt-get install build-essential cmake libssl-dev
   ```

3. **Build the project:**
    ```
    cd build
    cmake ..
    make
    ```
    Binaries for `cloud_server`, `data_owner`, and `data_user` will be generated in the `build` directory.

---

## How to Run

**Order of Execution (with TCP connections):**

1. **Start the Cloud Server** (must be running before other modules):
    ```
    cd ..
    ./Build/cloud_server 
    ```
    - The server listens for TCP connections from data users and data owners.

2. **Run Data User** (connects to the cloud server via TCP):
    ```
    ./Build/data_user 127.0.0.1 alice
    ```
    - Initiates a TCP connection to the cloud server.
    - Requests files and handles decryption keys.

3. **Run Data Owner** (connects to the cloud server via TCP):
    ```
    ./Build/data_owner 127.0.0.1 alice bob
    ```
    - Initiates a TCP connection to the cloud server.
    - Uploads and manages encrypted files.

> **Note:**  
> - Each component must be run in a separate terminal or process.  
> - Ensure the server address and port are correctly configured in your configuration files or command-line arguments.

---

## Decryption Guide

To **decrypt a file as a Data User**:

1. Ensure the **cloud server** is running and you are connected via TCP.
2. Run the **data_user** binary and follow prompts to request the encrypted file.
3. When prompted, provide your private decryption key.
4. The decrypted file will be saved in your specified output directory.

**Example Command:**
![image](https://github.com/user-attachments/assets/7e64c052-d439-44de-82d8-1a0e73c458cd)
![image](https://github.com/user-attachments/assets/e8e5fcb7-5a7a-4804-a025-7f5ee8192f4b)
![image](https://github.com/user-attachments/assets/75edb986-16de-4149-bb5f-b300e7ac5ad0)
![image](https://github.com/user-attachments/assets/11710519-33f0-4043-a59b-18dfe5bc380c)
![image](https://github.com/user-attachments/assets/d7b324ea-37b6-4421-a83b-a49b95ec9949)




