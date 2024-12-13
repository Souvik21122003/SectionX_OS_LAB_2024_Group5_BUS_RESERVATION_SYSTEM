# os_project_bus reservation system



This project demonstrates a client-server application for a bus booking system. It utilizes socket programming for communication between the client and server. The system supports two user roles: User and Admin, each with distinct functionalities.

## Client-Side Highlights

### Socket Setup:
- The client creates a socket using `socket()`.
- It connects to the server at `127.0.0.1` on port `8080`.

### User Roles:
- **User:** Can log in, search buses, and book tickets.
- **Admin:** Can log in and add new bus details.

### Functionalities:
1. **User Login:**
   - Sends a login request to the server with the username and password.
   - Upon successful login, the user can:
     - Search for buses based on source and destination.
     - Book a seat and proceed with payment.

2. **User Sign-Up:**
   - Allows new users to register by sending their credentials to the server.

3. **Admin Features:**
   - Admins can log in to:
     - Add bus details to the system.

### Dynamic Price Calculation:
- Seat prices are calculated dynamically based on:
  - Seat column.
  - Users age.
  - Number of booked seats.

## Server-Side Highlights

### Socket Setup:
- The server listens for incoming connections on port `8080`.
- Communicates with clients using sockets.

### User Management:
- User credentials are stored in `users.txt`.
- The server validates login credentials or registers new users.

### Bus Management:
- Bus details are stored in `bus.txt`.
- Admins can add new buses.
- Users can search for buses by specifying the source and destination.

### Dynamic Seat Pricing:
- Prices are adjusted dynamically based on:
  - Seat location.
  - Users age.
  - Available seat count.

### Data Communication:
- The server sends:
  - Bus details.
  - Booking confirmations.
  - Payment prompts.

## Suggestions for Improvements

1. **Error Handling:**
   - Add checks for all `send()` and `recv()` calls to handle disconnections or failed transmissions.
   - Validate user inputs on the client side to prevent invalid options.

2. **Concurrent Connections:**
   - Use `pthread` or `fork` to handle multiple client connections simultaneously on the server.

3. **Data Storage:**
   - Replace text files with a database (e.g., SQLite) for better scalability and data integrity.

4. **Security:**
   - Store passwords securely using hashing algorithms.
   - Add encryption (e.g., SSL/TLS) for secure communication.

5. **Code Modularity:**
   - Refactor the client and server logic into reusable functions for better readability and maintainability.
