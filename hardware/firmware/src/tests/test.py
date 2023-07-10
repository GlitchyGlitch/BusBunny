import socket

HOST = "192.168.4.1"
PORT = 9002


def test_alphanumeric_loopback():
    data = b"\x06hello"
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        s.send(data)
        received = s.recv(1024)
    
    if data != received:
        print("[+] Passed!")
 
def test_binary_loopback_with_zeros():
    data = b"\x08hell\x00\x11oo"
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        s.send(data)
        received = s.recv(1024)
    
    print(data, received)
    if data == received:
        print("[+] Passed!")

def test():
    print("Test entered")
    data = b"\x05\x00asdf"
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        for _ in range(1024):
            received = s.recv(1024)
            print(data, received)
            if data == received:
                print("[+] Passed!")

if __name__ == "__main__":
    print("You need to have the device with your firmwere atached!")
    test()
