#include <occi.h>
#include <iostream>
#include <cstring>
#include <string>
#include <cctype>

using namespace std;
using namespace oracle::occi;
using oracle::occi::Environment;
using oracle::occi::Connection;

struct Employee {
	int employeeNumber;
	string lastName;
	string firstName;
	string email;
	string phone;
	string extension;
	string reportsTo;
	string jobTitle;
	string city;
};


bool isNumber(string input) {
	bool state = true;
	if (input.length() == 0) {
		state = false;
	}
	else {
		for (int counter = 0; counter < input.length(); ++counter) {
			if (!isdigit(input[counter])) {
				state = false;
				break;
			}
		}
	}
	return state;
};

string getEmployeeNumber()
{
	bool stop = false;
	string result;
	while (!stop)
	{
		cout << "Enter employee number: ";
		char employeeNumber[255];
		cin >> employeeNumber;
		cin.clear();
		cin.ignore(1000, '\n');
		if (isNumber(employeeNumber))
		{
			result = employeeNumber;
			stop = true;
		}
		else
		{
			cout << "Invalid employee number, please try again: " << endl;
		}
	}
	return result;
};

void printMenu() {
	cout << "********************* HR Menu *********************" << endl;
	cout << "1)	Find Employee" << endl;
	cout << "2)	Employees Report" << endl;
	cout << "3)	Add Employee" << endl;
	cout << "4)	Update Employee" << endl;
	cout << "5)	Remove Employee" << endl;
	cout << "0)	Exit" << endl;
}

int menu() {
	printMenu();
	bool stop = false;
	string user_input = "";
	int user_option = 0;
	while (!stop) {
		cin >> user_input;
		cin.clear();
		cin.ignore(1000, '\n');
		if (isNumber(user_input)) {
			user_option = stoi(user_input);
			if (user_option >= 0 && user_option <= 5) {
				stop = true;
			}
			else {
				cout << "Invalid option, please choose option from 1 to 5: ";
			}
		}
		else {
			cout << "Invalid input, please enter a number: ";
		}
	}
	return user_option;
};

int findEmployeee(Connection* conn, int employeeNumber, struct Employee* emp) {
	if (conn != nullptr && emp != nullptr) {
		try {
			Statement* statement = conn->createStatement();
			string query = "";
			query += "SELECT *";
			query += " FROM(";
			query += " SELECT employees.employeenumber,";
			query += " employees.lastname,";
			query += " employees.firstname,";
			query += " employees.email,";
			query += " offices.phone,";
			query += " employees.extension,";
			query += " managers.firstname || ', ' || managers.lastname, ";
			query += " employees.jobtitle,";
			query += " offices.city";
			query += " FROM dbs211_employees employees";
			query += " INNER JOIN dbs211_offices offices ON employees.officecode = offices.officecode";
			query += " INNER JOIN dbs211_employees managers";
			query += " ON employees.reportsto = managers.employeenumber";
			query += " )";
			query += " WHERE employeenumber = ";
			query += to_string(employeeNumber);
			ResultSet* resultset = statement->executeQuery(query);

			if (resultset->next()) {
				emp->employeeNumber = resultset->getInt(1);
				emp->lastName = resultset->getString(2);
				emp->firstName = resultset->getString(3);
				emp->email = resultset->getString(4);
				emp->phone = resultset->getString(5);
				emp->extension = resultset->getString(6);
				emp->reportsTo = resultset->getString(7);
				emp->jobTitle = resultset->getString(8);
				emp->city = resultset->getString(9);

				delete[] resultset;
				resultset = nullptr;
				conn->terminateStatement(statement);

				return 1;
			}
			else {
				return 0;
			}
		}
		catch (SQLException& err) {
			cout << err.getErrorCode() << ": " << err.getMessage();
		}
	}
	else {
		cout << "Invaid parameters" << endl;
		return 0;
	}
};

void displayEmployee(Connection* conn, struct Employee* emp) {
	if (conn != nullptr) {
		int employeeNumber = stoi(getEmployeeNumber());
		if (findEmployeee(conn, employeeNumber, emp)) {
			cout << "employeeNumber = " << emp->employeeNumber << endl;
			cout << "lastName = " << emp->lastName << endl;
			cout << "firstName = " << emp->firstName << endl;
			cout << "email = " << emp->email << endl;
			cout << "phone = " << emp->phone << endl;
			cout << "extension = " << emp->extension << endl;
			cout << "reportsTo = " << emp->reportsTo << endl;
			cout << "jobTitle = " << emp->jobTitle << endl;
			cout << "city = " << emp->city << endl;
		}
		else {
			cout << "Employee " << employeeNumber << " does not exist.";
		}
	}
	else {
		cout << "Invaid parameters" << endl;
	}
};

void printHeader() {
	cout.width(8);
	cout << "E";
	cout.width(20);
	cout << "Employee Name";
	cout.width(35);
	cout << "Email";
	cout.width(20);
	cout << "Phone";
	cout.width(10);
	cout << "Ext";
	cout.width(25);
	cout << "Manager" << endl;
}

