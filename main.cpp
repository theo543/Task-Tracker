#include <utility>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <numeric>
#include <filesystem>
#include <map>

template <typename Type>
void prompt_user(std::string const& prompt, std::vector<Type> const& options, std::ostream& output = std::cout) {
    output << prompt << "\n";
    for (unsigned int i = 0; i < options.size(); i++) {
        output << i + 1 << ". ";
        output << options[i] << "\n";
    }
}

template<typename Type>
std::pair<int, Type> ask_user(std::string const& question, std::vector<Type> const& options, std::istream& input = std::cin, std::ostream& output = std::cout) {
    while(true) {
        prompt_user(question, options, output);
        unsigned int choice;
        input >> choice;
        if (choice < 1 || choice > options.size()) {
            output << "Invalid choice, try again\n";
            input.clear();
            input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        return std::make_pair(choice, options[choice - 1]);
    }
}

std::filesystem::path ask_path(std::string const& prompt) {
    using namespace std::filesystem;
    while(true) {
        path p = current_path();
        std::cout << "Current path: " << p << std::endl;
        std::cout << prompt << std::endl;
        std::string input;
        std::cin >> input;
        p = p / input;
        p = weakly_canonical(p);
        std::cout << "New path: " << p << std::endl;
        bool choice = ask_user("Is this correct?", std::vector{"Yes", "No"}).first == 1;
        if(choice) {
            return p;
        }
    }
}

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
    [[nodiscard]] Teacher getTeacher() const {
        return teacher;
    }
    Task(const Teacher& teacher_, int priority_, std::string description_) : teacher(teacher_), priority(priority_), description(std::move(description_)) {
        if(priority_ < 0 || priority_ > 4) {
            throw std::invalid_argument("Priority must be between 0 and 4 but was " + std::to_string(priority_));
        }
    }
    std::ostream friend & operator<<(std::ostream &os, const Task& t)  {
        os << "{" << t.getDescription() << ", " << priority_names[t.getPriority()] << ", assigned by " << t.getTeacher() << "}";
        return os;
    }
    static std::vector<std::string> getPriorityNames() {
        return priority_names;
    }
};

class Heap {
    std::vector<Task> elements;
    bool ascending;
    [[nodiscard]] bool check_valid() const {
        for(unsigned int i = 0; i < elements.size(); i++) {
            if(2 * i + 1 < elements.size() && less_than(elements[2 * i + 1], elements[i])) {
                return false;
            }
            if(2 * i + 2 < elements.size() && less_than(elements[2 * i + 2], elements[i])) {
                return false;
            }
        }
        return true;
    }
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
        if(empty())
            throw std::out_of_range("Heap is empty");
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
    [[nodiscard]] const std::vector<Task>& getElements() const {
        return elements;
    }
    std::ostream friend & operator<<(std::ostream &os, const Heap& h)  {
        os<<"Heap of size "<<h.size()<<"\n";
        // print vector
        for (const auto & element : h.getElements())
            os << element << ' ';
        return os;
    }
    explicit Heap(bool asc = false) : ascending(asc) {}
    explicit Heap(std::vector<Task> &&elements_, bool asc = false, bool assert_valid = false) : elements(std::move(elements_)), ascending(asc) {
        bool valid = check_valid();
        if(!valid){
            if(assert_valid)
                throw std::invalid_argument("Heap is not valid but is supposed to be");
            for (unsigned int i = elements.size() / 2; i > 0; i--) {
                down_heapify(i - 1);
            }
            down_heapify(0);
        }
    }
};

class TaskData {
    Heap h;
    std::vector<Teacher> teachers;
public:
    void addTeacher(const Teacher& teacher) {
        teachers.push_back(teacher);
    }
    void addTask(const Task& task) {
        h.push(task);
    }
    [[nodiscard]] const std::vector<Teacher>& getTeachers() const {
        return teachers;
    }
    [[nodiscard]] bool queueEmpty() const {
        return h.empty();
    }
    [[nodiscard]] unsigned int queueSize() const {
        return h.size();
    }

    [[nodiscard]]
    Task popTask() {
        return h.pop();
    }

    friend class InputUtils;
};


class InputUtils {
public:
    static Task read_task(std::istream &input, std::vector<Teacher> const &teachers, std::ostream &output) {
        std::string description;
        int priority;
        Teacher teacher;
        priority = ask_user<std::string>("What is the priority of the task?", Task::getPriorityNames(), input, output).first - 1;
        teacher = ask_user<Teacher>("Which teacher assigned the task?", teachers, input, output).second;
        output << "Enter the description of the task:\n";
        input >> std::ws;
        std::getline(input, description);
        Task t = Task(teacher, priority, description);
        output<<t<<"\n";
        return t;
    }

