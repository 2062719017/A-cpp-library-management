#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>

using namespace std;
class BookData {
private:
    string bookName;  
    string bookID;    
    int bookCount;    
public:
    BookData() : bookName(""), bookID(""), bookCount(0) {}
    BookData(string name, string id, int count)
        : bookName(move(name)), bookID(move(id)), bookCount(count) {
    }

    // Getter �� Setter ����
    string getBookName() const { return bookName; }
    string getBookID() const { return bookID; }
    int getBookCount() const { return bookCount; }
    void setBookCount(int count) { this->bookCount = count; }
};

// �û�������
class UserData {
private:
    long long phoneNumber;  
    long long studentID;    
    string userName;        
    string password;        
    struct BorrowRecord {
        string bookID;
        int count;
    };

    vector<BorrowRecord> borrowRecords; // �洢�û��Ľ�����Ϣ
public:
    UserData(long long phone, long long id, string name, string pwd)
        : phoneNumber(phone), studentID(id), userName(move(name)), password(move(pwd)) {
    }
    long long getPhoneNumber() const { return phoneNumber; }
    long long getStudentID() const { return studentID; }
    string getUserName() const { return userName; }
    string getPassword() const { return password; }
    void setPassword(string newPwd) { password = newPwd; }

    vector<BorrowRecord> getBorrowRecords() const { return borrowRecords; }
    void addBorrowRecord(string bookID, int count) {
        for (auto& record : borrowRecords) {
            if (record.bookID == bookID) {
                record.count += count;
                return;
            }
        }
        borrowRecords.push_back({ bookID, count });
    }

    // ɾ�����ļ�¼
    void removeBorrowRecord(string bookID, int count) {
        for (auto it = borrowRecords.begin(); it != borrowRecords.end(); ++it) {
            if (it->bookID == bookID) {
                if (it->count == count) {
                    borrowRecords.erase(it);
                }
                else {
                    it->count -= count;
                }
                break;
            }
        }
    }

    // ��ȡָ��ͼ��Ľ�������
    int getBorrowedCount(string bookID) const {
        for (auto record : borrowRecords) {
            if (record.bookID == bookID) {
                return record.count;
            }
        }
        return 0;
    }
};

// �ļ�������
class FileOperator {
public:
    // ���ļ��ж�ȡͼ����Ϣ
    static vector<BookData> parseBookFile(fstream& file) {
        string line;
        vector<BookData> bookDataList;
        while (getline(file, line)) {
            size_t firstDelimiter = line.find('#');
            size_t secondDelimiter = line.find('#', firstDelimiter + 1);
            BookData book(
                line.substr(0, firstDelimiter),
                line.substr(firstDelimiter + 1, secondDelimiter - firstDelimiter - 1),
                stoi(line.substr(secondDelimiter + 1))
            );
            bookDataList.push_back(book);
        }
        return bookDataList;
    }

    // ��ͼ����Ϣд���ļ�
    static void writeBookFile(const vector<BookData>& bookList) {
        fstream file("book.txt", ios::out);
        for (const auto& book : bookList) {
            file << book.getBookName() << "#" << book.getBookID() << "#" << book.getBookCount() << endl;
        }
        file.close();
    }

    // ���ļ��ж�ȡ�û���Ϣ
    static vector<UserData> parseUserFile(istream& file) {
        vector<UserData> userDataList;
        string line;
        while (getline(file, line)) {
            size_t pos1 = line.find('#');
            size_t pos2 = line.find('#', pos1 + 1);
            size_t pos3 = line.find('#', pos2 + 1);
            size_t pos4 = line.find('#', pos3 + 1);

            string username = line.substr(0, pos1);
            string pwd = line.substr(pos1 + 1, pos2 - pos1 - 1);
            long long phone = stoll(line.substr(pos2 + 1, pos3 - pos2 - 1));
            long long id = stoll(line.substr(pos3 + 1, pos4 - pos3 - 1));

            UserData user(phone, id, username, pwd);


            size_t start = pos4 + 1;
            while (start < line.length()) {
                size_t nextPos = line.find('#', start);
                if (nextPos == string::npos) break;
                string bookID = line.substr(start, nextPos - start);
                start = nextPos + 1;
                nextPos = line.find('#', start);
                if (nextPos == string::npos) break;
                int count = stoi(line.substr(start, nextPos - start));
                user.addBorrowRecord(bookID, count);
                start = nextPos + 1;
            }
            userDataList.push_back(user);
        }
        return userDataList;
    }

