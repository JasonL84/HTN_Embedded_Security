# HTN_Embedded_Security
Demo scripts for the embedded cryptography workshop at Hack the North 2020++.

## List of Scripts

### speed_demo.go

> Run with: `go run speed_demo.go`
  
>  Ouputs: The time it takes to encrypt 1mb of randomly generated data with AES 256 and RSA 2048.

### generate_key.go

> Run with: `go run generate_key.go`
  
>  Ouputs: Public key as a PEM file (ed25519.pub), Private key raw bytes (ed25519.key), Public key as raw bytes (ed24419_public.key)

### arduino_demo_complete.ino 

> Load onto an arduino board

> The demo script used for an Intel Galileo board

### sample_server.py

> Run with: `python3 sample_server.py`

> The sample server that carries out the authentication
