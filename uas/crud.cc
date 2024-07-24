#include <iostream>
#include <mysql/mysql.h>
#include <sstream>

using namespace std;

const char* hostname = "127.0.0.1";
const char* user = "root";
const char* pass = "123";
const char* dbname = "cpp_crud";
unsigned int port = 31235;
const char* unixsocket = NULL;
unsigned long clientflag = 0;

MYSQL* connect_db() {
    MYSQL* conn = mysql_init(0);
    if (conn) {
        conn = mysql_real_connect(conn, hostname, user, pass, dbname, port, unixsocket, clientflag);
        if (conn) {
            cout << "Connected to the database successfully." << endl;
        } else {
            cerr << "Connection failed: " << mysql_error(conn) << endl;
        }
    } else {
        cerr << "mysql_init failed" << endl;
    }
    return conn;
}

// Admin Functions
void create_item(const string& nama_barang, int qty, int harga) {
    MYSQL* conn = connect_db();
    if (conn) {
        stringstream query;
        query << "INSERT INTO admin (nama_barang, qty, harga, waktu) VALUES ('" << nama_barang << "', " << qty << ", " << harga << ", NOW())";
        if (mysql_query(conn, query.str().c_str())) {
            cerr << "INSERT failed: " << mysql_error(conn) << endl;
        } else {
            cout << "Item successfully added." << endl;
        }
        mysql_close(conn);
    }
}

void get_items() {
    MYSQL* conn = connect_db();
    if (conn) {
        if (mysql_query(conn, "SELECT * FROM admin")) {
            cerr << "SELECT failed: " << mysql_error(conn) << endl;
            mysql_close(conn);
            return;
        }

        MYSQL_RES* res = mysql_store_result(conn);
        if (res == nullptr) {
            cerr << "mysql_store_result failed: " << mysql_error(conn) << endl;
            mysql_close(conn);
            return;
        }

        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res))) {
            cout << "ID: " << row[0] << ", Nama Barang: " << row[1] << ", Quantity: " << row[2] << ", Harga: " << row[3] << ", Waktu: " << row[4] << endl;
        }

        mysql_free_result(res);
        mysql_close(conn);
    }
}

void update_item(int item_id, const string& nama_barang, int qty, int harga) {
    MYSQL* conn = connect_db();
    if (conn) {
        stringstream query;
        query << "UPDATE admin SET nama_barang = '" << nama_barang << "', qty = " << qty << ", harga = " << harga << ", waktu = NOW() WHERE id = " << item_id;
        if (mysql_query(conn, query.str().c_str())) {
            cerr << "UPDATE failed: " << mysql_error(conn) << endl;
        } else {
            cout << "Item successfully updated." << endl;
        }
        mysql_close(conn);
    }
}

void delete_item(int item_id) {
    MYSQL* conn = connect_db();
    if (conn) {
        stringstream query;
        query << "DELETE FROM admin WHERE id = " << item_id;
        if (mysql_query(conn, query.str().c_str())) {
            cerr << "DELETE failed: " << mysql_error(conn) << endl;
        } else {
            cout << "Item successfully deleted." << endl;
        }
        mysql_close(conn);
    }
}

// User Functions
void view_items() {
    get_items();
}

void buy_item(const string& user_name, int item_id, int qty) {
    MYSQL* conn = connect_db();
    if (conn) {
        stringstream select_query;
        select_query << "SELECT nama_barang, harga FROM admin WHERE id = " << item_id;
        if (mysql_query(conn, select_query.str().c_str())) {
            cerr << "SELECT failed: " << mysql_error(conn) << endl;
            mysql_close(conn);
            return;
        }

        MYSQL_RES* res = mysql_store_result(conn);
        if (res == nullptr) {
            cerr << "mysql_store_result failed: " << mysql_error(conn) << endl;
            mysql_close(conn);
            return;
        }

        MYSQL_ROW row = mysql_fetch_row(res);
        if (!row) {
            cerr << "Item not found" << endl;
            mysql_free_result(res);
            mysql_close(conn);
            return;
        }

        string nama_barang = row[0];
        int harga = atoi(row[1]);
        int total_harga = harga * qty;

        mysql_free_result(res);

        stringstream insert_query;
        insert_query << "INSERT INTO users (nama, nama_barang, qty, total_harga, waktu_pembelian) VALUES ('"
                     << user_name << "', '" << nama_barang << "', " << qty << ", " << total_harga << ", NOW())";
        if (mysql_query(conn, insert_query.str().c_str())) {
            cerr << "INSERT failed: " << mysql_error(conn) << endl;
        } else {
            cout << "Purchase successful. Total Harga: " << total_harga << endl;
        }

        mysql_close(conn);
    }
}

