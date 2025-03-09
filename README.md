# 📚 Library Management System

A **Library Management System** implemented in **C++** using **Object-Oriented Programming (OOP)** principles. This system allows students and faculty to **borrow, return, and reserve books**, while librarians can **manage books and users**. The system ensures data persistence using file handling.

---

## ✨ Features
- 📖 **Borrow and return books** with automatic **fine calculation**
- 🔍 **Search for books** by title or author
- 🏫 **Different borrowing privileges** for students and faculty
- 🚫 **Late return fines** for students (₹10/day overdue)
- 📚 **Librarians can add/remove books and users**
- 📝 **Data is stored in files** (`books.csv`, `users.csv`) for persistence

---

## 🛠 Installation & Setup
### **1. Clone the Repository**
```sh
git clone https://github.com/yourusername/LibraryManagementSystem.git
cd LibraryManagementSystem
```

### **2. Compile the Program**
```sh
clang++ -std=gnu++14 -g main.cpp -o main
```

### **3. Run the Program**
```sh
./main
```

---

## 🚀 Usage
1. **Login** using a valid **Member ID** (from `users.csv`).
2. Choose an action:
   - `1` → **Borrow a book**
   - `2` → **Return a book**
   - `3` → **View checked-out books**
   - `4` → **View pending fees**
   - `5` → **Pay fines**
   - `6` → **Reserve a book**
   - `7` → **Search for books**
3. **Librarians** can add/remove books and users.
4. **Books & users are stored in files** (`books.csv`, `users.csv`).

---

## 📂 File Structure
```
📦 LibraryManagementSystem
 ┣ 📜 main.cpp              # Main program
 ┣ 📜 books.csv             # Book inventory
 ┣ 📜 users.csv             # User details
 ┣ 📜 DOCUMENTATION.md      # Technical documentation
 ┣ 📜 README.md             # Project overview
 ┗ 📜 checkouts.csv         # Borrowing history
```

---

## 👨‍💻 Contributing
Want to improve this project? Feel free to fork and submit a **pull request**!

---

## 📜 License
This project is **open-source**. You can modify and use it freely.

