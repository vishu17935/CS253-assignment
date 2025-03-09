# **Library Management System Documentation**

## **1. Overview**
This document provides a detailed explanation of the **Library Management System** implemented in C++ using **Object-Oriented Programming (OOP) principles**. The system allows **students and faculty to borrow, return, and reserve books**, while **librarians can manage books and users**.

The system maintains data persistence using **file handling**, ensuring that user records, book inventory, and borrowing history are saved between program runs.

---

## **2. Object-Oriented Design**
The system follows **OOP principles** like **abstraction, encapsulation, inheritance, and polymorphism**. The main classes are:
- **Book** (Represents books in the library)
- **Membership** (Manages borrowing details)
- **Member (Base class for users)**
  - **CollegeStudent** (Student users, limited borrowing privileges)
  - **Professor** (Faculty users, extended privileges, no fines)
  - **LibraryStaff** (Librarians who manage books and users)
- **LibrarySystem** (Manages all library operations)

---

## **3. Class Descriptions**

### **3.1 Book Class**
```cpp
class Book {
private:
    string isbn, name, creator, company;
    string availability, bookedBy;
    int publicationYear;
public:
    Book(string isbn_val, string name_val, int year_val, string creator_val, string company_val);
    string getISBN() const;
    string getAvailability() const;
    void setAvailability(string status);
};
```
**Purpose:**
- Represents books in the library.
- Keeps track of **ISBN, title, author, publisher, year, and status (available/borrowed/reserved).**
- Supports **serialization** for file storage.

---

### **3.2 Membership Class**
```cpp
class Membership {
private:
    string memberId;
    vector<BorrowInfo> checkedOutItems;
    double pendingFees;
public:
    Membership(string id);
    void checkoutItem(const string& isbn);
    void returnItem(const string& isbn, double fee);
    double getPendingFees() const;
    void clearFees(double amount);
};
```
**Purpose:**
- Stores **borrowed books and pending fees** for each user.
- Implements **checkout, return, and fine calculation**.

---

### **3.3 Member Class (Base Class)**
```cpp
class Member {
protected:
    string memberId, fullName, memberType;
    Membership membership;
public:
    Member(string id, string name, string type);
    virtual bool isEligibleToBorrow() const = 0;
    virtual int calculateLateFee(int daysLate) const = 0;
};
```
**Purpose:**
- Defines **common attributes** for all users (ID, name, type, and membership).
- Implements **polymorphic methods** for borrowing and fines.

### **Derived Classes:**
1. **CollegeStudent**: Can borrow up to 3 books for 15 days, fines ₹10/day.
2. **Professor**: Can borrow up to 5 books for 30 days, **no fines**.
3. **LibraryStaff**: Cannot borrow books, but can **add/remove books and users**.

---

### **3.4 LibrarySystem Class**
```cpp
class LibrarySystem {
private:
    vector<Book> catalog;
    vector<Member*> memberDatabase;
public:
    void addBook(const Book& item);
    void checkoutBook(Member* member, const string& isbn);
    void returnBook(Member* member, const string& isbn);
    void reserveBook(Member* member, const string& isbn);
    void displayCatalog();
    void displayMembers();
};
```
**Purpose:**
- Manages **all operations**: borrowing, returning, reservations, book/user management.
- Stores **books and members in vectors**.
- Uses **file handling** to store data persistently.

---

## **4. System Functioning**

### **4.1 Book Borrowing Process**
1. User logs in using **Member ID**.
2. System checks **borrowing limits** and **pending fines**.
3. If eligible, book status is updated to **"borrowed"**.
4. Borrow record is added to user's membership.

### **4.2 Book Returning Process**
1. User selects a book to return.
2. System checks **due date** and calculates **late fees (if applicable).**
3. Book status updates to **"available"** or **"reserved"** if another user reserved it.
4. Late fees (if any) are added to the user's account.

### **4.3 Reservation Process**
1. Users can **reserve** books that are currently borrowed.
2. Once returned, the system **notifies the reserver**.
3. Reservations **expire if not claimed within a set time.**

---

## **5. Data Structures Used**
### **5.1 Vectors for Storage**
- **`vector<Book> catalog;`** → Stores books.
- **`vector<Member*> memberDatabase;`** → Stores users dynamically.

### **5.2 File Handling**
- Books stored in `book.csv`
- Users stored in `members.csv`
- Borrow history in `checkouts.csv`
- Fines stored in `fees.csv`

**File format example (books):**
```
LIT001,Advanced Programming,Jane Doe,TechPress,2022,available,
LIT002,Data Structures,John Smith,CodeBooks,2020,borrowed,STU1
```

---

## **6. Error Handling & Edge Cases**
1. **Invalid Member ID:** System prevents unauthorized access.
2. **Missing Books in Catalog:** Proper checks before borrowing/returning.
3. **File Corruption Handling:** Uses `try-catch` for malformed entries.
4. **Overdue Handling:** Restricts further borrowing until fines are cleared.

---

## **7. Conclusion**
This **Library Management System** follows **OOP best practices** while efficiently handling **library operations**. The system ensures **data persistence**, smooth user experience, and strict role-based access.

This documentation covers all major **functionalities, data structures, and working principles**, making it easier to understand and extend in the future. 🚀