    static Teacher read_teacher(std::istream &input, std::ostream &output) {
        std::string name, course;
        output << "Enter the name of the teacher:\n";
        input >> std::ws;
        std::getline(input, name);
        output << "What does the teacher teach?\n";
        input >> std::ws;
        std::getline(input, course);
        return {name, course};
    }
    static TaskData readTaskData(std::istream &input, std::ostream &output, bool assertIsHeap = false) {
        output<<"Enter the number of teachers:\n";
        int teacher_nr, task_nr;
        input>>teacher_nr;
        TaskData td = TaskData();
        for(int i = 0; i < teacher_nr; i++) {
            td.addTeacher(read_teacher(input, output));
        }
        output<<"Enter the number of tasks:\n";
        input>>task_nr;
        std::vector<Task> tasks;
        for(int i = 0; i < task_nr; i++) {
            tasks.push_back(InputUtils::read_task(input, td.getTeachers(), output));
        }
        td.h = Heap(std::move(tasks), false, assertIsHeap);
        return td;
    }
    static void writeTaskData(std::ostream &output, const TaskData &td) {
        output<<td.teachers.size()<<"\n";
        std::map<std::string, unsigned int> teacher_map;
        int next_id = 0;
        for(const auto& teacher : td.teachers) {
            output<<teacher.getName()<<"\n";
            output<<teacher.getCourse()<<"\n";
            next_id++;
            teacher_map[teacher.getName()] = next_id;
        }
        output<<td.h.size()<<"\n";
        for(const auto& task : td.h.getElements()) {
            output<<task.getPriority() + 1<<"\n";
            output<<teacher_map[task.getTeacher().getName()]<<"\n";
            output<<task.getDescription()<<"\n";
        }
    }

};

int main(){
    std::cout<<"Welcome to the task tracker!\n";
    bool option = ask_user<std::string>("Load from file or enter data manually?", {"Load from file", "Enter data manually"}).first == 1;
    TaskData td;
    if(option) {
        std::ofstream nullOutput{};
        auto file = std::ifstream(ask_path("Enter the path to load: "));
        td = InputUtils::readTaskData(file, nullOutput, true);
        if(ask_user<std::string>("Would you like to add more teachers?\n", {"Yes", "No"}).first == 1){
            int teacher_nr;
            std::cout << "How many teachers are there?\n";
            std::cin >> teacher_nr;
            for(int i = 0; i < teacher_nr; i++) {
                td.addTeacher(InputUtils::read_teacher(std::cin, std::cout));
            }
        }
        if(ask_user<std::string>("Would you like to add more tasks to the queue?\n", {"Yes", "No"}).first == 1) {
            int task_nr;
            std::cout << "How many tasks are there?\n";
            std::cin >> task_nr;
            for(int i = 0; i < task_nr; i++) {
                td.addTask(InputUtils::read_task(std::cin, td.getTeachers(), std::cout));
            }
        }
    } else {
        td = InputUtils::readTaskData(std::cin, std::cout);
    }
    int query_len;
    std::cout<<"There are "<<td.queueSize()<<" tasks in the queue.\n";
    std::cout<<"How many tasks would you like to work on today? (enter 0 to skip to saving data unchanged)\n";
    std::cin>>query_len;
    std::string modified_prompt;
    if(query_len) {
        TaskData td_copy = td;
        modified_prompt = " (" + std::to_string(query_len) + " tasks have been removed from the list)";
        std::cout << "The " << query_len << " most important tasks are:\n";
        for (int i = 0; i < query_len; i++) {
            if (td.queueEmpty()) {
                std::cout << "ERROR: There are no more tasks in the queue.\n";
                break;
            }
            std::cout << td.popTask() << "\n";
        }
        bool remove = ask_user<std::string>("Would you like to remove these tasks from the queue?" + modified_prompt, {"Yes", "No"}).first == 1;
        if(!remove) {
            td = td_copy;
            modified_prompt = "";
        }
    }
    bool save = ask_user<std::string>("Would you like to save data to file?" + modified_prompt, {"Yes", "No"}).first == 1;
    if (save) {
        auto file = std::ofstream(ask_path("Enter the path to save: "));
        InputUtils::writeTaskData(file, td);
    }
    return 0;
}