    // ���û���Ϣд���ļ�
    static void writeUserFile(const vector<UserData>& userList) {
        ofstream file("userData.txt");
        for (const auto& user : userList) {
            file << user.getUserName() << "#" << user.getPassword() << "#"
                << user.getPhoneNumber() << "#" << user.getStudentID() << "#";
            for (const auto& record : user.getBorrowRecords()) {
                file << record.bookID << "#" << record.count << "#";
            }
            file << endl;
        }
        file.close();
    }
};

// ��ӡͼ����Ϣ
void printBook(BookData& book) {
    cout << setw(40) << left << book.getBookName()
        << setw(20) << left << book.getBookID()
        << book.getBookCount() << endl;
}

// ��ʼ��ͼ���ļ�
void initializeBookFile() {
    ifstream file("book.txt");
    if (file.peek() == ifstream::traits_type::eof()) {
        ofstream outputFile("book.txt", ios::app);
        outputFile << "��ʼ�ļ�д��ͼ���ʼ��Ϣ" << "#" << "0" << "#" << "0" << endl;
    }
    file.close();
}

// ����Ա��¼
bool adminLogin() {
    cout << "���������Ա����" << endl;
    string password;
    cin >> password;
    return password == "admin4211";
}

// �û���¼
string userLogin() {
    const int MAX_ATTEMPTS = 3;
    int attempts = 0;

    while (attempts < MAX_ATTEMPTS) {
        cout << "�������˺�" << endl;
        string username;
        cin >> username;

        if (username.empty()) {
            cout << "�˺Ų���Ϊ�գ����������롣" << endl;
            continue;
        }

        cout << "����������" << endl;
        string password;
        cin >> password;

        if (password.empty()) {
            cout << "���벻��Ϊ�գ����������롣" << endl;
            continue;
        }

        ifstream file("userData.txt");
        string line;
        while (getline(file, line)) {
            size_t pos1 = line.find('#');
            size_t pos2 = line.find('#', pos1 + 1);

            string storedUser = line.substr(0, pos1);
            string storedPassword = line.substr(pos1 + 1, pos2 - pos1 - 1);

            if (storedUser == username && storedPassword == password) {
                file.close();
                return storedUser;
            }
        }
        file.close();

        attempts++;
        cout << "�˺Ż������������ʣ " << MAX_ATTEMPTS - attempts << " �γ��Ի��ᡣ" << endl;
    }

    cout << "��¼ʧ�ܣ����ĳ��Դ����Ѵﵽ���ޣ����Ժ����ԡ�" << endl;
    return "";
}

// �û�ע��
void userSignUp() {
    cout << "�������û���: ";
    string username;
    cin >> username;

    // ����û����Ƿ��Ѵ���
    ifstream file("userData.txt");
    string line;
    while (getline(file, line)) {
        size_t pos1 = line.find('#');
        string storedUser = line.substr(0, pos1);
        if (storedUser == username) {
            cout << "���û����Ѵ��ڣ����������롣" << endl;
            file.close();
            return;
        }
    }
    file.close();

    cout << "����������: ";
    string password;
    cin >> password;

    cout << "�������ֻ�����: ";
    long long phone;
    while (!(cin >> phone) || phone <= 0) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "������ֻ�������Ч����������Ч���ֻ����롣" << endl;
    }

    cout << "������ѧ��: ";
    long long studentID;
    while (!(cin >> studentID) || studentID <= 0) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "�����ѧ����Ч����������Ч��ѧ�š�" << endl;
    }

    ofstream newFile("userData.txt", ios::app);
    if (newFile.is_open()) {
        newFile << username << "#" << password << "#" << phone << "#" << studentID << "#" << endl;
        cout << "ע��ɹ���" << endl;
        newFile.close();
    }
    else {
        cout << "�޷����ļ���ע��ʧ�ܡ�" << endl;
    }
}

