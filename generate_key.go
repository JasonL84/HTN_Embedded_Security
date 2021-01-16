package main

import (
	"fmt"
	"io/ioutil"
)
import "encoding/pem"
import "github.com/spacemonkeygo/openssl"

func check(e error) {
    if e != nil {
        panic(e)
    }
}

func main() {
	fmt.Println("Generating an ED25519 private and public keypair")
	privKey, _ := openssl.GenerateED25519Key()
	privKeyPem, _ := privKey.MarshalPKCS1PrivateKeyPEM()

	pubKeyPem, _ := privKey.MarshalPKIXPublicKeyPEM()

	block, _ := pem.Decode(privKeyPem)
	key := block.Bytes[len(block.Bytes)-32:]

	fmt.Println(key)
	err := ioutil.WriteFile("ed25519.key", key, 0644)
	check(err)

	block, _ = pem.Decode(pubKeyPem)
	key = block.Bytes[len(block.Bytes)-32:]

	err = ioutil.WriteFile("ed25519_public.key", key, 0644)
	check(err)

	err = ioutil.WriteFile("ed25519.pub", pubKeyPem, 0644)
	check(err)
}