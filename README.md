
# Moara

![image](https://github.com/user-attachments/assets/da0b87ff-d6a7-4556-9c41-96ae748a8aed)



## Functionalities
1. **User Registration and Authentication**
   - Allows users to create accounts and log in
   - The accounts are kept in a local database that is accessed by the server
2. **Real-Time Lobby System**
   - Users can create, join, or leave lobbies in real-time using WebSocket communication.
   - Each lobby maintains its own unique session and tracks the participants.
3. **Multiple board types**
   - Players can choose between multiple types of boards
   - The architecture supports adding custom boards (independent of size and form)
4. **Game types**
   - PvP
   - Against AI
5. **PvP**
   - A match can be played with up to 3 players at the moment
   - If needed, the architecture allows that number to increase
6. **Bots**
   - The bots have 3 main difficulties: Easy, Medium and Hard
   - By extending an available interface, a custom difficulty can be added to adjust different aspects of the bot's behavior
7. **Other features**
   - You can undo any move if you need
   - The board types include the option to add diagonals, alongside increasing the board's size
  
---
## Development Process

1. **Most of the Logic implementation**
  
2. **Creation of boards**

3. **Configuration, timer, and bots**

4. **Base SDK implementation**

5. **Finished the rest of the logic**

6. **Added SFML server & Client UI**

7. **Switch to QtWebSockets**
   - Rebuilt the server and SDK using QtWebSockets to replace the SFML-based implementation.
  
8. **Database Integration**
   - Added login and sign-up functionality in the user interface.
   - Established database connectivity on the server side.

9. **Bug Fixes and Testing**
---
## Technologies Used


### User Interface: *Qt*

  

For the Client-side, **Qt** was used, specifically with **QtWebSockets** for the server-side communication.

You can download the latest version of Qt from the official Qt website: [Qt Downloads](https://account.qt.io/s/downloads)



---

### Server: *QtWebSockets*


 QtWebSockets is used on the server-side for communication over WebSockets, enabling real-time, two-way interaction between the client and server.
 
 The package can be installed using the **Qt Maintenance Tool**, which is included with the Qt installation.

---
  

### Testing: *GTest / GMock & Artillery*

  

For testing, two primary tools were used:

  

1.  **GTest / GMock**

These are used for unit testing and mocking in the project. They can be easily installed via **NuGet**.

  

2.  **Artillery**

To install Artillery globally, run the following command in your terminal or command prompt:

```bash

npm install -g artillery

```

  

-  **Running Tests:**

To execute the existing `.yml` test configuration file, use the following command:

```bash

artillery run websocket-test.yml

```

  

For more details on Artillery, visit: [Artillery on npm](https://www.npmjs.com/package/artillery)

  

---

  

### Database: *PostgreSQL & libpqxx*

  

The database used in this project is **PostgreSQL**. It is a relational database system that stores essential information, such as usernames and passwords.

PostgreSQL can be downloaded from the official website: [PostgreSQL Downloads](https://www.postgresql.org/download/)

  

#### Connecting to Database with **libpqxx**

  

To connect to PostgreSQL, **libpqxx** is used, which is a C++ library for interacting with PostgreSQL databases.

You can install **libpqxx** using **vcpkg**, a tool to manage libraries in C++ projects.

  

**Steps to install vcpkg:**

```bash

git clone https://github.com/microsoft/vcpkg.git

cd vcpkg

.\bootstrap-vcpkg.bat

.\vcpkg.exe integrate install
```
---

### JSON Parsing: **nlohmann/json**

For JSON parsing, the project uses **nlohmann/json**.

You can find the library and its documentation on GitHub:  [nlohmann/json on GitHub](https://github.com/nlohmann/json)