// ���ͼ����Ϣ
void addBook() {
    char choice;
    do {
        cout << "���ڿ�ʼ���ͼ����Ϣ" << endl;
        cout << "����������:" << endl;
        string name;
        cin >> name;
        cout << "������ͼ�� ID:" << endl;
        string id;
        cin >> id;
        int count;
        cout << "����������:" << endl;
        while (!(cin >> count) || count <= 0) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "�����������Ч����������Ч��������" << endl;
        }

        fstream bookFile("book.txt");
        vector<BookData> bookList = FileOperator::parseBookFile(bookFile);
        bookFile.close();

        bool bookExists = false;
        for (BookData& book : bookList) {
            if (book.getBookID() == id) {
                // ͼ���Ѵ��ڣ���������
                book.setBookCount(book.getBookCount() + count);
                bookExists = true;
                break;
            }
        }

        if (!bookExists) {
            // ͼ�鲻���ڣ�����¼�¼
            BookData newBook(name, id, count);
            bookList.push_back(newBook);
        }

        // �����º��ͼ����Ϣд���ļ�
        FileOperator::writeBookFile(bookList);

        cout << "Ҫ�������ͼ�飬������ Y/y;ֹͣ��ӣ������� N/n" << endl;
        cin >> choice;
    } while (choice == 'Y' || choice == 'y');
}

// �û����鹦��
bool borrowBook(string username) {
    fstream bookFile("book.txt");
    vector<BookData> bookList = FileOperator::parseBookFile(bookFile);
    bookFile.close();

    // ��ӡ�ɽ���ͼ���б�
    cout << "�ɽ���ͼ���б�����:" << endl;
    cout << setw(40) << left << "����"
        << setw(20) << left << "ͼ����"
        << "����" << endl;
    cout << string(70, '-') << endl;
    for (auto& book : bookList) {
        if (book.getBookCount() > 0) {
            printBook(book);
        }
    }

    cout << "��ѡ���ѯ��ʽ����ͨ��������ͼ���Ų�ѯ" << endl;
    cout << "1.����" << endl;
    cout << "2.���" << endl;
    int queryOption;
    while (!(cin >> queryOption) || (queryOption != 1 && queryOption != 2)) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "������Ч�������� 1 �� 2��" << endl;
    }

    bool found = false;
    vector<BookData> updatedBookList(bookList);

    ifstream userFile("userData.txt");
    vector<UserData> userList = FileOperator::parseUserFile(userFile);
    userFile.close();

    UserData* currentUser = nullptr;
    for (auto& user : userList) {
        if (user.getUserName() == username) {
            currentUser = &user;
            break;
        }
    }

    switch (queryOption) {
    case 1: {
        cout << "������Ҫ���ĵ�ͼ������" << endl;
        string bookName;
        cin >> bookName;
        for (BookData& book : updatedBookList) {
            if (book.getBookName() == bookName) {
                cout << "�ҵ��ؼ���¼: ";
                printBook(book);
                found = true;
                cout << "������Ҫ���ĵ�ͼ������" << endl;
                int count;
                while (!(cin >> count) || count <= 0 || count > book.getBookCount()) {
                    cin.clear();
                    cin.ignore(10000, '\n');
                    cout << "�����������Ч����������Ч�Ľ���������" << endl;
                }
                book.setBookCount(book.getBookCount() - count);
                if (currentUser) {
                    currentUser->addBorrowRecord(book.getBookID(), count);
                }
                cout << "���ĳɹ���" << endl;
            }
        }
        FileOperator::writeBookFile(updatedBookList);
        FileOperator::writeUserFile(userList);
        if (!found)
            cout << "δ�ҵ���¼" << endl;
        break;
    }
    case 2: {
        cout << "������Ҫ���ĵ�ͼ����" << endl;
        string bookID;
        cin >> bookID;
        for (BookData& book : updatedBookList) {
            if (book.getBookID() == bookID) {
                cout << "�ҵ��ؼ���¼: ";
                printBook(book);
                found = true;
                cout << "������Ҫ���ĵ�ͼ������" << endl;
                int count;
                while (!(cin >> count) || count <= 0 || count > book.getBookCount()) {
                    cin.clear();
                    cin.ignore(10000, '\n');
                    cout << "�����������Ч����������Ч�Ľ���������" << endl;
                }
                book.setBookCount(book.getBookCount() - count);
                if (currentUser) {
                    currentUser->addBorrowRecord(book.getBookID(), count);
                }
                cout << "���ĳɹ���" << endl;
            }
        }
        FileOperator::writeBookFile(updatedBookList);
        FileOperator::writeUserFile(userList);
        if (!found)
            cout << "δ�ҵ���¼" << endl;
        break;
    }
    default:
        break;
    }
    cout << endl << endl;
    return found;
}

