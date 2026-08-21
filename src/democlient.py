import socket

def main():
    host = "127.0.0.1"
    port = 54000  # change this to whatever port your server listens on

    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect((host, port))

    print(f"Connected to {host}:{port}")

    try:
        while True:
            message = input("> ")
            if message.lower() in ("quit", "exit"):
                break

            client_socket.sendall(message.encode())

            response = client_socket.recv(4096)
            if not response:
                print("Server closed the connection")
                break

            print(response.decode(errors="replace"))
    finally:
        client_socket.close()

if __name__ == "__main__":
    main()
