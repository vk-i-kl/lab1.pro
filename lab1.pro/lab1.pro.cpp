#include <iostream>
#include <thread>
#include <mutex>
#include <list>
#include <algorithm>
#include <string>
#include <Windows.h>
#include <clocale>

using namespace std;

void Thread1() {
    cout << "1\n";
}

void Thread2() {
    cout << "2\n";
}

void Task_1_2_1_and_2() {
    cout << "Пункти 1.2.1 та 1.2.2 \n";
    thread t1(Thread1);
    thread t2(Thread2);

    t1.detach();
    t2.detach();

    Sleep(100);
}

list<int> globalList;
mutex listMutex;


void AddToList_123(int startVal) {
    for (int i = 0; i < 10; ++i) {
        globalList.push_back(startVal + i);
        cout << "[1.2.3 Add] Додано: " << startVal + i << "\n";
    }
}

void ListContains_123(int targetVal) {
    for (int i = 0; i < 10; ++i) {
        bool found = (find(globalList.begin(), globalList.end(), targetVal) != globalList.end());
        cout << "[1.2.3 Check] Спроба " << i + 1 << " - Чи є " << targetVal << "? " << (found ? "Так" : "Ні") << "\n";
    }
}

void Task_1_2_3() {
    cout << " Пункт 1.2.3 (Без м'ютексів) \n";
    globalList.clear(); 
    thread t1(AddToList_123, 10);
    thread t2(ListContains_123, 15);
    t1.join();
    t2.join();
}

void AddToList_124(int startVal) {
    for (int i = 0; i < 10; ++i) {
        listMutex.lock();
        globalList.push_back(startVal + i);
        cout << "[1.2.4 Add] Додано: " << startVal + i << "\n";
        listMutex.unlock();
    }
}

void ListContains_124(int targetVal) {
    for (int i = 0; i < 10; ++i) {
        listMutex.lock();
        bool found = (find(globalList.begin(), globalList.end(), targetVal) != globalList.end());
        cout << "[1.2.4 Check] Спроба " << i + 1 << " - Чи є " << targetVal << "? " << (found ? "Так" : "Ні") << "\n";
        listMutex.unlock();
    }
}

void Task_1_2_4() {
    cout << " Пункт 1.2.4 (Ручні м'ютекси) \n";
    globalList.clear();
    thread t1(AddToList_124, 10);
    thread t2(ListContains_124, 15);
    t1.join();
    t2.join();
}

void AddToList_125(int startVal) {
    lock_guard<mutex> lock(listMutex);
    globalList.push_back(startVal);
    cout << "[AddToList 1.2.5] Додано елемент: " << startVal << "\n";
}

void ListContains_125(int targetVal) {
    lock_guard<mutex> lock(listMutex);
    bool found = (find(globalList.begin(), globalList.end(), targetVal) != globalList.end());
    cout << "[ListContains 1.2.5] Чи є " << targetVal << " у списку? " << (found ? "Так" : "Ні") << "\n";
}

void Task_1_2_5() {
    cout << "Пункт 1.2.5 (std::lock_guard) \n";
    globalList.clear();
    int startValue = 10;
    int targetValue = 15;

    for (int i = 0; i < 10; ++i) {
        thread tAdd(AddToList_125, startValue + i);
        thread tCheck(ListContains_125, targetValue);

        tAdd.detach();
        tCheck.detach();
    }

    Sleep(500);
}

class someData {
public:
    string name;
    string surname;
    string address;
    int age;

    someData(string n = "За замовчуванням", string s = "Користувач", string addr = "Невідомо", int a = 0)
        : name(n), surname(s), address(addr), age(a) {}
};

class exchangePerson {
public:
    someData data;
    mutex m;

    static void JohnDoe(exchangePerson& obj) {
        lock_guard<mutex> lock(obj.m);
        obj.data.name = "Тарас";
        obj.data.surname = "Шевченко";
        obj.data.address = "Канів";
        obj.data.age = 47;
    }

    static void JacobSmith(exchangePerson& obj) {
        lock_guard<mutex> lock(obj.m);
        obj.data.name = "Іван";
        obj.data.surname = "Франко";
        obj.data.address = "Нагуєвичі";
        obj.data.age = 59;
    }

    // 1.2.6
    static void Swap126(exchangePerson& a, exchangePerson& b) {
        if (&a == &b) return;

        lock(a.m, b.m);
        lock_guard<mutex> lock_a(a.m, adopt_lock);
        lock_guard<mutex> lock_b(b.m, adopt_lock);

        swap(a.data, b.data);
        cout << "[Swap 1.2.6] Дані успішно обміняно!\n";
    }

    // 1.2.7
    static void Swap127(exchangePerson& a, exchangePerson& b) {
        if (&a == &b) return;

        unique_lock<mutex> lock_a(a.m, defer_lock);
        unique_lock<mutex> lock_b(b.m, defer_lock);
        lock(lock_a, lock_b);

        swap(a.data, b.data);
        cout << "[Swap 1.2.7] Дані успішно обміняно!\n";
    }

    void PrintData(const string& objName) {
        lock_guard<mutex> lock(m);
        cout << objName << " -> Name: " << data.name << " " << data.surname
            << ", Address: " << data.address << ", Age: " << data.age << "\n";
    }
};

void Task_1_2_6_and_7() {
    cout << " Пункти 1.2.6 та 1.2.7 (Deadlock та Swap) \n";
    exchangePerson person1, person2;

    thread tJohn(exchangePerson::JohnDoe, std::ref(person1));
    thread tJacob(exchangePerson::JacobSmith, std::ref(person2));
    tJohn.detach();
    tJacob.detach();

    Sleep(100);

    cout << "ДО обміну:\n";
    person1.PrintData("Особа 1");
    person2.PrintData("Особа 2");

    thread tSwap(exchangePerson::Swap126, std::ref(person1), std::ref(person2));
    tSwap.join();

    cout << "ПІСЛЯ обміну:\n";
    person1.PrintData("Особа 1");
    person2.PrintData("Особа 2");
}
int main() {
   
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "ukr");

    cout << "Початок роботи \n\n";

    Task_1_2_1_and_2();
    system("pause");
    cout << "\n";

    Task_1_2_3();
    system("pause");
    cout << "\n";

    Task_1_2_4();
    system("pause");
    cout << "\n";

    Task_1_2_5();
    system("pause");
    cout << "\n";

    Task_1_2_6_and_7();

    cout << "\nКінець\n";
    system("pause");

    return 0;
}