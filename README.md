# 🔥 Real-Time Multi-Client Chat Application

[![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](https://opensource.org/licenses/MIT)

A real-time chat application developed in C++ using TCP/IP sockets and multi-threading to allow multiple clients to connect and communicate simultaneously.

## 🌟 Features
- **Multi-Client Support**: Multiple users can connect simultaneously.
- **Broadcast Messaging**: Messages sent by one user are relayed to all clients.
- **Non-Blocking Sockets**: Sockets operate in non-blocking mode for better performance.
- **Thread Management**: Dedicated threads for each client to handle concurrent operations.
- **Detailed Logging**: Server logs connection events and errors in `server_logs.txt`.

## 🛠️ Technologies
- **Language**: C++17
- **Network Communication**: TCP/IP Sockets
- **Concurrency**: `std::thread`, Mutex (`std::mutex`)
- **Logging**: File-based logging system.

## 🚀 Installation
### Requirements
- Linux/macOS (Use [WSL](https://docs.microsoft.com/en-us/windows/wsl/install) for Windows)
- GCC (with C++17 support)

### Steps
1. **Clone the Repository**:
   ```bash
   git clone https://github.com/your-username/realtime-chat.git
   cd realtime-chat
   ```

2. **Compile and Run the Server**:
   ```bash
   g++ server.cpp -o server -lpthread
   ./server
   ```

3. **Compile and Run the Client** (In Separate Terminals):
   ```bash
   g++ client.cpp -o client
   ./client
   ```

## 📖 Usage
1. Start the server (default port: `8080`).
2. Connect multiple clients.
3. Type a message and press `Enter` to broadcast it to all clients.
4. Type `exit` to disconnect.

**Example Client Output**:
```
Connected to server. Type messages and press Enter.
Enter message: Hello!
Server: Client 4: Hello!
```

## 🐛 Error Handling
- **Connection Drops**: Server automatically detects disconnected clients.
- **Thread Safety**: Client list and log file are protected with `std::mutex`.
- **Error Codes**: Detailed logging of socket errors.

## 🤝 Contributing
Contributions are welcome! Please follow these steps:
1. Fork the repo (https://github.com/your-username/realtime-chat/fork)
2. Create a branch (`git checkout -b feature/fooBar`)
3. Commit changes (`git commit -am 'Add some fooBar'`)
4. Push to the branch (`git push origin feature/fooBar`)
5. Create a Pull Request

## 📜 License
This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.

## 📞 Contact
Questions? Reach out:  
[![LinkedIn](https://img.shields.io/badge/LinkedIn-Your_Profile-blue.svg)](https://www.linkedin.com/in/togunchan/)
