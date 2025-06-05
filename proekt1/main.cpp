#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <memory>
#include <regex>
#include <windows.h>

using namespace std;

class Income {
protected:
    string date;
    string source;
    int amount;

public:
    Income(const string& d, const string& s, int a)
        : date(d), source(s), amount(a) {
    }

    virtual string toString() const {
        return date + " \"" + source + "\" " + to_string(amount);
    }

    virtual void print(int index) const {
        cout << index + 1 << ") ����: " << date
            << ", ��������: " << source
            << ", �����: " << amount << " ���.";
    }

    static shared_ptr<Income> fromString(const string& line);
    virtual ~Income() = default;
};

class PhysicalIncome : public Income {
public:
    PhysicalIncome(const string& d, const string& s, int a)
        : Income(d, s, a) {
    }

    string toString() const override {
        return Income::toString() + " physical";
    }

    void print(int index) const override {
        Income::print(index);
        cout << ", ���: ��������";
    }
};

class DigitalIncome : public Income {
public:
    DigitalIncome(const string& d, const string& s, int a)
        : Income(d, s, a) {
    }

    string toString() const override {
        return Income::toString() + " digital";
    }

    void print(int index) const override {
        Income::print(index);
        cout << ", ���: ��������";
    }
};

shared_ptr<Income> Income::fromString(const string& line) {
    istringstream iss(line);
    string date, dummy, source, type;
    int amount;

    iss >> date;
    getline(iss, dummy, '"');
    getline(iss, source, '"');
    iss >> amount >> type;

    if (type == "physical") {
        return make_shared<PhysicalIncome>(date, source, amount);
    }
    else if (type == "digital") {
        return make_shared<DigitalIncome>(date, source, amount);
    }
    else {
        return make_shared<Income>(date, source, amount);
    }
}

class IncomeManager {
private:
    string filename;
    vector<shared_ptr<Income>> incomes;

public:
    IncomeManager(const string& fname) : filename(fname) {
        loadFromFile();
    }

    void addIncome(shared_ptr<Income> income) {
        incomes.push_back(income);
        saveToFileAppend(income);
    }

    void displayAll() const {
        if (incomes.empty()) {
            cout << "������� ���.\n";
        }
        else {
            for (size_t i = 0; i < incomes.size(); ++i) {
                incomes[i]->print(i);
                cout << endl;
            }
        }
    }

    void deleteAll() {
        incomes.clear();
        ofstream fout(filename, ios::trunc);
        fout.close();
        cout << "��� ������ �������.\n";
    }

    void deleteByIndex(int index) {
        if (index >= 0 && index < (int)incomes.size()) {
            incomes.erase(incomes.begin() + index);
            saveAll();
            cout << "������ �������.\n";
        }
        else {
            cout << "�������� �����.\n";
        }
    }

    void showDeletable() const {
        if (incomes.empty()) {
            cout << "������ ����.\n";
            return;
        }
        cout << "�������� ������ ��� ��������:\n";
        displayAll();
    }

private:
    void loadFromFile() {
        incomes.clear();
        ifstream fin(filename);
        string line;
        while (getline(fin, line)) {
            if (!line.empty())
                incomes.push_back(Income::fromString(line));
        }
    }

    void saveToFileAppend(shared_ptr<Income> income) {
        ofstream fout(filename, ios::app);
        fout << income->toString() << endl;
        fout.close();
    }

    void saveAll() {
        ofstream fout(filename, ios::trunc);
        for (const auto& income : incomes) {
            fout << income->toString() << endl;
        }
        fout.close();
    }
};

void showMenu() {
    cout << "\n==== ���� ====\n";
    cout << "1. �������� ����� ������\n";
    cout << "2. �������� ��� ������\n";
    cout << "3. ������� ��� ������\n";
    cout << "4. ������� ���������� ������\n";
    cout << "5. �����\n";
    cout << "�������� �����: ";
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    IncomeManager manager("income.txt");
    int choice;

    regex date_re(R"((\d{4}\.\d{2}\.\d{2}))");
    regex amount_re(R"(\b(\d+)\b)");
    regex source_re(R"([\"']([^\"']+)[\"'])");
    regex type_re(R"(\b(physical|digital)\b)");

    while (true) {
        showMenu();
        cin >> choice;
        cin.ignore();

        if (choice == 1) {
            string input;
            cout << "������� ������ (� ����� �������, ��������: \"�������\" 2024.05.22 physical 50000):\n";
            getline(cin, input);

            smatch match;
            string date, source, type;
            int amount = 0;

            if (regex_search(input, match, date_re)) date = match.str(1);
            if (regex_search(input, match, amount_re)) amount = stoi(match.str(1));
            if (regex_search(input, match, source_re)) source = match.str(1);
            if (regex_search(input, match, type_re)) type = match.str(1);

            shared_ptr<Income> income;
            if (type == "physical") {
                income = make_shared<PhysicalIncome>(date, source, amount);
            }
            else if (type == "digital") {
                income = make_shared<DigitalIncome>(date, source, amount);
            }
            else {
                income = make_shared<Income>(date, source, amount);
            }

            manager.addIncome(income);
            cout << "������ ���������.\n";

        }
        else if (choice == 2) {
            manager.displayAll();

        }
        else if (choice == 3) {
            manager.deleteAll();

        }
        else if (choice == 4) {
            manager.showDeletable();
            int delIndex;
            cout << "������� ����� ��� ��������: ";
            cin >> delIndex;
            cin.ignore();
            manager.deleteByIndex(delIndex - 1);

        }
        else if (choice == 5) {
            cout << "����� �� ���������.\n";
            break;

        }
        else {
            cout << "�������� �����. ���������.\n";
        }
    }

    return 0;
}
