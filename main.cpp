#include <fstream>
#include <vector>
#include <string>
#include <iostream>

class Student {
    std::string name;
    int year;
public:
    std::ostream friend & operator<<(std::ostream &os, const Student& u)  {
        os << "{" << u.name << ' ' <<", year: " << u.year << "}";
        return os;
    }
    [[nodiscard]] std::string getName() const {
        return name;
    }
    [[nodiscard]] int getYear() const {
        return year;
    }
    Student() : name(""), year(0) {}
    Student(std::string name_, int year_) : name(std::move(name_)), year(year_) {}
    Student(const Student& other) : name(other.name), year(other.year){}
    Student& operator=(const Student& other){
        name = other.name;
        year = other.year;
        return *this;
    }
    ~Student() {}
};

class Task {
    Student assignee;
    int priority;
    std::string description;
public:
    int friend operator <=> (const Task& lhs, const Task& rhs) {
        return lhs.priority - rhs.priority;
    }
    [[nodiscard]] int getPriority() const {
        return priority;
    }
    [[nodiscard]] std::string getDescription() const {
        return description;
    }
    [[nodiscard]] Student getAssignee() const {
        return assignee;
    }
    Task(const Student& assignee_, int priority_, std::string description_) : assignee(assignee_), priority(priority_), description(std::move(description_)) {}
    std::ostream friend & operator<<(std::ostream &os, const Task& t)  {
        os << "{" << t.description << ", priority " << t.priority << ", assigned to " << t.assignee << "}";
        return os;
    }
};

class Heap {
    std::vector<Task> elements;
    void down_heapify(unsigned int node) {
        unsigned int i = node;
        while(2 * i + 1 < elements.size()) {
            unsigned int target = i;
            if(elements[2 * i + 1] < elements[target]) {
                target = 2 * i + 1;
            }
            if(2 * i + 2 < elements.size() && elements[2 * i + 2] < elements[target]) {
                target = 2 * i + 2;
            }
            if(target == i) {
                break;
            }
            std::swap(elements[i], elements[target]);
            i = target;
        }
    }
    void up_heapify(unsigned int i) {
        while(i != 0 && elements[i] < elements[(i - 1) / 2]) {
            std::swap(elements[i], elements[(i - 1) / 2]);
            i = (i - 1) / 2;
        }
    }
public:
    void push(const Task& task) {
        elements.push_back(task);
        up_heapify(elements.size() - 1);
    }
    Task pop() {
        auto task = elements[0];
        elements[0] = elements[elements.size() - 1];
        elements.pop_back();
        down_heapify(0);
        return task;
    }
    [[nodiscard]] Task& get() {
        return elements[0];
    }
    [[nodiscard]] bool empty() const {
        return elements.empty();
    }
    [[nodiscard]] unsigned int size() const {
        return elements.size();
    }
    std::ostream friend & operator<<(std::ostream &os, const Heap& h)  {
        os<<"Heap of size "<<h.size()<<"\n";
        // print vector
        for (const auto & element : h.elements)
            os << element << ' ';
        return os;
    }
};

int main(){
    Heap h;
    std::vector<Student> users;
    int user_nr;
    std::cin >> user_nr;
    users.resize(user_nr);
    for(int i = 0; i < user_nr; i++) {
        std::string name;
        int year;
        std::cin >> name >> year;
        users[i] = Student(name, year);
        std::cout<<users[i]<<"\n";
    }
    int task_nr;
    std::cin >> task_nr;
    for(int i = 0; i < task_nr; i++) {
        std::string description;
        int priority;
        int assignee;
        std::cin >> priority >> assignee;
        getline(std::cin, description);
        h.push(Task(users[assignee - 1], priority, description));
    }
    Task &most_important = h.get();
    std::cout << "Most important task: " << most_important << '\n';
    return 0;
}