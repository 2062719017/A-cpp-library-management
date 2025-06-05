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

    // Getter 和 Setter 方法
    string getBookName() const { return bookName; }
    string getBookID() const { return bookID; }
    int getBookCount() const { return bookCount; }
    void setBookCount(int count) { this->bookCount = count; }
};

// 用户数据类
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

    vector<BorrowRecord> borrowRecords; // 存储用户的借阅信息
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

    // 删除借阅记录
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

    // 获取指定图书的借阅数量
    int getBorrowedCount(string bookID) const {
        for (auto record : borrowRecords) {
            if (record.bookID == bookID) {
                return record.count;
            }
        }
        return 0;
    }
};

// 文件操作类
class FileOperator {
public:
    // 从文件中读取图书信息
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

    // 将图书信息写入文件
    static void writeBookFile(const vector<BookData>& bookList) {
        fstream file("book.txt", ios::out);
        for (const auto& book : bookList) {
            file << book.getBookName() << "#" << book.getBookID() << "#" << book.getBookCount() << endl;
        }
        file.close();
    }

    // 从文件中读取用户信息
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

    // 将用户信息写入文件
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

// 打印图书信息
void printBook(BookData& book) {
    cout << setw(40) << left << book.getBookName()
        << setw(20) << left << book.getBookID()
        << book.getBookCount() << endl;
}

// 初始化图书文件
void initializeBookFile() {
    ifstream file("book.txt");
    if (file.peek() == ifstream::traits_type::eof()) {
        ofstream outputFile("book.txt", ios::app);
        outputFile << "初始文件写入图书初始信息" << "#" << "0" << "#" << "0" << endl;
    }
    file.close();
}

// 管理员登录
bool adminLogin() {
    cout << "请输入管理员密码" << endl;
    string password;
    cin >> password;
    return password == "admin4211";
}

// 用户登录
string userLogin() {
    const int MAX_ATTEMPTS = 3;
    int attempts = 0;

    while (attempts < MAX_ATTEMPTS) {
        cout << "请输入账号" << endl;
        string username;
        cin >> username;

        if (username.empty()) {
            cout << "账号不能为空，请重新输入。" << endl;
            continue;
        }

        cout << "请输入密码" << endl;
        string password;
        cin >> password;

        if (password.empty()) {
            cout << "密码不能为空，请重新输入。" << endl;
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
        cout << "账号或密码错误，您还剩 " << MAX_ATTEMPTS - attempts << " 次尝试机会。" << endl;
    }

    cout << "登录失败，您的尝试次数已达到上限，请稍后再试。" << endl;
    return "";
}

// 用户注册
void userSignUp() {
    cout << "请输入用户名: ";
    string username;
    cin >> username;

    // 检查用户名是否已存在
    ifstream file("userData.txt");
    string line;
    while (getline(file, line)) {
        size_t pos1 = line.find('#');
        string storedUser = line.substr(0, pos1);
        if (storedUser == username) {
            cout << "该用户名已存在，请重新输入。" << endl;
            file.close();
            return;
        }
    }
    file.close();

    cout << "请输入密码: ";
    string password;
    cin >> password;

    cout << "请输入手机号码: ";
    long long phone;
    while (!(cin >> phone) || phone <= 0) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "输入的手机号码无效，请输入有效的手机号码。" << endl;
    }

    cout << "请输入学号: ";
    long long studentID;
    while (!(cin >> studentID) || studentID <= 0) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "输入的学号无效，请输入有效的学号。" << endl;
    }

    ofstream newFile("userData.txt", ios::app);
    if (newFile.is_open()) {
        newFile << username << "#" << password << "#" << phone << "#" << studentID << "#" << endl;
        cout << "注册成功！" << endl;
        newFile.close();
    }
    else {
        cout << "无法打开文件，注册失败。" << endl;
    }
}

