
# Moara

  

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