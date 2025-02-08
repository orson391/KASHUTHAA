import asyncio
import websockets
import socket

TCP_HOST = "127.0.0.1"  # SDL_net server address
TCP_PORT = 12345        # SDL_net server port
WS_PORT = 8080          # WebSocket proxy port

async def handle_client(websocket, path):
    print("WebSocket client connected")

    # Connect to the TCP server
    tcp_client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    tcp_client.connect((TCP_HOST, TCP_PORT))

    async def receive_from_ws():
        """Receive messages from WebSocket and send to TCP server."""
        try:
            async for message in websocket:
                print(f"Received from WebSocket: {message}")
                tcp_client.sendall(message.encode())  # Send to TCP server
        except:
            pass

    def receive_from_tcp():
        """Receive messages from TCP and send to WebSocket."""
        try:
            while True:
                data = tcp_client.recv(512)
                if not data:
                    break
                print(f"Received from TCP: {data.decode()}")
                asyncio.run(websocket.send(data.decode()))  # Send to WebSocket
        except:
            pass

    # Run both send and receive operations
    loop = asyncio.get_event_loop()
    tcp_task = loop.run_in_executor(None, receive_from_tcp)
    ws_task = receive_from_ws()

    await asyncio.gather(ws_task, tcp_task)

    print("Connection closed")
    tcp_client.close()

# Start WebSocket server
start_server = websockets.serve(handle_client, "0.0.0.0", WS_PORT)
print(f"WebSocket-to-TCP proxy running on ws://localhost:{WS_PORT}")

asyncio.get_event_loop().run_until_complete(start_server)
asyncio.get_event_loop().run_forever()
