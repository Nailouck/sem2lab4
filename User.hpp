#pragma once

#include <iostream>
#include <string>


struct User {
    std::string name;
    int age;
    int id;

    User() = default;
    User(std::string name_, int age_, int id_ = 0)
        : name(std::move(name_)), age(age_), id(id_) {
    }

    bool operator==(const User& other) const {
        return name == other.name && age == other.age && id == other.id;
    }

    friend std::ostream& operator<<(std::ostream& os, const User& user) {
        return os << "User(name: " << user.name << ", age: " << user.age << ", id: " << user.id << ")";
    }

    friend std::istream& operator>>(std::istream& is, User& user) {
        std::cout << "Enter name: ";
        is >> user.name;

        std::cout << "Enter age: ";
        is >> user.age;
        while (user.age < 0 || user.age > 150) {
            std::cout << "Age cannot be negative or more than 150. Try again: ";
            is >> user.age;
        }

        std::cout << "Enter id: ";
        is >> user.id;
        while (user.id < 0) {
            std::cout << "ID cannot be negative. Try again: ";
            is >> user.id;
        }

        return is;
    }
};



struct Student : public User {
    std::string group;
    bool exam_pass;

    Student() = default;

    Student(const std::string& name, int age, int id, const std::string& group, bool pass)
        : User(name, age, id), group(group), exam_pass(pass) {
    }

    bool operator==(const Student& other) const {
        return static_cast<const User&>(*this) == static_cast<const User&>(other) &&
            group == other.group && exam_pass == other.exam_pass;
    }

    void Print() const {
        std::cout << "Student " << name << ", age: " << age << ", id: " << id << ", group: " << group << ", Is cleared to exam: " << exam_pass << "\n";
    }

    friend std::ostream& operator<<(std::ostream& os, const Student& student) {
        return os << "Student " << student.name << ", age: " << student.age << ", id: " << student.id << ", group: " << student.group << ", Is cleared to exam: " << student.exam_pass;
    }

    friend std::istream& operator>>(std::istream& is, Student& student) {
        is >> static_cast<User&>(student);

        std::cout << "Enter group: ";
        is >> student.group;

        std::cout << "Enter cleared to exam or not: ";
        is >> student.exam_pass;

        return is;
    }
};



struct Professor : public User {
    std::string subject;
    bool be_on_exam;

    Professor() = default;

    Professor(const std::string& name, int age, int id, const std::string& subject, bool pass) : User(name, age, id), subject(subject), be_on_exam(pass) {
    }

    bool operator==(const Professor& other) const {
        return static_cast<const User&>(*this) == static_cast<const User&>(other) && subject == other.subject && be_on_exam == other.be_on_exam;
    }

    void Print() const {
        std::cout << "Professor " << name << ", age: " << age << ", id: " << id << ", subject: " << subject << ", Be on the exam: " << be_on_exam << "\n";
    }

    friend std::ostream& operator<<(std::ostream& os, const Professor& teacher) {
        return os << "Professor " << teacher.name << ", age: " << teacher.age << ", id: " << teacher.id << ", subject: " << teacher.subject << ", Be on the exam: " << teacher.be_on_exam;
    }

    friend std::istream& operator>>(std::istream& is, Professor& teacher) {
        is >> static_cast<User&>(teacher);

        std::cout << "Enter subject: ";
        is >> teacher.subject;

        std::cout << "Enter Be on the exam or not: ";
        is >> teacher.be_on_exam;

        return is;
    }
};