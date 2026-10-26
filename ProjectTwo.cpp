//============================================================================
// Name        : ProjectTwo.cpp
// Author      : Veronica Chambers
// Course      : CS 300 - Data Structures and Algorithms
// Description : ABCU Advising Assistance Program
//               Loads course data into a binary search tree, prints the
//               courses in alphanumeric order, and displays an individual
//               course with its prerequisites.
//============================================================================

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

using namespace std;

// Stores all information for one ABCU course.
struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;
};

// Removes leading and trailing whitespace from a string.
string trim(const string& value) {
    size_t start = value.find_first_not_of(" \t\r\n");
    if (start == string::npos) {
        return "";
    }

    size_t end = value.find_last_not_of(" \t\r\n");
    return value.substr(start, end - start + 1);
}

// Converts course numbers to uppercase so searches are case-insensitive.
string toUpper(string value) {
    transform(value.begin(), value.end(), value.begin(),
        [](unsigned char ch) { return static_cast<char>(toupper(ch)); });
    return value;
}

// Splits one comma-separated line into trimmed fields.
vector<string> splitCSVLine(const string& line) {
    vector<string> fields;
    string field;
    stringstream input(line);

    while (getline(input, field, ',')) {
        fields.push_back(trim(field));
    }

    return fields;
}

// Binary search tree used to store Course objects by course number.
class BinarySearchTree {
private:
    struct Node {
        Course course;
        Node* left;
        Node* right;

        explicit Node(const Course& aCourse)
            : course(aCourse), left(nullptr), right(nullptr) {
        }
    };

    Node* root;

    // Recursively releases all nodes in a subtree.
    void clear(Node* node) {
        if (node == nullptr) {
            return;
        }

        clear(node->left);
        clear(node->right);
        delete node;
    }

    // Recursively inserts a course by alphanumeric course number.
    Node* insert(Node* node, const Course& course) {
        if (node == nullptr) {
            return new Node(course);
        }

        if (course.courseNumber < node->course.courseNumber) {
            node->left = insert(node->left, course);
        }
        else if (course.courseNumber > node->course.courseNumber) {
            node->right = insert(node->right, course);
        }
        else {
            // Replace duplicate course data rather than creating duplicate keys.
            node->course = course;
        }

        return node;
    }

    // Recursively prints the BST in sorted (inorder) sequence.
    void printInOrder(Node* node) const {
        if (node == nullptr) {
            return;
        }

        printInOrder(node->left);
        cout << node->course.courseNumber << ", " << node->course.courseTitle << endl;
        printInOrder(node->right);
    }

public:
    BinarySearchTree() : root(nullptr) {}

    ~BinarySearchTree() {
        clear(root);
        root = nullptr;
    }

    // Removes all courses currently stored in the tree.
    void clear() {
        clear(root);
        root = nullptr;
    }

    bool empty() const {
        return root == nullptr;
    }

    void insert(const Course& course) {
        root = insert(root, course);
    }

    // Iteratively searches for a course by course number.
    const Course* search(const string& courseNumber) const {
        string key = toUpper(trim(courseNumber));
        Node* current = root;

        while (current != nullptr) {
            if (key == current->course.courseNumber) {
                return &current->course;
            }

            if (key < current->course.courseNumber) {
                current = current->left;
            }
            else {
                current = current->right;
            }
        }

        return nullptr;
    }

    void printCourseList() const {
        printInOrder(root);
    }
};

