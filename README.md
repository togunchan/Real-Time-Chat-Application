# Real-Time Multi-Client Chat Application

A TCP/IP chat system written in modern C++ that lets multiple clients exchange messages through a multithreaded server. The project targets macOS or Linux (Windows users can rely on WSL) and comes with lightweight tooling to keep logs and binaries out of version control.

## Features
- **Multi-client broadcasting**: every message from a client is relayed to all connected peers.
- **Threaded connection handling**: each client runs on its own `std::thread`, with shared state protected by `std::mutex`.
- **Non-blocking server reads**: client sockets run in non-blocking mode to prevent stalled threads.
- **File-backed logging**: connection and error events are appended to `server_logs.txt`.
- **Quick integration smoke test**: simple Python-based test script (see below) to validate end-to-end messaging.

## Repository Layout
- `server.cpp` – TCP server accepting clients, broadcasting messages, and writing logs.
- `client.cpp` – Interactive CLI client that connects to the server and sends user input.
- `.vscode/` – Editor configuration for VS Code IntelliSense on macOS.
- `.gitignore` – Filters build artefacts (e.g., `server`, `client`, `server_test`, logs, object files).

## Prerequisites
- macOS or Linux
- Xcode Command Line Tools (macOS) or GCC/Clang with C++17 support
- POSIX sockets (`arpa/inet.h`, `sys/socket.h`, etc.)
- `python3` (optional, for the smoke test)

If you are on macOS, install the command line tools via:

```bash
xcode-select --install
```

## Building
Compile the binaries with any C++17-capable compiler:

```bash
# Build the server
g++ server.cpp -std=c++17 -pthread -o server

# Build the client
g++ client.cpp -std=c++17 -pthread -o client

# Optional: build the refactored test binaries
g++ server.cpp -std=c++17 -pthread -o server_test
g++ client.cpp -std=c++17 -pthread -o client_test
```

All executables are already ignored by `.gitignore`.

## Usage
1. Start the server (default port `8080`):
   ```bash
   ./server
   ```
2. In separate terminals, start as many clients as you like:
   ```bash
   ./client
   ```
3. Type messages and press **Enter** to broadcast to everyone.
4. Type `exit` from any client to disconnect gracefully.

### Example Client Session
```
Connected to server. Type messages and press Enter to send.
Enter message: Hello!
Message sent

Server: Client 5: Hello!
Enter message:
```

## Logging & Troubleshooting
- Server events are stored in `server_logs.txt`. The file grows indefinitely; rotate or clear it as needed.
- If the client reports `Invalid address`, confirm `SERVER_ADDRESS` matches your server host.
- macOS users seeing `arpa/inet.h` errors should ensure the Command Line Tools are installed or configure VS Code IntelliSense via `.vscode/c_cpp_properties.json`.

## Integration Smoke Test
You can run a quick end-to-end test after building `server_test`:

```bash
./server_test &
SERVER_PID=$!
sleep 1
python3 scripts/smoke_test.py  # or adapt the inline Python snippet below
kill $SERVER_PID
```

Inline Python snippet (no separate file required):

```bash
python3 - <<'PY'
import socket
import time

HOST = '127.0.0.1'
PORT = 8080

clients = []
for _ in range(2):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((HOST, PORT))
    clients.append(s)

time.sleep(0.5)
clients[0].sendall(b'Hello from test!\n')

time.sleep(0.5)
print(clients[1].recv(1024).decode())

for s in clients:
    s.close()
PY
```

## Contributing
1. Fork the repository and create a feature branch.
2. Follow the existing coding style (C++17, mutex-protected shared state).
3. Add tests or reproduce steps when fixing bugs.
4. Submit a pull request describing the change and any verification performed.

## License
This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.

## Contact
Questions, feedback, ideas?

[![LinkedIn](https://img.shields.io/badge/LinkedIn-Murat_Toğunçhan_Düzgün-blue.svg)](https://www.linkedin.com/in/togunchan/)
[![GitHub](https://img.shields.io/badge/GitHub-togunchan-black.svg)](https://github.com/togunchan)