// 添加图书信息
void addBook() {
    char choice;
    do {
        cout << "正在开始添加图书信息" << endl;
        cout << "请输入名称:" << endl;
        string name;
        cin >> name;
        cout << "请输入图书 ID:" << endl;
        string id;
        cin >> id;
        int count;
        cout << "请输入数量:" << endl;
        while (!(cin >> count) || count <= 0) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "输入的数量无效，请输入有效的数量。" << endl;
        }

        fstream bookFile("book.txt");
        vector<BookData> bookList = FileOperator::parseBookFile(bookFile);
        bookFile.close();

        bool bookExists = false;
        for (BookData& book : bookList) {
            if (book.getBookID() == id) {
                // 图书已存在，增加数量
                book.setBookCount(book.getBookCount() + count);
                bookExists = true;
                break;
            }
        }

        if (!bookExists) {
            // 图书不存在，添加新记录
            BookData newBook(name, id, count);
            bookList.push_back(newBook);
        }

        // 将更新后的图书信息写入文件
        FileOperator::writeBookFile(bookList);

        cout << "要继续添加图书，请输入 Y/y;停止添加，请输入 N/n" << endl;
        cin >> choice;
    } while (choice == 'Y' || choice == 'y');
}

// 用户借书功能
bool borrowBook(string username) {
    fstream bookFile("book.txt");
    vector<BookData> bookList = FileOperator::parseBookFile(bookFile);
    bookFile.close();

    // 打印可借阅图书列表
    cout << "可借阅图书列表如下:" << endl;
    cout << setw(40) << left << "名称"
        << setw(20) << left << "图书编号"
        << "数量" << endl;
    cout << string(70, '-') << endl;
    for (auto& book : bookList) {
        if (book.getBookCount() > 0) {
            printBook(book);
        }
    }

    cout << "请选择查询方式，可通过书名或图书编号查询" << endl;
    cout << "1.书名" << endl;
    cout << "2.编号" << endl;
    int queryOption;
    while (!(cin >> queryOption) || (queryOption != 1 && queryOption != 2)) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "输入无效，请输入 1 或 2。" << endl;
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
        cout << "请输入要借阅的图书名称" << endl;
        string bookName;
        cin >> bookName;
        for (BookData& book : updatedBookList) {
            if (book.getBookName() == bookName) {
                cout << "找到关键记录: ";
                printBook(book);
                found = true;
                cout << "请输入要借阅的图书数量" << endl;
                int count;
                while (!(cin >> count) || count <= 0 || count > book.getBookCount()) {
                    cin.clear();
                    cin.ignore(10000, '\n');
                    cout << "输入的数量无效，请输入有效的借阅数量。" << endl;
                }
                book.setBookCount(book.getBookCount() - count);
                if (currentUser) {
                    currentUser->addBorrowRecord(book.getBookID(), count);
                }
                cout << "借阅成功！" << endl;
            }
        }
        FileOperator::writeBookFile(updatedBookList);
        FileOperator::writeUserFile(userList);
        if (!found)
            cout << "未找到记录" << endl;
        break;
    }
    case 2: {
        cout << "请输入要借阅的图书编号" << endl;
        string bookID;
        cin >> bookID;
        for (BookData& book : updatedBookList) {
            if (book.getBookID() == bookID) {
                cout << "找到关键记录: ";
                printBook(book);
                found = true;
                cout << "请输入要借阅的图书数量" << endl;
                int count;
                while (!(cin >> count) || count <= 0 || count > book.getBookCount()) {
                    cin.clear();
                    cin.ignore(10000, '\n');
                    cout << "输入的数量无效，请输入有效的借阅数量。" << endl;
                }
                book.setBookCount(book.getBookCount() - count);
                if (currentUser) {
                    currentUser->addBorrowRecord(book.getBookID(), count);
                }
                cout << "借阅成功！" << endl;
            }
        }
        FileOperator::writeBookFile(updatedBookList);
        FileOperator::writeUserFile(userList);
        if (!found)
            cout << "未找到记录" << endl;
        break;
    }
    default:
        break;
    }
    cout << endl << endl;
    return found;
}

