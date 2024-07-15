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

void create_user(const string& name, const string& nim, const string& asal_kampus, const string& jurusan, const string& fakultas){
    MYSQL* conn = connect_db();
    if(conn){
       string query = "INSERT INTO mahasiswa (name, nim, asal_kampus, jurusan, fakultas) VALUES ('" + name + "', '" + nim + "', '" + asal_kampus + "', '" + jurusan + "', '" + fakultas +"')";
       if (mysql_query(conn, query.c_str())){
        cerr << "INSERT failed: " << mysql_error(conn) << endl;
       } else {
        cout << "User successfully added." << endl;
       }
       mysql_close(conn);
    }
}
