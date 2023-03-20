#include <utility>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <numeric>
#include <filesystem>
#include <map>

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

std::filesystem::path ask_path(std::string const& prompt, std::istream& input = std::cin, std::ostream& output = std::cout) {
    using namespace std::filesystem;
    while(true) {
        path p = current_path();
        output << "Current path: " << p << std::endl;
        output << prompt << std::endl;
        std::string path_addition;
        input >> path_addition;
        p = p / path_addition;
        p = weakly_canonical(p);
        output << "New path: " << p << std::endl;
        bool choice = ask_user("Is this correct?", std::vector{"Yes", "No"}, input, output).first == 1;
        if(choice) {
            return p;
        }
    }
}

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

class TerminalOption {
    std::string name;
    typedef std::function<void(std::istream&, std::ostream&, TaskData&)> TerminalOptionFunction;
    TerminalOptionFunction function;
public:
    [[nodiscard]] bool isCallable() const {
        return function != nullptr;
    }
    void operator()(std::istream &input, std::ostream &output, TaskData &td) {
        if(!isCallable())
            throw std::runtime_error("Cannot call option \"" + name + "\" because it is not callable");
        function(input, output, td);
    }
    friend std::ostream &operator<<(std::ostream &os, const TerminalOption &option) {
        os << option.name;
        return os;
    }
    TerminalOption(std::string name, TerminalOptionFunction function) : name(std::move(name)), function(std::move(function)) {}
};

class TerminalController {
    std::istream &input;
    std::ostream &output;
    TaskData td{};
    std::vector<TerminalOption> options;
public:
    TerminalController(std::istream &input, std::ostream &output) : input(input), output(output) {}
    void addOption(const TerminalOption& new_option) {
        options.push_back(new_option);
    }
    void addOption(const std::string& name, const std::function<void(std::istream&, std::ostream&, TaskData&)>& function) {
        addOption(TerminalOption(name, function));
    }
    // returns false when an un-callable option was selected
    bool readExecuteCommand() {
        auto option = ask_user<TerminalOption>("What would you like to do?", options, input, output);
        if(!option.second.isCallable())
            return false;
        option.second(input, output, td);
        return true;
    }
};

int main(){
    std::cout<<"Welcome to the task tracker!\n";
    TerminalController term{std::cin, std::cout};
    term.addOption("Add a teacher.", [](auto &in, auto &out, auto &td) {
        td.addTeacher(InputUtils::read_teacher(in, out));
    });
    term.addOption("Add a task.", [](auto &in, auto &out, auto &td) {
        td.addTask(InputUtils::read_task(in, td.getTeachers(), out));
    });
    term.addOption("Work on some tasks.", [](auto &in, auto &out, auto &td) {
        out<<"There are "<<td.queueSize()<<" tasks in the queue.\n";
        out<<"How many tasks would you like to work on today?\n";
        int query_len;
        in>>query_len;
        TaskData td_copy = td;
        while(query_len--) {
            if(td_copy.queueEmpty()) {
                out<<"No more tasks remaining.\n";
                break;
            }
            out<<td_copy.popTask()<<"\n";
        }
        bool remove = ask_user<std::string>("Would you like to remove these tasks from the queue?", {"No", "Yes"}, in, out).first == 2;
        if(remove) {
            td = td_copy;
        }
    });
    term.addOption("Load from file.", [](auto &in, auto &out, auto &td) {
        auto file = std::ifstream(ask_path("Enter the path to load: ", in, out));
        auto nullStream = std::ofstream{};
        if(!file)
            out<<"Failed to open file for reading.\n";
        else
            td = InputUtils::readTaskData(file, nullStream, true);
    });
    term.addOption("Save to file.", [](auto &in, auto &out, auto &td) {
        auto file = std::ofstream(ask_path("Enter the path to save: ", in, out));
        if(!file)
            out<<"Failed to open file for writing.\n";
        else
            InputUtils::writeTaskData(file, td);
    });
    term.addOption("Exit.", nullptr);
    bool exitChosen = false;
    while (!exitChosen) {
        exitChosen = !term.readExecuteCommand();
    }
    std::cout<<"Goodbye!"<<std::endl;
    return 0;
}
