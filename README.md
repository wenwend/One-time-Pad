# One-time-Pad
Definition:
Primary reference on One-Time Pads (OTP)

Plaintext is the term for the information that you wish to encrypt and protect. It is human readable.

Ciphertext is the term for the plaintext after it has been encrypted by your programs. Ciphertext is not human-readable, and in fact cannot be cracked, if the OTP system is used correctly.

A Key is the random sequence of characters that will be used to convert Plaintext to Ciphertext, and back again. It must not be re-used, or else the encryption is in danger of being broken.

This program will encrypt and decrypt plaintext into ciphertext, using a key. All 27 characters will be encrypted and decrypted as above.

otp_enc_d.c:
This program will run in the background as a daemon. This child process of otp_enc_d must first check to make sure it is communicating with otp_enc (see otp_enc, below). After verifying that the connection to otp_enc_d is coming from otp_enc, then this child receives from otp_enc plaintext and a key via the communication socket (not the original listen socket). The otp_enc_d child will then write back the ciphertext to the otp_enc process that it is connected to via the same communication socket. Note that the key passed in must be at least as big as the plaintext. In all error situations, this program must output errors to stderr as appropriate (see grading script below for details), but should not crash or otherwise exit, unless the errors happen when the program is starting up (i.e. are part of the networking start up protocols like bind()). If given bad input, once running, otp_enc_d should recognize the bad input, report an error to stderr, and continue to run. Generally speaking, though, this daemon shouldn't receive bad input, since that should be discovered and handled in the client first. All error text must be output to stderr.

otp_enc.c:
This program connects to otp_enc_d, and asks it to perform a one-time pad style encryption as detailed above. By itself, otp_enc doesn’t do the encryption - otp_end_d does.

otp_dec_d.c:
This program performs exactly like otp_enc_d, in syntax and usage. In this case, however, otp_dec_d will decrypt ciphertext it is given, using the passed-in ciphertext and key. Thus, it returns plaintext again to otp_dec.

otp_dec.c:
Similarly, this program will connect to otp_dec_d and will ask it to decrypt ciphertext using a passed-in ciphertext and key, and otherwise performs exactly like otp_enc, and must be runnable in the same three ways. otp_dec should NOT be able to connect to otp_enc_d, even if it tries to connect on the correct port - you'll need to have the programs reject each other, as described in otp_enc.

keygen.c:
This program creates a key file of specified length. The characters in the file generated will be any of the 27 allowed characters, generated using the standard UNIX randomization methods. Do not create spaces every five characters, as has been historically done. Note that you specifically do not have to do any fancy random number generation: we’re not looking for cryptographically secure random number generation! rand() is just fine. The last character keygen outputs should be a newline. All error text must be output to stderr, if any.
