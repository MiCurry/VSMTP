How to use this set of software:

Start with setting up your servers:
	1.	Start the encoding daemon with some port number:
			./otp_enc_d 55555 &
			
	2.	Start the decoding daemon with some port number:
			./otp_dec_d 66666 &
			
	4.	Run the keygen software and create a keyfile
		(500 chars for a 500 char sized text file example)
		Redirect it to something like "mykey":
			./keygen 500 > mykey
	
	5.	Use the text file "text" or create your own text file
		with up to the number of chars created in the keyfile
		and run it through the encoding daemon, with the key file
		and port number:
			./otp_enc text mykey 55555 > eText
			
		This will encode the file and spit it back out as an
		encoded text where the only way to get it back to norma
		is through using the key and the decryption software.
		
	6.	To decrypt it: run the decryption software with the eText
		file, the keyfile and the port number as parameters:
			./otp_dec eText mykey 66666 > dText
			
		This will decrypt the software to a file named dText,
		resulting in a file that resembles the original normal
		text file.