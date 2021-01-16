package main

import (
	"crypto"
	"crypto/rsa"
	"crypto/aes"
	"crypto/rand"
	_ "crypto/sha256"
	math_rand "math/rand"
	"fmt"
	"time"
)

func check(e error) {
    if e != nil {
        panic(e)
    }
}

func main() {
	fmt.Println("Benchmarking RSA encryption and AES encryption by comparing the time taken to encrypt 1MB of random data")
	rand_data:= make([]byte, 1000000)
	aes_key := make([]byte, 32)
	_, err := math_rand.Read(aes_key)
	check(err)

	_,err = math_rand.Read(rand_data)
	check(err)
	rand_data_aes := rand_data

	rng := rand.Reader
	rsa_priv_key, err := rsa.GenerateKey(rng, 2048)
	check(err)
	rsa_pub_key := rsa_priv_key.PublicKey

	fmt.Println("Generated RSA and AES keys")

	fmt.Println("Benchmarking RSA")
	start_time := time.Now()
	segment := 2048 / 8
	data := make([]byte, 0)
	for i := range rand_data {
		start := i * segment / 2
		end := start
		if start+segment/2 < len(rand_data) {
			end = start + segment/2
		} else {
			end = len(rand_data)
		}
		byteSequence := rand_data[start:end]

		segmentEncrypt, err := rsa.EncryptOAEP(crypto.SHA256.New(), rand.Reader, &rsa_pub_key, byteSequence,
		 nil)
		
		check(err)

		data = append(data, segmentEncrypt...)

		if end == len(rand_data) {
			break
		}
	}
	end_time := time.Now()
	elapsed := end_time.Sub(start_time)
	fmt.Println("Time taken for RSA: ",elapsed)

	fmt.Println("Benchmarking AES")

	start_time = time.Now()
	block, _ := aes.NewCipher(aes_key)
	dst := make([]byte, 1000000)

	block.Encrypt(dst, rand_data_aes)
	end_time = time.Now()
	elapsed = end_time.Sub(start_time)
	fmt.Println("Time taken for AES: ",elapsed)
}