// ��ӡ����ͼ����Ϣ
void printAllBooks() {
    fstream file("book.txt", ios::in);
    if (!file.is_open()) {
        cout << "�޷���ͼ���ļ���" << endl;
        return;
    }
    vector<BookData> bookList = FileOperator::parseBookFile(file);
    file.close();

    cout << setw(40) << left << "����"
        << setw(20) << left << "ͼ����"
        << "����" << endl;
    cout << string(70, '-') << endl;

    int index = 1;
    for (const auto& book : bookList) {
        cout << index << ". ";
        printBook(const_cast<BookData&>(book));
        index++;
    }
}

// �û����鹦��
void returnBook(string username) {
    char choice = 'y';
    fstream bookFile("book.txt");
    vector<BookData> bookList = FileOperator::parseBookFile(bookFile);
    bookFile.close();

    ifstream userFile("userData.txt");
    vector<UserData> userList = FileOperator::parseUserFile(userFile);
    userFile.close();

    UserData* currentUser = nullptr;
    for (auto& user : userList) {
        if (user.getUserName() == username) {
            currentUser = &user;
            break;
        }
    }

    if (currentUser) {
        auto borrowRecords = currentUser->getBorrowRecords();
        if (borrowRecords.empty()) {
            cout << "��û�н��ļ�¼���޷����飡" << endl;
            return;
        }
        cout << "���Ľ��ļ�¼���£�" << endl;
        cout << setw(20) << left << "ͼ����"
            << setw(20) << left << "��������" << endl;
        cout << string(40, '-') << endl;
        for (const auto& record : borrowRecords) {
            cout << setw(20) << left << record.bookID
                << setw(20) << left << record.count << endl;
        }
    }

    do {
        cout << "���ڿ�ʼ������Ϣ" << endl;
        cout << "������ͼ�� ID:" << endl;
        string id;
        cin >> id;

        if (id.empty()) {
            cout << "ͼ�� ID ����Ϊ�գ����������롣" << endl;
            continue;
        }

        cout << "�����뻹������" << endl;
        int count;
        while (!(cin >> count) || count <= 0) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "�����������Ч����������Ч�Ļ���������" << endl;
        }

        if (currentUser) {
            int borrowedCount = currentUser->getBorrowedCount(id);
            if (count > borrowedCount) {
                cout << "��������������������������ʧ�ܣ�" << endl;
                continue;
            }
        }

        bool bookExists = false;
        for (BookData& book : bookList) {
            if (book.getBookID() == id) {
                // ͼ���Ѵ��ڣ���������
                book.setBookCount(book.getBookCount() + count);
                bookExists = true;
                cout << "����ɹ�����ǰͼ�� " << book.getBookName() << " ����Ϊ: " << book.getBookCount() << endl;
                if (currentUser) {
                    currentUser->removeBorrowRecord(id, count);
                }
                break;
            }
        }

        if (!bookExists) {
            cout << "δ�ҵ���ͼ���ţ�����ʧ�ܣ�" << endl;
        }

        // �����º��ͼ����Ϣд���ļ�
        FileOperator::writeBookFile(bookList);
        FileOperator::writeUserFile(userList);

        cout << "Ҫ�������飬������ Y/y;ֹͣ���飬������ N/n" << endl;
        cin >> choice;
    } while (choice == 'Y' || choice == 'y');
}

