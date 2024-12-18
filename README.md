# CSC213_Final_Project

__Gun Woo Kim__ (Quinsh) & __Yash Malik__ (Yashmalikk) 

An RSA based end-to-end encrypted messaging program for local networks, especially Grinnell Colleges' MATLAN. Messages are saved locally using file system. Multithreading is used for encryption/decryption and bi-directional message sending.

## Instructions for use:

__1__ Download this repo in some folder

__2__ Open the terminal

__3__ run make

If you are the server (you make the chat room)
    __4__ run "./secure_communication <some integer between 0000 and 9999(lets call it s)> <some ip like 192.134.78.54> <some integer less than the first(c such that c is less than s)> <the name of the person you are connecting too>

Else, if you are joining the chat:
    __4__ run "./secure_communication <some integer less than the first(c such that c is less than s)> <some ip like 192.134.78.54> <some integer between 0000 and 9999(lets call it s)> <the name of the person you are connecting too>
Enjoy!

NOTE: You must already decide on the two 4 digit numbers for the ports and the ip in order to communicate.

NOTE NOTE: It is best to comminicate on new ports every time, or run make clean and then make in the terminal and repeat.


## References
- [Prof. Curtsinger Networking Exercise](https://curtsinger.cs.grinnell.edu/teaching/2024F/CSC213/exercises/networking/)