void displayAllEmployees(Connection* conn) {
	try {
		Statement* statement = conn->createStatement();
		string query = "";
		query += "SELECT *";
		query += " FROM(";
		query += " SELECT employees.employeenumber,";
		query += " employees.lastname,";
		query += " employees.firstname,";
		query += " employees.email,";
		query += " offices.phone,";
		query += " employees.extension,";
		query += " managers.firstname || ', ' || managers.lastname, ";
		query += " employees.jobtitle,";
		query += " offices.city";
		query += " FROM dbs211_employees employees";
		query += " INNER JOIN dbs211_offices offices ON employees.officecode = offices.officecode";
		query += " INNER JOIN dbs211_employees managers";
		query += " ON employees.reportsto = managers.employeenumber";
		query += " )";
		ResultSet* resultset = statement->executeQuery(query);

		bool empty = true;
		while (resultset->next()) {
			if (empty) {
				printHeader();
				empty = false;
			}
			string employee_fullname = resultset->getString(2) + " " + resultset->getString(3);
			string manager_fullname = resultset->getString(7);
			cout.width(8);
			cout << resultset->getInt(1);
			cout.width(20);
			cout << employee_fullname;
			cout.width(35);
			cout << resultset->getString(4);
			cout.width(20);
			cout << resultset->getString(5);
			cout.width(10);
			cout << resultset->getString(6);
			cout.width(25);
			cout << manager_fullname << endl;
		}
		if (empty) {
			cout << "There is no employees’ information to be displayed." << endl;
		}
		delete[] resultset;
		resultset = nullptr;
		conn->terminateStatement(statement);
	}
	catch (SQLException& err) {
		cout << err.getErrorCode() << ": " << err.getMessage();
	}
}

void insertEmployee(Connection* conn, struct Employee* emp) {
	if (conn != nullptr) {
		try {
			if (!findEmployeee(conn, emp->employeeNumber, emp)) {
				Statement* statement = conn->createStatement();
				string query = "";
				query += "INSERT INTO dbs211_employees(employeenumber, lastname, firstname, extension, email, officecode, reportsto, jobtitle)";
				query += " VALUES (";
				query += " " + to_string(emp->employeeNumber);
				query += ", '" + emp->lastName + "'";
				query += ", '" + emp->firstName + "'";
				query += ", '" + emp->extension + "'";
				query += ", '" + emp->email + "'";
				query += ", '1'";
				query += ", 1002";
				query += ", '" + emp->jobTitle + "'";
				query += ")";
				statement->executeUpdate(query);
				cout << "The new employee is added successfully." << endl;
			}
			else {
				cout << "An employee with the same employee number exists." << endl;
			}
		}
		catch (SQLException& err) {
			cout << err.getErrorCode() << ": " << err.getMessage();
		}
	}
	else {
		cout << "Invaid parameters" << endl;
	}
}

void updateEmployee(Connection* conn, int employeeNumber) {
	if (conn != nullptr) {
		try {

		}
		catch (SQLException & err) {
			cout << err.getErrorCode() << ": " << err.getMessage();
		}
	}
	else {
		cout << "Invaid parameters" << endl;
	}
}

int main() {
	try {
		Environment* env = nullptr;
		Connection* conn = nullptr;

		string username = "dbs211_202b18";
		string password = "19060254";
		string server = "myoracle12c.senecacollege.ca:1521/oracle12c";

		env = Environment::createEnvironment(Environment::DEFAULT);
		conn = env->createConnection(username, password, server);

		Employee* emp = new Employee();
		int option = 0;
		while ((option = menu()) != 0) {
			if (option == 1) {
				displayEmployee(conn, emp);
			}
			else if (option == 2) {
				displayAllEmployees(conn);
			}
			else if (option == 3) {
				cout << "Employee Number: ";
				cin >> emp->employeeNumber;
				cin.clear();
				cin.ignore(1000, '\n');
				cout << "Last Name: ";
				cin >> emp->lastName;
				cin.clear();
				cin.ignore(1000, '\n');
				cout << "First Name: ";
				cin >> emp->firstName;
				cin.clear();
				cin.ignore(1000, '\n');
				cout << "Email: ";
				cin >> emp->email;
				cin.clear();
				cin.ignore(1000, '\n');
				cout << "Extension: ";
				cin >> emp->extension;
				cin.clear();
				cin.ignore(1000, '\n');
				cout << "Job Title: ";
				cin >> emp->jobTitle;
				cin.clear();
				cin.ignore(1000, '\n');
				cout << "City: ";
				cin >> emp->city;
				cin.clear();
				cin.ignore(1000, '\n');
				insertEmployee(conn, emp);
			}
		}

		delete emp;
		emp = nullptr;

		env->terminateConnection(conn);
		Environment::terminateEnvironment(env);
	}
	catch (SQLException& err) {
		cout << err.getErrorCode() << ": " << err.getMessage();
	}
	return 0;
} 