// 打印所有图书信息
void printAllBooks() {
    fstream file("book.txt", ios::in);
    if (!file.is_open()) {
        cout << "无法打开图书文件！" << endl;
        return;
    }
    vector<BookData> bookList = FileOperator::parseBookFile(file);
    file.close();

    cout << setw(40) << left << "名称"
        << setw(20) << left << "图书编号"
        << "数量" << endl;
    cout << string(70, '-') << endl;

    int index = 1;
    for (const auto& book : bookList) {
        cout << index << ". ";
        printBook(const_cast<BookData&>(book));
        index++;
    }
}

// 用户还书功能
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
            cout << "您没有借阅记录，无法还书！" << endl;
            return;
        }
        cout << "您的借阅记录如下：" << endl;
        cout << setw(20) << left << "图书编号"
            << setw(20) << left << "借阅数量" << endl;
        cout << string(40, '-') << endl;
        for (const auto& record : borrowRecords) {
            cout << setw(20) << left << record.bookID
                << setw(20) << left << record.count << endl;
        }
    }

    do {
        cout << "正在开始还书信息" << endl;
        cout << "请输入图书 ID:" << endl;
        string id;
        cin >> id;

        if (id.empty()) {
            cout << "图书 ID 不能为空，请重新输入。" << endl;
            continue;
        }

        cout << "请输入还书数量" << endl;
        int count;
        while (!(cin >> count) || count <= 0) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "输入的数量无效，请输入有效的还书数量。" << endl;
        }

        if (currentUser) {
            int borrowedCount = currentUser->getBorrowedCount(id);
            if (count > borrowedCount) {
                cout << "还书数量超过借阅数量，还书失败！" << endl;
                continue;
            }
        }

        bool bookExists = false;
        for (BookData& book : bookList) {
            if (book.getBookID() == id) {
                // 图书已存在，增加数量
                book.setBookCount(book.getBookCount() + count);
                bookExists = true;
                cout << "还书成功，当前图书 " << book.getBookName() << " 数量为: " << book.getBookCount() << endl;
                if (currentUser) {
                    currentUser->removeBorrowRecord(id, count);
                }
                break;
            }
        }

        if (!bookExists) {
            cout << "未找到该图书编号，还书失败！" << endl;
        }

        // 将更新后的图书信息写入文件
        FileOperator::writeBookFile(bookList);
        FileOperator::writeUserFile(userList);

        cout << "要继续还书，请输入 Y/y;停止还书，请输入 N/n" << endl;
        cin >> choice;
    } while (choice == 'Y' || choice == 'y');
}

// 打印所有用户信息
void printAllUsers() {
    ifstream file("userData.txt");
    if (!file.is_open()) {
        cout << "无法打开用户文件！" << endl;
        return;
    }
    vector<UserData> userList = FileOperator::parseUserFile(file);
    file.close();

    // 打印表头
    cout << setw(20) << left << "用户名"
        << setw(20) << left << "密码"
        << setw(20) << left << "手机号码"
        << setw(20) << left << "学号"
        << "借阅记录" << endl;
    cout << string(100, '-') << endl;

    for (const auto& user : userList) {
        cout << setw(20) << left << user.getUserName()
            << setw(20) << left << user.getPassword()
            << setw(20) << left << user.getPhoneNumber()
            << setw(20) << left << user.getStudentID();

        auto borrowRecords = user.getBorrowRecords();
        if (borrowRecords.empty()) {
            cout << "无借阅记录" << endl;
        }
        else {
            cout << endl;
            for (const auto& record : borrowRecords) {
                cout << setw(20) << ""
                    << setw(20) << ""
                    << setw(20) << ""
                    << setw(20) << ""
                    << "图书编号: " << record.bookID << ", 数量: " << record.count << endl;
            }
        }
    }
}

