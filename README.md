# One-time-Pad
Definition:
Primary reference on One-Time Pads (OTP):https://en.wikipedia.org/wiki/One-time_pad

Plaintext is the term for the information that you wish to encrypt and protect. It is human readable.

Ciphertext is the term for the plaintext after it has been encrypted by your programs. Ciphertext is not human-readable, and in fact cannot be cracked, if the OTP system is used correctly.

A Key is the random sequence of characters that will be used to convert Plaintext to Ciphertext, and back again. It must not be re-used, or else the encryption is in danger of being broken.

This program will encrypt and decrypt plaintext into ciphertext, using a key. All 27 characters will be encrypted and decrypted as above.

keygen.c:
This program creates a key file of specified length. The characters in the file generated will be any of the 27 allowed characters.

otp_enc_d.c:
This program will run in the background as a daemon. It will do the one-time pad encryption work. It will create the cipher file and send to user who need decrypt.

otp_enc.c:
This program connects to otp_enc_d, and asks it to perform a one-time pad style encryption as detailed above. By itself, otp_enc doesn’t do the encryption - otp_end_d does.

otp_dec_d.c:
This program performs exactly like otp_enc_d, in syntax and usage. In this case, however, otp_dec_d will decrypt ciphertext it is given, using the passed-in ciphertext and key. Thus, it returns plaintext again to otp_dec.

otp_dec.c:
Similarly, this program will connect to otp_dec_d and will ask it to decrypt ciphertext using a passed-in ciphertext and key, and otherwise performs exactly like otp_enc, and must be runnable in the same three ways. otp_dec should NOT be able to connect to otp_enc_d, even if it tries to connect on the correct port - you'll need to have the programs reject each other, as described in otp_enc.


