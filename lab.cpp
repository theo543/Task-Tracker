#include <iostream>
#include <utility>

class Facultate{
    std::string nume = "FMI";
public:
    Facultate () {
        std::cout<<"implicit init facultate\n";
    }
    explicit Facultate (std::string nume_) : nume(std::move(nume_)) {
        std::cout<<"init facultate\n";
    }
    Facultate (const Facultate& other) : nume(other.nume) {
        std::cout<<"copiere facultate\n";
    }
    Facultate& operator=(const Facultate& other){
        std::cout<<"operator= facultate\n";
        nume = other.nume;
        return *this;
    }
    ~Facultate(){
        std::cout<<"destructor facultate\n";
    }
    friend std::ostream& operator<<(std::ostream& os, const Facultate& facultate) {
        os << "Facultate: " << facultate.nume;
        return os;
    }
};

class Student{
private:
    std::string nume;
    int grupa = -1;
    Facultate facultate{"FMI Unibuc"};
public:
    Student () {
        std::cout<<"Constructor implicit student\n";
    }
    Student (std::string nume_, int grupa_) : nume(std::move(nume_)), grupa(grupa_) {
        std::cout<<"Constructor student\n";
    }
    Student (const Student &other) : nume(other.nume), grupa(other.grupa), facultate(other.facultate){
        std::cout<<"Constructor copiere student\n";
    }
    std::string getNume() {return nume;}
    [[nodiscard]] int getGrupa() const {return grupa;}
    Facultate getFacultate() {return facultate;}
    void setNume(std::string nume_) {nume = std::move(nume_);}
    void setGrupa(int grupa_){grupa = grupa_;}
    void setFacultate(Facultate f){facultate = f;}
    friend std::ostream& operator<<(std::ostream& os, const Student& student) {
        os << "Student:" << student.nume << " Grupa: " << student.grupa << " " << student.facultate;
        return os;
    }};
void f(Student st){
    Student s = st;
}
Student f2(){
    Student tmp = Student{"1", 2};
    std::cout<<tmp.getGrupa()<<'\n';
    return tmp;
}
int main() {
    Facultate fmi, poli{"Poli"}, ase{"Ase"};
    std::cout<<"1\n";
    Student s("Theo", 151);
    std::cout<<"2\n";
    Student st{"", -1};
    st.setFacultate(poli);
    std::cout<<"3\n";
    f(st);
    std::cout<<fmi<<'\n'<<poli<<'\n'<<ase<<'\n';
    std::cout<<s<<'\n';
    return 0;
}