// 删除用户信息
void deleteUser() {
    cout << "请输入要删除的用户名" << endl;
    string username;
    cin >> username;

    ifstream file("userData.txt");
    if (!file.is_open()) {
        cout << "无法打开用户文件！" << endl;
        return;
    }
    vector<UserData> userList = FileOperator::parseUserFile(file);
    file.close();

    for (auto it = userList.begin(); it != userList.end(); ++it) {
        if (it->getUserName() == username) {
            userList.erase(it);
            FileOperator::writeUserFile(userList);
            cout << "用户删除成功！" << endl;
            return;
        }
    }
    cout << "未找到该用户！" << endl;
}

// 管理员修改用户密码
void adminModifyUserPassword() {
    cout << "请输入要修改密码的用户名: ";
    string username;
    cin >> username;

    ifstream file("userData.txt");
    if (!file.is_open()) {
        cout << "无法打开用户文件！" << endl;
        return;
    }
    vector<UserData> userList = FileOperator::parseUserFile(file);
    file.close();

    for (auto& user : userList) {
        if (user.getUserName() == username) {
            cout << "请输入新密码: ";
            string newPassword;
            cin >> newPassword;
            user.setPassword(newPassword);
            FileOperator::writeUserFile(userList);
            cout << "密码修改成功！" << endl;
            return;
        }
    }
    cout << "未找到该用户！" << endl;
}

// 用户修改自己的密码
void userModifyPassword(string username) {
    ifstream file("userData.txt");
    if (!file.is_open()) {
        cout << "无法打开用户文件！" << endl;
        return;
    }
    vector<UserData> userList = FileOperator::parseUserFile(file);
    file.close();

    for (auto& user : userList) {
        if (user.getUserName() == username) {
            cout << "请输入当前密码: ";
            string currentPassword;
            cin >> currentPassword;
            if (currentPassword != user.getPassword()) {
                cout << "当前密码不正确，修改失败！" << endl;
                return;
            }
            cout << "请输入新密码: ";
            string newPassword;
            cin >> newPassword;
            user.setPassword(newPassword);
            FileOperator::writeUserFile(userList);
            cout << "密码修改成功！" << endl;
            return;
        }
    }
    cout << "未找到该用户！" << endl;
}

// 管理员菜单
void adminMenu() {
    bool exit = false;
    while (true) {
        cout << "请选择要执行的操作" << endl;
        cout << "1.添加图书" << endl;
        cout << "2.打印所有图书信息" << endl;
        cout << "3.打印所有用户信息" << endl;
        cout << "4.删除用户" << endl;
        cout << "5.修改用户密码" << endl;
        cout << "6.退出登录" << endl;
        int option;
        while (!(cin >> option) || option < 1 || option > 6) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "输入无效，请输入 1 到 6 之间的数字。" << endl;
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

// 用户菜单
void userMenu(string username) {
    bool exit = false;
    while (true) {
        cout << "请选择要执行的操作" << endl;
        cout << "1.借阅" << endl;
        cout << "2.归还" << endl;
        cout << "3.查询借阅记录" << endl;
        cout << "4.修改密码" << endl;
        cout << "5.退出登录" << endl;
        int option;
        while (!(cin >> option) || option < 1 || option > 5) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "输入无效，请输入 1 到 5 之间的数字。" << endl;
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
                        cout << "无借阅记录" << endl;
                    }
                    else {
                        cout << "借阅记录如下:" << endl;
                        for (const auto& record : borrowRecords) {
                            cout << "图书编号: " << record.bookID << ", 数量: " << record.count << endl;
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

// 主函数
int main() {
    initializeBookFile();
    while (true) {
        cout << endl;
        cout << "                 " << "欢迎使用图书管理系统" << endl << endl;
        cout << "1.管理员登录" << endl;
        cout << "2.用户登录" << endl;
        cout << "3.用户注册" << endl;

        int choice;
        while (!(cin >> choice) || choice < 1 || choice > 3) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "输入无效，请输入 1 到 3 之间的数字。" << endl;
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