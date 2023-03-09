#include <utility>
#include <vector>
#include <string>
#include <iostream>
#include <numeric>

class Teacher {
    std::string name;
    std::string course;
public:
    [[nodiscard]] std::string getName() const {
        return name;
    }
    [[nodiscard]] std::string getCourse() const {
        return course;
    }
    std::ostream friend & operator<<(std::ostream &os, const Teacher& u)  {
        os << "{" << u.getName() <<", teaching: " << u.getCourse() << "}";
        return os;
    }
    Teacher() : name(""), course("") {}
    Teacher(std::string name_, std::string course_) : name(std::move(name_)), course(std::move(course_)) {}
    Teacher(const Teacher& other) : name(other.name), course(other.course){}
    Teacher& operator=(const Teacher& other){
        name = other.name;
        course = other.course;
        return *this;
    }
    ~Teacher() {}
};

class Task {
    Teacher teacher;
    int priority = 0;
    const static inline std::vector<std::string> priority_names = {"optional", "low", "medium", "high", "OVERDUE"};
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
    [[nodiscard]] Teacher getAssignee() const {
        return teacher;
    }
    Task(const Teacher& teacher_, int priority_, std::string description_) : teacher(teacher_), priority(priority_), description(std::move(description_)) {
        if(priority_ < 0 || priority_ > 4) {
            throw std::invalid_argument("Priority must be between 0 and 4 but was " + std::to_string(priority_));
        }
    }
    std::ostream friend & operator<<(std::ostream &os, const Task& t)  {
        os << "{" << t.getDescription() << ", " << priority_names[t.getPriority()] << ", assigned by " << t.getAssignee() << "}";
        return os;
    }
    static std::vector<std::string> getPriorityNames() {
        return priority_names;
    }
};

class Heap {
    std::vector<Task> elements;
    bool ascending;
    [[nodiscard]] bool less_than(const Task& lhs, const Task& rhs) const {
        if(ascending) {
            return lhs < rhs;
        } else {
            return rhs < lhs;
        }
    }
    void down_heapify(unsigned int node) {
        unsigned int i = node;
        while(2 * i + 1 < elements.size()) {
            unsigned int target = i;
            if(less_than(elements[2 * i + 1], elements[target])) {
                target = 2 * i + 1;
            }
            if(2 * i + 2 < elements.size() && less_than(elements[2 * i + 2], elements[target])) {
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
        while(i != 0 && less_than(elements[i], elements[(i - 1) / 2])) {
            std::swap(elements[i], elements[(i - 1) / 2]);
            i = (i - 1) / 2;
        }
    }
public:
    void push(const Task& task) {
        elements.push_back(task);
        up_heapify(elements.size() - 1);
    }
    [[nodiscard]] Task get() const {
        return elements[0];
    }
    Task pop() {
        if(empty()) {
            throw std::out_of_range("Heap is empty");
        }
        auto task = get();
        elements[0] = elements[elements.size() - 1];
        elements.pop_back();
        down_heapify(0);
        return task;
    }
    [[nodiscard]] bool empty() const {
        return elements.empty();
    }
    [[nodiscard]] unsigned int size() const {
        return elements.size();
    }
    [[nodiscard]] std::vector<Task> toArray() const {
        return elements;
    }
    std::ostream friend & operator<<(std::ostream &os, const Heap& h)  {
        os<<"Heap of size "<<h.size()<<"\n";
        // print vector
        for (const auto & element : h.toArray())
            os << element << ' ';
        return os;
    }
    explicit Heap(bool asc) : ascending(asc) {}
    Heap() : ascending(true) {}
};

template<typename Type>
std::pair<int, Type> ask_user(std::string const& question, std::vector<Type> const& options, bool number = true) {
    while(true) {
        std::cout << question << "\n";
        for (unsigned int i = 0; i < options.size(); i++) {
            if (number) {
                std::cout << i + 1 << ". ";
            }
            std::cout << options[i] << "\n";
        }
        unsigned int choice;
        std::cin >> choice;
        if (choice < 1 || choice > options.size()) {
            std::cout << "Invalid choice, try again\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        return std::make_pair(choice, options[choice - 1]);
    }
}

int main(){
    Heap h;
    std::vector<Teacher> teachers;
    int teacher_nr;
    std::cout << "How many teachers are there?\n";
    std::cin >> teacher_nr;
    teachers.resize(teacher_nr);
    for(int i = 0; i < teacher_nr; i++) {
        std::cout << "Enter the name of the teacher:\n";
        std::string name;
        std::cin >> std::ws;
        std::getline(std::cin, name);
        std::cout << "What does the teacher teach?\n";
        std::string course;
        std::cin >> std::ws;
        std::getline(std::cin, course);
        teachers[i] = Teacher(name, course);
        std::cout << teachers[i] << "\n";
    }
    int task_nr;
    std::cout << "How many tasks are there?\n";
    std::cin >> task_nr;
    for(int i = 0; i < task_nr; i++) {
        std::string description;
        int priority = ask_user<std::string>("What is the priority of the task?", Task::getPriorityNames()).first - 1;
        Teacher teacher = ask_user<Teacher>("Which teacher assigned the task?", teachers).second;
        std::cout << "Enter the description of the task:\n";
        std::cin >> std::ws;
        std::getline(std::cin, description);
        Task t = Task(teacher, priority, description);
        std::cout<<t<<"\n";
        h.push(t);
    }
    int query_len;
    std::cout<<"How many tasks would you like to work on today?\n";
    std::cin>>query_len;
    std::cout<<"The " << query_len << " most important tasks are:\n";
    for(int i = 0; i < query_len; i++) {
        try {
            std::cout << h.pop() << "\n";
        } catch (std::out_of_range& e) {
            std::cout << "ERROR: No more tasks to show\n";
            break;
        }
    }
    return 0;
}
