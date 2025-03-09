#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <algorithm>

using namespace std;

class Book {
private:
    string isbn, name, creator, company;
    string availability, bookedBy;
    int publicationYear;

public:
    // Constructor with different parameter order
    Book(string isbn_val, string name_val, int year_val, string creator_val, string company_val) 
        : isbn(isbn_val), name(name_val), publicationYear(year_val), 
          creator(creator_val), company(company_val), availability("available"), bookedBy("") {}

    // Getters with different names
    string getISBN() const { return isbn; }
    string getName() const { return name; }
    string getCreator() const { return creator; }
    string getCompany() const { return company; }
    int getPublicationYear() const { return publicationYear; }
    string getAvailability() const { return availability; }
    string getBookedBy() const { return bookedBy; }
    
    // Setters with different names
    void setAvailability(string status) { availability = status; }
    void setBookedBy(string memberId) { bookedBy = memberId; }

    // Data serialization function
    string serialize() const {
        ostringstream data;
        data << isbn << "," << name << "," << creator << "," 
             << company << "," << publicationYear << "," 
             << availability << "," << bookedBy;
        return data.str();
    }

    // Static factory method to create object from serialized data
    static Book deserialize(const string& data) {
        istringstream stream(data);
        string isbn_val, name_val, creator_val, company_val, year_str, status, reserver;
        
        getline(stream, isbn_val, ',');
        getline(stream, name_val, ',');
        getline(stream, creator_val, ',');
        getline(stream, company_val, ',');
        getline(stream, year_str, ',');
        getline(stream, status, ',');
        getline(stream, reserver);
        
        int year_val = stoi(year_str);
        Book lit(isbn_val, name_val, year_val, creator_val, company_val);
        lit.setAvailability(status);
        lit.setBookedBy(reserver);
        return lit;
    }
};

struct BorrowInfo {
    string isbn;
    chrono::system_clock::time_point checkoutDate;
};

class Membership {
private:
    string memberId;
    vector<BorrowInfo> checkedOutItems;
    double pendingFees;

public:
    // Constructor
    Membership(string id) : memberId(id), pendingFees(0.0) {}

    // Methods with different naming
    void checkoutItem(const string& isbn) {
        checkedOutItems.push_back({isbn, chrono::system_clock::now()});
    }

    void returnItem(const string& isbn, double fee) {
        auto it = find_if(checkedOutItems.begin(), checkedOutItems.end(), 
                         [&isbn](const BorrowInfo& info) { return info.isbn == isbn; });
        if (it != checkedOutItems.end()) {
            checkedOutItems.erase(it);
            pendingFees += fee;
        }
    }

    // Getters and setters
    double getPendingFees() const { return pendingFees; }
    void clearFees(double amount) { pendingFees = max(0.0, pendingFees - amount); }
    void setPendingFees(double fee) { pendingFees = fee; }
    const vector<BorrowInfo>& getCheckedOutItems() const { return checkedOutItems; }

    void addCheckoutRecord(const BorrowInfo& info) { checkedOutItems.push_back(info); }
};

// Member base class and derived classes (renamed from User)
class Member {
protected:
    string memberId, fullName, memberType;
    Membership membership;

public:
    Member(string id, string name, string type) 
        : memberId(id), fullName(name), memberType(type), membership(id) {}
    virtual ~Member() {}
    
    // Getters
    string getMemberId() const { return memberId; }
    string getFullName() const { return fullName; }
    string getMemberType() const { return memberType; }
    Membership& getMembership() { return membership; }
    const Membership& getMembership() const { return membership; }
    
    // Pure virtual methods
    virtual bool isEligibleToBorrow() const = 0;
    virtual int calculateLateFee(int daysLate) const = 0;

    // Serialization method
    string serialize() const { return memberId + "," + fullName + "," + memberType; }
};

// Derived class for students
class CollegeStudent : public Member {
public:
    CollegeStudent(string id, string name) : Member(id, name, "student") {}
    
    bool isEligibleToBorrow() const override {
        return membership.getCheckedOutItems().size() < 3 && membership.getPendingFees() == 0;
    }
    
    int calculateLateFee(int daysLate) const override {
        return daysLate > 0 ? daysLate * 10 : 0;
    }
};

// Derived class for faculty
class professor : public Member {
public:
    professor(string id, string name) : Member(id, name, "faculty") {}
    