// Reads and validates the ABCU input file before loading courses into the BST.
bool loadCourses(const string& fileName, BinarySearchTree& courseTree) {
    ifstream inputFile(fileName);

    if (!inputFile.is_open()) {
        cerr << "Error: Unable to open file '" << fileName << "'." << endl;
        return false;
    }

    vector<Course> courses;
    unordered_set<string> courseNumbers;
    string line;
    int lineNumber = 0;

    // First pass: parse every course record and collect all course numbers.
    while (getline(inputFile, line)) {
        ++lineNumber;
        line = trim(line);

        if (line.empty()) {
            continue;
        }

        vector<string> fields = splitCSVLine(line);

        if (fields.size() < 2) {
            cerr << "Error: Line " << lineNumber
                << " must contain at least a course number and course title." << endl;
            return false;
        }

        Course course;
        course.courseNumber = toUpper(fields[0]);
        course.courseTitle = fields[1];

        if (course.courseNumber.empty() || course.courseTitle.empty()) {
            cerr << "Error: Line " << lineNumber
                << " contains an empty course number or course title." << endl;
            return false;
        }

        // Remove a UTF-8 BOM if one exists at the beginning of the first field.
        if (lineNumber == 1 && course.courseNumber.size() >= 3 &&
            static_cast<unsigned char>(course.courseNumber[0]) == 0xEF &&
            static_cast<unsigned char>(course.courseNumber[1]) == 0xBB &&
            static_cast<unsigned char>(course.courseNumber[2]) == 0xBF) {
            course.courseNumber = course.courseNumber.substr(3);
        }

        for (size_t i = 2; i < fields.size(); ++i) {
            string prerequisite = toUpper(fields[i]);
            if (!prerequisite.empty()) {
                course.prerequisites.push_back(prerequisite);
            }
        }

        if (!courseNumbers.insert(course.courseNumber).second) {
            cerr << "Error: Duplicate course number '" << course.courseNumber
                << "' found on line " << lineNumber << "." << endl;
            return false;
        }

        courses.push_back(course);
    }

    if (courses.empty()) {
        cerr << "Error: The file does not contain any course records." << endl;
        return false;
    }

    // Second pass: verify every prerequisite exists as another course in the file.
    for (const Course& course : courses) {
        for (const string& prerequisite : course.prerequisites) {
            if (courseNumbers.find(prerequisite) == courseNumbers.end()) {
                cerr << "Error: Course " << course.courseNumber
                    << " lists prerequisite " << prerequisite
                    << ", but that course does not exist in the file." << endl;
                return false;
            }
        }
    }

    // Validation succeeded, so replace the current tree with the new data.
    courseTree.clear();
    for (const Course& course : courses) {
        courseTree.insert(course);
    }

    cout << courses.size() << " courses loaded successfully." << endl;
    return true;
}

// Displays one course and the course number/title of each prerequisite.
void printCourseInformation(const BinarySearchTree& courseTree, const string& courseNumber) {
    const Course* course = courseTree.search(courseNumber);

    if (course == nullptr) {
        cout << "Course " << toUpper(trim(courseNumber)) << " not found." << endl;
        return;
    }

    cout << course->courseNumber << ", " << course->courseTitle << endl;

    if (course->prerequisites.empty()) {
        cout << "Prerequisites: None" << endl;
        return;
    }

    cout << "Prerequisites: ";
    for (size_t i = 0; i < course->prerequisites.size(); ++i) {
        const string& prerequisiteNumber = course->prerequisites[i];
        const Course* prerequisiteCourse = courseTree.search(prerequisiteNumber);

        if (i > 0) {
            cout << "; ";
        }

        cout << prerequisiteNumber;
        if (prerequisiteCourse != nullptr) {
            cout << ", " << prerequisiteCourse->courseTitle;
        }
    }
    cout << endl;
}

void displayMenu() {
    cout << endl;
    cout << "1. Load Data Structure." << endl;
    cout << "2. Print Course List." << endl;
    cout << "3. Print Course." << endl;
    cout << "9. Exit" << endl;
    cout << "What would you like to do? ";
}

int main() {
    BinarySearchTree courseTree;
    bool dataLoaded = false;
    string input;

    cout << "Welcome to the course planner." << endl;

    while (true) {
        displayMenu();
        getline(cin, input);

        int choice = 0;
        try {
            size_t processed = 0;
            choice = stoi(trim(input), &processed);
            if (processed != trim(input).size()) {
                throw invalid_argument("extra characters");
            }
        }
        catch (...) {
            cout << input << " is not a valid option." << endl;
            continue;
        }

        if (choice == 1) {
            cout << "Enter the file name containing the course data: ";
            string fileName;
            getline(cin, fileName);
            fileName = trim(fileName);

            if (fileName.empty()) {
                cout << "Error: A file name is required." << endl;
                dataLoaded = false;
                continue;
            }

            dataLoaded = loadCourses(fileName, courseTree);
        }
        else if (choice == 2) {
            if (!dataLoaded || courseTree.empty()) {
                cout << "Please load the course data first using option 1." << endl;
                continue;
            }

            cout << "Here is a sample schedule:" << endl;
            courseTree.printCourseList();
        }
        else if (choice == 3) {
            if (!dataLoaded || courseTree.empty()) {
                cout << "Please load the course data first using option 1." << endl;
                continue;
            }

            cout << "What course do you want to know about? ";
            string courseNumber;
            getline(cin, courseNumber);
            printCourseInformation(courseTree, courseNumber);
        }
        else if (choice == 9) {
            cout << "Thank you for using the course planner!" << endl;
            break;
        }
        else {
            cout << choice << " is not a valid option." << endl;
        }
    }

    return 0;
}