// ��ӡ�����û���Ϣ
void printAllUsers() {
    ifstream file("userData.txt");
    if (!file.is_open()) {
        cout << "�޷����û��ļ���" << endl;
        return;
    }
    vector<UserData> userList = FileOperator::parseUserFile(file);
    file.close();

    // ��ӡ��ͷ
    cout << setw(20) << left << "�û���"
        << setw(20) << left << "����"
        << setw(20) << left << "�ֻ�����"
        << setw(20) << left << "ѧ��"
        << "���ļ�¼" << endl;
    cout << string(100, '-') << endl;

    for (const auto& user : userList) {
        cout << setw(20) << left << user.getUserName()
            << setw(20) << left << user.getPassword()
            << setw(20) << left << user.getPhoneNumber()
            << setw(20) << left << user.getStudentID();

        auto borrowRecords = user.getBorrowRecords();
        if (borrowRecords.empty()) {
            cout << "�޽��ļ�¼" << endl;
        }
        else {
            cout << endl;
            for (const auto& record : borrowRecords) {
                cout << setw(20) << ""
                    << setw(20) << ""
                    << setw(20) << ""
                    << setw(20) << ""
                    << "ͼ����: " << record.bookID << ", ����: " << record.count << endl;
            }
        }
    }
}

// ɾ���û���Ϣ
void deleteUser() {
    cout << "������Ҫɾ�����û���" << endl;
    string username;
    cin >> username;

    ifstream file("userData.txt");
    if (!file.is_open()) {
        cout << "�޷����û��ļ���" << endl;
        return;
    }
    vector<UserData> userList = FileOperator::parseUserFile(file);
    file.close();

    for (auto it = userList.begin(); it != userList.end(); ++it) {
        if (it->getUserName() == username) {
            userList.erase(it);
            FileOperator::writeUserFile(userList);
            cout << "�û�ɾ���ɹ���" << endl;
            return;
        }
    }
    cout << "δ�ҵ����û���" << endl;
}

// ����Ա�޸��û�����
void adminModifyUserPassword() {
    cout << "������Ҫ�޸�������û���: ";
    string username;
    cin >> username;

    ifstream file("userData.txt");
    if (!file.is_open()) {
        cout << "�޷����û��ļ���" << endl;
        return;
    }
    vector<UserData> userList = FileOperator::parseUserFile(file);
    file.close();

    for (auto& user : userList) {
        if (user.getUserName() == username) {
            cout << "������������: ";
            string newPassword;
            cin >> newPassword;
            user.setPassword(newPassword);
            FileOperator::writeUserFile(userList);
            cout << "�����޸ĳɹ���" << endl;
            return;
        }
    }
    cout << "δ�ҵ����û���" << endl;
}