void view_purchase_history() {
    MYSQL* conn = connect_db();
    if (conn) {
        if (mysql_query(conn, "SELECT * FROM users")) {
            cerr << "SELECT failed: " << mysql_error(conn) << endl;
            mysql_close(conn);
            return;
        }

        MYSQL_RES* res = mysql_store_result(conn);
        if (res == nullptr) {
            cerr << "mysql_store_result failed: " << mysql_error(conn) << endl;
            mysql_close(conn);
            return;
        }

        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res))) {
            cout << "User ID: " << row[0] << ", Nama: " << row[1] << ", Nama Barang: " << row[2] << ", Quantity: " << row[3]
                 << ", Total Harga: " << row[4] << ", Waktu Pembelian: " << row[5] << endl;
        }

        mysql_free_result(res);
        mysql_close(conn);
    }
}

bool admin_login() {
    string username, password;
    cout << "Enter Admin Username: ";
    cin >> username;
    cout << "Enter Admin Password: ";
    cin >> password;

    if (username == "admin" && password == "admin") {
        return true;
    } else {
        cout << "Invalid admin credentials." << endl;
        return false;
    }
}

int main() {
    int choice;
    cout << "Login as:\n";
    cout << "1. Admin\n";
    cout << "2. User\n";
    cout << "Enter choice: ";
    cin >> choice;

    if (choice == 1) {
        if (!admin_login()) {
            return 0;
        }
    } else if (choice == 2) {
        // User mode does not need a password, just a username
        string user_name;
        cout << "Enter User Name: ";
        cin.ignore();
        getline(cin, user_name);
        while (true) {
            cout << "\nUser Menu:\n";
            cout << "1. View Items\n";
            cout << "2. Buy Item\n";
            cout << "3. View Purchase History\n";
            cout << "4. Exit\n";
            cout << "Enter choice: ";
            cin >> choice;

            if (choice == 1) {
                view_items();
            } else if (choice == 2) {
                int item_id, qty;
                cout << "Enter Item ID: ";
                cin >> item_id;
                cout << "Enter Quantity: ";
                cin >> qty;
                buy_item(user_name, item_id, qty);
            } else if (choice == 3) {
                view_purchase_history();
            } else if (choice == 4) {
                break;
            } else {
                cout << "Invalid choice. Please try again." << endl;
            }
        }
        return 0;
    }

    // Admin Menu
    while (true) {
        cout << "\nAdmin Menu:\n";
        cout << "1. Add Item\n";
        cout << "2. Update Item\n";
        cout << "3. Delete Item\n";
        cout << "4. View Items\n";
        cout << "5. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 1) {
            string nama_barang;
            int qty, harga;
            cout << "Enter Item Name: ";
            cin.ignore();
            getline(cin, nama_barang);
            cout << "Enter Quantity: ";
            cin >> qty;
            cout << "Enter Price: ";
            cin >> harga;
            create_item(nama_barang, qty, harga);
        } else if (choice == 2) {
            int item_id;
            string nama_barang;
            int qty, harga;
            cout << "Enter Item ID: ";
            cin >> item_id;
            cout << "Enter Item Name: ";
            cin.ignore();
            getline(cin, nama_barang);
            cout << "Enter Quantity: ";
            cin >> qty;
            cout << "Enter Price: ";
            cin >> harga;
            update_item(item_id, nama_barang, qty, harga);
        } else if (choice == 3) {
            int item_id;
            cout << "Enter Item ID to Delete: ";
            cin >> item_id;
            delete_item(item_id);
        } else if (choice == 4) {
            view_items();
        } else if (choice == 5) {
            break;
        } else {
            cout << "Invalid choice. Please try again." << endl;
        }
    }

    return 0;
}
