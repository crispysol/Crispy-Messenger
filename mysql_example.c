#include <mysql_connection.h>
#include <mysql_driver.h>

#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

#include <iostream>
using namespace std;

int main() {
	sql::mysql::MySQL_Driver *driver;
	sql::Connection	*con;
	sql::Statement	*stmt;

	driver = sql::mysql::get_mysql_driver_instance();
	con = driver->connect("tcp://127.0.0.1:3306", "root", "euchre");

	stmt = con->createStatement();
	stmt->execute("USE dhproductions");
	stmt->execute("DROP TABLE IF EXISTS test");
	stmt->execute("CREATE TABLE test(id INT, label CHAR(1))");
	stmt->execute("INSERT INTO test(id, label) VALUES (1, 'b')");

	sql::ResultSet * res = stmt->executeQuery("SELECT id, label FROM test ORDER BY id ASC");
	while (res->next()) {  
	  // You can use either numeric offsets...
	  cout << "id = " << res->getInt("id");
	  // ... or column names for accessing results. The latter is recommended.
	  cout << ", label = '" << res->getString("label") << "'" << endl;
	}

	delete res;
	delete stmt;
	delete con;
}