    bool isEligibleToBorrow() const override {
        if (membership.getCheckedOutItems().size() >= 5) return false;
        
        auto currentTime = chrono::system_clock::now();
        for (const auto& item : membership.getCheckedOutItems()) {
            int daysSinceCheckout = chrono::duration_cast<chrono::hours>
                                   (currentTime - item.checkoutDate).count() / 24;
            if (daysSinceCheckout > 90) return false; // 30 days + 60 days late
        }
        return true;
    }
    
    int calculateLateFee(int daysLate) const override { return 0; }
};

// Derived class for librarians
class LibraryStaff : public Member {
public:
    LibraryStaff(string id, string name) : Member(id, name, "librarian") {}
    
    bool isEligibleToBorrow() const override { return false; }
    int calculateLateFee(int daysLate) const override { return 0; }
};
// Library class implementation
class LibrarySystem {
    private:
        vector<Book> catalog;
        vector<Member*> memberDatabase;
    
    public:
        // Constructor loads data
        LibrarySystem() { importData(); }
        
        // Destructor saves data and cleans up
        ~LibrarySystem() { 
            exportData(); 
            for (Member* m : memberDatabase) delete m; 
        }
    
        // Catalog management methods
        void addbook(const Book& item) { 
            catalog.push_back(item); 
        }
        
        void removebook(const string& isbn) {
            catalog.erase(remove_if(catalog.begin(), catalog.end(), 
                         [&isbn](const Book& lit) { return lit.getISBN() == isbn; }), 
                         catalog.end());
        }
    
        // Member management methods
        void registerMember(Member* member) { 
            memberDatabase.push_back(member); 
        }
        
        void removeMember(const string& memberId) {
            auto it = find_if(memberDatabase.begin(), memberDatabase.end(),
                             [&memberId](const Member* m) { return m->getMemberId() == memberId; });
            if (it != memberDatabase.end()) {
                delete *it;
                memberDatabase.erase(it);
            }
        }
    
        // Search methods
        Book* findbook(const string& isbn) {
            for (auto& item : catalog) {
                if (item.getISBN() == isbn) return &item;
            }
            return nullptr;
        }
    
        Member* findMember(const string& memberId) {
            for (Member* member : memberDatabase) {
                if (member->getMemberId() == memberId) return member;
            }
            return nullptr;
        }
    
        // Checkout process
        void checkoutbook(Member* member, const string& isbn) {
            Book* item = findbook(isbn);
            if (!item) { 
                cout << "Item not found in catalog.\n"; 
                return; 
            }
            
            if (item->getAvailability() == "borrowed") { 
                cout << "Item is already checked out.\n"; 
                return; 
            }
            
            if (item->getAvailability() == "reserved" && item->getBookedBy() != member->getMemberId()) {
                cout << "Item is reserved by another member.\n"; 
                return;
            }
            
            if (!member->isEligibleToBorrow()) { 
                cout << "You are not eligible to borrow at this time.\n"; 
                return; 
            }
            
            bool wasReservedByMember = (item->getAvailability() == "reserved" && 
                                      item->getBookedBy() == member->getMemberId());
            
            member->getMembership().checkoutItem(isbn);
            item->setAvailability("borrowed");
            item->setBookedBy("");  // Clear reservation
            
            cout << "Item checked out successfully.\n";
            
            if (wasReservedByMember) {
                int remainingReserved = getReservationCount(member->getMemberId());
                cout << "Reservation fulfilled. You have " << remainingReserved 
                     << " remaining reservations.\n";
            }
        }
    
        // Return process
        void returnbook(Member* member, const string& isbn) {
            Book* item = findbook(isbn);
            if (!item) { 
                cout << "Item not found in catalog.\n"; 
                return; 
            }
            
            if (item->getAvailability() != "borrowed") { 
                cout << "Item is not checked out.\n"; 
                return; 
            }
            
            auto& checkedOut = member->getMembership().getCheckedOutItems();
            auto it = find_if(checkedOut.begin(), checkedOut.end(), 
                             [&isbn](const BorrowInfo& info) { return info.isbn == isbn; });
                             
            if (it == checkedOut.end()) { 
                cout << "You have not checked out this item.\n"; 
                return; 
            }
            
            auto currentTime = chrono::system_clock::now();
            int daysSinceCheckout = chrono::duration_cast<chrono::hours>
                                  (currentTime - it->checkoutDate).count() / 24;
                                  
            int loanPeriod = (member->getMemberType() == "student") ? 15 : 30;
            int daysLate = max(0, daysSinceCheckout - loanPeriod);
            int fee = member->calculateLateFee(daysLate);
            
            member->getMembership().returnItem(isbn, fee);
            item->setAvailability(item->getBookedBy().empty() ? "available" : "reserved");
            
            cout << "Item returned successfully.\n";
            if (fee > 0) cout << "Late fee of " << fee << " rupees applied.\n";
        }
    