// �û��޸��Լ�������
void userModifyPassword(string username) {
    ifstream file("userData.txt");
    if (!file.is_open()) {
        cout << "�޷����û��ļ���" << endl;
        return;
    }
    vector<UserData> userList = FileOperator::parseUserFile(file);
    file.close();

    for (auto& user : userList) {
        if (user.getUserName() == username) {
            cout << "�����뵱ǰ����: ";
            string currentPassword;
            cin >> currentPassword;
            if (currentPassword != user.getPassword()) {
                cout << "��ǰ���벻��ȷ���޸�ʧ�ܣ�" << endl;
                return;
            }
            cout << "������������: ";
            string newPassword;
            cin >> newPassword;
            user.setPassword(newPassword);
            FileOperator::writeUserFile(userList);
            cout << "�����޸ĳɹ���" << endl;
            return;
        }
    }
    cout << "δ�ҵ����û���" << endl;
}

// ����Ա�˵�
void adminMenu() {
    bool exit = false;
    while (true) {
        cout << "��ѡ��Ҫִ�еĲ���" << endl;
        cout << "1.���ͼ��" << endl;
        cout << "2.��ӡ����ͼ����Ϣ" << endl;
        cout << "3.��ӡ�����û���Ϣ" << endl;
        cout << "4.ɾ���û�" << endl;
        cout << "5.�޸��û�����" << endl;
        cout << "6.�˳���¼" << endl;
        int option;
        while (!(cin >> option) || option < 1 || option > 6) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "������Ч�������� 1 �� 6 ֮������֡�" << endl;
        }
        switch (option) {
        case 1: {
            addBook();
            break;
        }
        case 2: {
            printAllBooks();
            break;
        }
        case 3: {
            printAllUsers();
            break;
        }
        case 4: {
            deleteUser();
            break;
        }
        case 5: {
            adminModifyUserPassword();
            break;
        }
        case 6: {
            exit = true;
            break;
        }
        }
        if (exit)
            return;
    }
}

// �û��˵�
void userMenu(string username) {
    bool exit = false;
    while (true) {
        cout << "��ѡ��Ҫִ�еĲ���" << endl;
        cout << "1.����" << endl;
        cout << "2.�黹" << endl;
        cout << "3.��ѯ���ļ�¼" << endl;
        cout << "4.�޸�����" << endl;
        cout << "5.�˳���¼" << endl;
        int option;
        while (!(cin >> option) || option < 1 || option > 5) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "������Ч�������� 1 �� 5 ֮������֡�" << endl;
        }
        switch (option) {
        case 1: {
            borrowBook(username);
            break;
        }
        case 2: {
            returnBook(username);
            break;
        }
        case 3: {
            ifstream userFile("userData.txt");
            vector<UserData> userList = FileOperator::parseUserFile(userFile);
            userFile.close();

            for (const auto& user : userList) {
                if (user.getUserName() == username) {
                    auto borrowRecords = user.getBorrowRecords();
                    if (borrowRecords.empty()) {
                        cout << "�޽��ļ�¼" << endl;
                    }
                    else {
                        cout << "���ļ�¼����:" << endl;
                        for (const auto& record : borrowRecords) {
                            cout << "ͼ����: " << record.bookID << ", ����: " << record.count << endl;
                        }
                    }
                    break;
                }
            }
            break;
        }
        case 4: {
            userModifyPassword(username);
            break;
        }
        case 5: {
            exit = true;
            break;
        }
        }
        if (exit)
            return;
    }
}

// ������
int main() {
    initializeBookFile();
    while (true) {
        cout << endl;
        cout << "                 " << "��ӭʹ��ͼ�����ϵͳ" << endl << endl;
        cout << "1.����Ա��¼" << endl;
        cout << "2.�û���¼" << endl;
        cout << "3.�û�ע��" << endl;

        int choice;
        while (!(cin >> choice) || choice < 1 || choice > 3) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "������Ч�������� 1 �� 3 ֮������֡�" << endl;
        }

        switch (choice) {
        case 1: {
            if (adminLogin())
                adminMenu();
            break;
        }
        case 2: {
            string username = userLogin();
            if (!username.empty()) {
                userMenu(username);
            }
            break;
        }
        case 3: {
            userSignUp();
            break;
        }
        }
    }
    return 0;
}