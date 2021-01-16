import socket
from nacl.signing import VerifyKey
def verify_challenge(challenge_resp, ip_address):

	pub_key_f = open("ed25519_public.key", "rb")
	pub_key = pub_key_f.read()

	verify_key = VerifyKey(pub_key)

	try:
		verify_key.verify(bytes(ip_address, 'utf-8'), challenge_resp)
	except:
		print("Verification failure")
		return None

	print("Verification success")
	return ip_address

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
	server.bind(('192.168.1.29', 19997))
	while True:
		server.listen(1)

		conn, addr = server.accept()
		
		conn.sendall(len(addr[0]).to_bytes(1, 'big'))
		print(len(addr[0]))
		conn.sendall(bytes(addr[0], 'utf-8'))

		resp = conn.recv(64)

		hostname = verify_challenge(resp, addr[0])

		if hostname:
			ping = bytes("PING", 'utf-8')
			conn.sendall(len(ping).to_bytes(1, 'big'))
			conn.sendall(ping)
			resp = conn.recv(64)
		conn.close()