        // Reservation process
        void reservebook(Member* member, const string& isbn) {
            Book* item = findbook(isbn);
            if (!item) { 
                cout << "Item not found in catalog.\n"; 
                return; 
            }
            
            if (item->getAvailability() != "borrowed") { 
                cout << "Item is not eligible for reservation.\n"; 
                return; 
            }
            
            if (!item->getBookedBy().empty()) { 
                cout << "Item is already reserved by someone else.\n"; 
                return; 
            }
            
            item->setBookedBy(member->getMemberId());
            cout << "Item reserved successfully.\n";
        }
    
        // Count reservations for a member
        int getReservationCount(const string& memberId) const {
            int count = 0;
            for (const auto& item : catalog) {
                if (item.getBookedBy() == memberId) count++;
            }
            return count;
        }
    
        // Search functions
        void searchCatalog(const string& query) {
            if (catalog.empty()) { 
                cout << "Catalog is empty.\n"; 
                return; 
            }
            
            bool itemFound = false;
            for (const auto& item : catalog) {
                if (item.getName().find(query) != string::npos || 
                    item.getCreator().find(query) != string::npos) {
                    cout << item.getISBN() << " - " << item.getName() << " by " 
                         << item.getCreator() << " (" << item.getAvailability() << ")\n";
                    itemFound = true;
                }
            }
            
            if (!itemFound) cout << "No matching items found.\n";
        }
    
        // Display catalog
        void displayCatalog() {
            if (catalog.empty()) { 
                cout << "Catalog is empty.\n"; 
                return; 
            }
            
            for (const auto& item : catalog) {
                cout << item.getISBN() << " - " << item.getName() 
                     << " (" << item.getAvailability() << ")\n";
            }
        }
    
        // Display member list
        void displayMembers() {
            if (memberDatabase.empty()) {
                cout << "No members registered.\n";
                return;
            }
            
            for (const auto& member : memberDatabase) {
                cout << member->getMemberId() << " - " << member->getFullName() 
                     << " (" << member->getMemberType() << ")\n";
            }
        }
    
        // Data persistence methods
        void importData() {
            // Import catalog
            ifstream catalogFile("book.csv");
            if (catalogFile.is_open()) {
                string line;
                while (getline(catalogFile, line)) catalog.push_back(Book::deserialize(line));
                catalogFile.close();
            } else {
                // Default catalog data
                addbook(Book("LIT001", "Advanced Programming", 2022, "Jane Doe", "TechPress"));
                addbook(Book("LIT002", "Data Structures", 2020, "John Smith", "CodeBooks"));
                addbook(Book("LIT003", "Algorithm Design", 2021, "Alice Johnson", "CompSci"));
                addbook(Book("LIT004", "Database Systems", 2019, "Bob Williams", "DataPub"));
                addbook(Book("LIT005", "Machine Learning", 2023, "Carol Brown", "AIPress"));
            }
    
            // Import members
            ifstream memberFile("members.csv");
            if (memberFile.is_open()) {
                string line;
                while (getline(memberFile, line)) {
                    istringstream stream(line);
                    string id, name, type;
                    getline(stream, id, ',');
                    getline(stream, name, ',');
                    getline(stream, type);
                    
                    if (type == "student") registerMember(new CollegeStudent(id, name));
                    else if (type == "faculty") registerMember(new professor(id, name));
                    else if (type == "librarian") registerMember(new LibraryStaff(id, name));
                }
                memberFile.close();
            } else {
                // Default member data
                registerMember(new CollegeStudent("STU1", "Student One"));
                registerMember(new CollegeStudent("STU2", "Student Two"));
                registerMember(new CollegeStudent("STU3", "Student Three"));
                registerMember(new CollegeStudent("STU4", "Student Four"));
                registerMember(new CollegeStudent("STU5", "Student Five"));
                registerMember(new professor("PROF1", "Professor One"));
                registerMember(new professor("PROF2", "Professor Two"));
                registerMember(new professor("PROF3", "Professor Three"));
                registerMember(new LibraryStaff("STAFF1", "Staff One"));
            }
    
            // Import checkout history
            ifstream checkoutFile("checkouts.csv");
            if (checkoutFile.is_open()) {
                string line;
                while (getline(checkoutFile, line)) {
                    istringstream stream(line);
                    string mid, isbn, timeStampStr;
                    getline(stream, mid, ',');
                    getline(stream, isbn, ',');
                    getline(stream, timeStampStr);
                    
                    long long timeStamp = stoll(timeStampStr);
                    auto timePoint = chrono::system_clock::time_point(chrono::seconds(timeStamp));
                    
                    Member* m = findMember(mid);
                    if (m) m->getMembership().addCheckoutRecord({isbn, timePoint});
                }
                checkoutFile.close();
            }
    
            // Import fees
            ifstream feesFile("fees.csv");
            if (feesFile.is_open()) {
                string line;
                while (getline(feesFile, line)) {
                    istringstream stream(line);
                    string mid, feeStr;
                    getline(stream, mid, ',');
                    getline(stream, feeStr);
                    
                    double fee = stod(feeStr);
                    Member* m = findMember(mid);
                    if (m) m->getMembership().setPendingFees(fee);
                }
                feesFile.close();
            }
        }
    
