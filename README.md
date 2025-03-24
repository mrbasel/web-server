# Http Server
**NOTE: This project is currently for educational purposes and should not be used in production**

This is a small lightweight HTTP 1.1 server implementation written using the C programming language.

## Usage

After cloning the project, run `make`, then run the program (first arg is port and second is workers):
```
.\program 3000 4
```

## Motivation

The main goal of this project was to help me learn C. I ended up learning a lot about how web servers work, socket programming and managing memory in large applications. 

## Architecture

The web server consists of a main thread and a pool of worker threads.

The main thread's job is to accept connections and dispatch requests to the workers queue.

The worker pool threads monitor the workers queue for requests and then process them and sent the response's to the clients directly.

Connections are kept open by default for at least 5 seconds, after that the main thread will close them.

Each worker thread has it's own memory arena for any memory allocations it might need while processing requests.