        // Export data to files
        void exportData() {
            // Export catalog
            ofstream catalogFile("book.csv");
            for (const auto& item : catalog) catalogFile << item.serialize() << "\n";
            catalogFile.close();
    
            // Export member data
            ofstream memberFile("members.csv");
            for (const auto& member : memberDatabase) memberFile << member->serialize() << "\n";
            memberFile.close();
    
            // Export checkout data
            ofstream checkoutFile("checkouts.csv");
            for (const auto& member : memberDatabase) {
                for (const auto& info : member->getMembership().getCheckedOutItems()) {
                    long long timeStamp = chrono::duration_cast<chrono::seconds>
                                        (info.checkoutDate.time_since_epoch()).count();
                    checkoutFile << member->getMemberId() << "," << info.isbn << "," << timeStamp << "\n";
                }
            }
            checkoutFile.close();
    
            // Export fees data
            ofstream feesFile("fees.csv");
            for (const auto& member : memberDatabase) {
                double fee = member->getMembership().getPendingFees();
                if (fee > 0) feesFile << member->getMemberId() << "," << fee << "\n";
            }
            feesFile.close();
        }
    };
// Main application function
int main() {
    LibrarySystem system;

    // Main application loop
    while (true) {
        cout << "\n===== LIBRARY MANAGEMENT SYSTEM =====\n";
        cout << "Enter member ID to login (or 'exit' to quit): ";
        string input;
        cin >> input;
        
        if (input == "exit") {
            cout << "Saving data and exiting..." << endl;
            break;
        }

        // Verify member
        Member* activeMember = system.findMember(input);
        if (!activeMember) { 
            cout << "Member not found. Please try again.\n"; 
            continue; 
        }

        // Display login confirmation
        cout << "\nWelcome, " << activeMember->getFullName() 
             << " (" << activeMember->getMemberType() << ")" << endl;

        // Show summary for students and faculty
        if (activeMember->getMemberType() == "student" || activeMember->getMemberType() == "faculty") {
            int itemsCheckedOut = activeMember->getMembership().getCheckedOutItems().size();
            int itemsReserved = system.getReservationCount(activeMember->getMemberId());
            cout << "Items checked out: " << itemsCheckedOut << endl;
            cout << "Items reserved: " << itemsReserved << endl;
        }

        // Display appropriate menu based on member type
        if (activeMember->getMemberType() == "student" || activeMember->getMemberType() == "faculty") {
            // Member menu loop
            while (true) {
                cout << "\n----- MEMBER MENU -----\n";
                cout << "1. Check out an item\n";
                cout << "2. Return an item\n";
                cout << "3. View checked out items\n";
                cout << "4. View pending fees\n";
                cout << "5. Pay fees\n";
                cout << "6. Reserve an item\n";
                cout << "7. Search catalog\n";
                cout << "8. Logout\n";
                cout << "Selection: ";
                
                int choice;
                cin >> choice;
                
                if (choice == 8) break;

                string isbn, query;
                switch (choice) {
                    case 1: // Check out
                        cout << "Enter ISBN: ";
                        cin >> isbn;
                        system.checkoutbook(activeMember, isbn);
                        break;
                        
                    case 2: // Return
                        cout << "Enter ISBN: ";
                        cin >> isbn;
                        system.returnbook(activeMember, isbn);
                        break;
                        
                    case 3: // View checked out items
                        cout << "\nCurrently checked out items:\n";
                        for (const auto& info : activeMember->getMembership().getCheckedOutItems()) {
                            Book* item = system.findbook(info.isbn);
                            if (item) {
                                cout << info.isbn << " - " << item->getName() << "\n";
                            } else {
                                cout << info.isbn << " (Item details not available)\n";
                            }
                        }
                        if (activeMember->getMembership().getCheckedOutItems().empty()) {
                            cout << "No items currently checked out.\n";
                        }
                        break;
                        
                    case 4: // View fees
                        cout << "Pending fees: " << activeMember->getMembership().getPendingFees() << " rupees\n";
                        break;
                        
                    case 5: {// Pay fees
                        double currentFees = activeMember->getMembership().getPendingFees();
                        cout << "Paying total fees: " << currentFees << " rupees\n";
                        activeMember->getMembership().clearFees(currentFees);
                        cout << "Fees cleared successfully.\n";
                        break;
                    }    
                    case 6: // Reserve
                        cout << "Enter ISBN: ";
                        cin >> isbn;
                        system.reservebook(activeMember, isbn);
                        break;
                        
                    case 7: // Search
                        cout << "Enter search term: ";
                        cin.ignore();
                        getline(cin, query);
                        system.searchCatalog(query);
                        break;
                        
                    default:
                        cout << "Invalid selection. Please try again.\n";
                }
            }
        } 
        else if (activeMember->getMemberType() == "librarian") {
            // Library staff menu loop
            while (true) {
                cout << "\n----- STAFF MENU -----\n";
                cout << "1. Add new item\n";
                cout << "2. Remove item\n";
                cout << "3. Register new member\n";
                cout << "4. Remove member\n";
                cout << "5. Display full catalog\n";
                cout << "6. Display all members\n";
                cout << "7. Search catalog\n";
                cout << "8. Logout\n";
                cout << "Selection: ";
                
                int choice;
                cin >> choice;
                
                if (choice == 8) break;

                string isbn, id, name, type, title, author, publisher, query;
                int year;
                
                switch (choice) {
                    case 1: // Add item
                        cout << "ISBN: ";
                        cin >> isbn;
                        cin.ignore();
                        cout << "Title: ";
                        getline(cin, title);
                        cout << "Author: ";
                        getline(cin, author);
                        cout << "Publisher: ";
                        getline(cin, publisher);
                        cout << "Publication Year: ";
                        cin >> year;
                        
                        system.addbook(Book(isbn, title, year, author, publisher));
                        cout << "Item added to catalog successfully.\n";
                        break;
                        
                    case 2: // Remove item
                        cout << "Enter ISBN to remove: ";
                        cin >> isbn;
                        system.removebook(isbn);
                        cout << "Item removed from catalog.\n";
                        break;
                        
                    case 3: // Add member
                        cout << "Member ID: ";
                        cin >> id;
                        cin.ignore();
                        cout << "Full Name: ";
                        getline(cin, name);
                        cout << "Member Type (student/faculty/librarian): ";
                        cin >> type;
                        
                        if (type == "student") system.registerMember(new CollegeStudent(id, name));
                        else if (type == "faculty") system.registerMember(new professor(id, name));
                        else if (type == "librarian") system.registerMember(new LibraryStaff(id, name));
                        else cout << "Invalid member type.\n";
                        
                        cout << "Member registered successfully.\n";
                        break;
                        
                    case 4: // Remove member
                        cout << "Enter Member ID to remove: ";
                        cin >> id;
                        system.removeMember(id);
                        cout << "Member removed successfully.\n";
                        break;
                        
                    case 5: // Display catalog
                        cout << "\n----- FULL CATALOG -----\n";
                        system.displayCatalog();
                        break;
                        
                    case 6: // Display members
                        cout << "\n----- MEMBER DIRECTORY -----\n";
                        system.displayMembers();
                        break;
                        
                    case 7: // Search
                        cout << "Enter search term: ";
                        cin.ignore();
                        getline(cin, query);
                        system.searchCatalog(query);
                        break;
                        
                    default:
                        cout << "Invalid selection. Please try again.\n";
                }
            }
        }
    }
    
    return 0;
}
