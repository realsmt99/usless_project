#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "malloc.h"
#include <stddef.h>
#include <ctype.h>
#include <math.h>
typedef struct
{
    int student_id;
    char name[50];
    int age;
    char major[50];
} Student;

// Course structure
typedef struct
{
    int course_id;
    char course_name[100];
    char instructor[50];
} Course;

typedef struct StudentTree
{
    Student student;
    struct StudentTree *right, *left;
} StudentTree;

typedef StudentTree *arbreS;

typedef struct CourseTree
{
    Course course;
    struct CourseTree *right, *left;
} CourseTree;

typedef CourseTree *arbreC;

typedef struct
{
    int student_id;
    int course_id;
} Enrollement;

typedef struct TreeEnrollment
{
    Enrollement value;
    struct TreeEnrollment *right, *left;
} TreeEnrollment;

typedef TreeEnrollment *arbreE;

void print_tree(arbreE root)
{
    if (root == NULL)
    {
        return;
    }
    print_tree(root->left);
    printf("Student ID: %d, Course ID: %d\n", root->value.student_id, root->value.course_id);
    print_tree(root->right);
}

arbreC insertCourse(arbreC root, Course course)
{
    if (root == NULL)
    {
        arbreC newNode = (arbreC)malloc(sizeof(CourseTree));
        newNode->course = course;
        newNode->left = newNode->right = NULL;
        return newNode;
    }

    if (course.instructor < root->course.instructor)
    {
        root->left = insertCourse(root->left, course);
    }
    else if (course.instructor > root->course.instructor)
    {
        root->right = insertCourse(root->right, course);
    }
    else if (course.instructor == root->course.instructor)
    {
        if (course.course_id < root->course.course_id)
        {
            root->left = insertCourse(root->left, course);
        }
        else if (course.course_id > root->course.course_id)
        {
            root->right = insertCourse(root->right, course);
        }
    }

    return root;
}
void remove_leading_spaces(char str[])
{
    int i = 0, j = 0;
    while (str[i] == ' ')
        i++; // Skip leading spaces
    while (str[i])
    {
        str[j++] = str[i++]; // Shift non-space characters to the left
    }
    str[j] = '\0'; // Null-terminate the string
}

arbreS insert_student(arbreS root, Student e)
{
    if (root == NULL)
    {
        root = (arbreS)malloc(sizeof(StudentTree));
        root->student = e;
        root->left = NULL;
        root->right = NULL;
    }
    else if (e.name < root->student.name)
    {
        root->left = insert_student(root->left, e);
    }
    else if (e.name > root->student.name)
    {
        root->right = insert_student(root->right, e);
    }
    else if (e.name == root->student.name)
    {
        if (e.student_id < root->student.student_id)
        {
            root->left = insert_student(root->left, e);
        }
        else if (e.student_id > root->student.student_id)
        {
            root->right = insert_student(root->right, e);
        }
    }
    return root;
}

arbreE insert_enrollment(arbreE root, Enrollement e)
{
    if (root == NULL)
    {
        root = (arbreE)malloc(sizeof(TreeEnrollment));
        root->value = e;
        root->left = NULL;
        root->right = NULL;
    }
    else if (e.course_id < root->value.course_id)
    {
        root->left = insert_enrollment(root->left, e);
    }
    else if (e.course_id > root->value.course_id)
    {
        root->right = insert_enrollment(root->right, e);
    }
    else if (e.course_id == root->value.course_id)
    {
        if (e.student_id < root->value.student_id)
        {
            root->left = insert_enrollment(root->left, e);
        }
        else if (e.student_id > root->value.student_id)
        {
            root->right = insert_enrollment(root->right, e);
        }
    }
    return root;
}
// PARSING FUNCTIONS **********************************************************************************************************************

void parse_students_csv(arbreS *studentTree)
{
    FILE *file = fopen("students.csv", "r");
    if (file == NULL)
    {
        perror("Error opening file");
    }

    char line[256];
    Student student;
    while (fgets(line, sizeof(line), file) != NULL)
    {

        sscanf(line, "%d,%49[^,],%d,%49[^,\n]", &student.student_id, student.name, &student.age, student.major);
        printf("%d,%s,%d,%s\n", student.student_id, student.name, student.age, student.major);
        *studentTree = insert_student(*studentTree, student);
    }

    fclose(file);
}

void parse_courses_csv(arbreC *courseTree)
{
    FILE *file = fopen("courses.csv", "r");
    if (file == NULL)
    {
        perror("Error opening file");
    }

    char line[256];
    Course course;

    while (fgets(line, sizeof(line), file) != NULL)
    {

        sscanf(line, "%d,%49[^,],%49[^,\n]", &course.course_id, course.course_name, course.instructor);

        printf("%d,%s,%s\n", course.course_id, course.course_name, course.instructor);
        *courseTree = insertCourse(*courseTree, course);
    }

    fclose(file);
}

void parse_follow_course_csv(arbreE *root)
{
    FILE *file = fopen("follow_course_output.csv", "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return;
    }
    char line[256];
    Enrollement enrollment;
    while (fgets(line, sizeof(line), file))
    {

        sscanf(line, "%d,%d", &enrollment.student_id, &enrollment.course_id);
        printf("%d,%d\n", enrollment.student_id, enrollment.course_id);
        *root = insert_enrollment(*root, enrollment);
    }
    fclose(file);
}

void serialize_students_csv_helper(FILE *file, arbreS node)
{
    if (node == NULL)
    {
        return;
    }
    remove_leading_spaces(node->student.name);
    remove_leading_spaces(node->student.major);

    fprintf(file, "%d, %s, %d, %s\n", node->student.student_id, node->student.name, node->student.age, node->student.major);

    serialize_students_csv_helper(file, node->left);

    serialize_students_csv_helper(file, node->right);
}

void serialize_students_csv(arbreS studentTree)
{
    FILE *file = fopen("students.csv", "w");

    serialize_students_csv_helper(file, studentTree);

    fclose(file);
}
void serialize_courses_csv_helper(FILE *file, arbreC node)
{
    if (node == NULL)
    {
        return;
    }
    remove_leading_spaces(node->course.course_name);
    remove_leading_spaces(node->course.instructor);
    fprintf(file, "%d, %s, %s\n", node->course.course_id, node->course.course_name, node->course.instructor);
    serialize_courses_csv_helper(file, node->left);

    serialize_courses_csv_helper(file, node->right);
}

void serialize_courses_csv(arbreC courseTree)
{
    FILE *file = fopen("courses.csv", "w");

    serialize_courses_csv_helper(file, courseTree);

    fclose(file);
}

void serialize_follow_course(FILE *file, arbreE root)
{
    if (root == NULL)
    {
        return;
    }

    fprintf(file, "%d,%d\n", root->value.student_id, root->value.course_id);

    serialize_follow_course(file, (arbreE)root->left);
    serialize_follow_course(file, (arbreE)root->right);
}
void serialize_follow_course_csv(arbreE root)
{
    FILE *file = fopen("follow_course_output.csv", "w");

    serialize_follow_course(file, root);
    fclose(file);
}

void remove_enrollment(arbreE *root)
{
    if (*root == NULL)
    {
        return;
    }
    remove_enrollment(&(*root)->left);
    remove_enrollment(&(*root)->right);
    free(*root);
    *root = NULL;
}

void remove_Students(arbreS *root)
{
    if (*root == NULL)
    {
        return;
    }
    remove_Students(&(*root)->left);
    remove_Students(&(*root)->right);
    free(*root);
    *root = NULL;
}
void remove_courses(arbreC *root)
{
    if (*root == NULL)
    {
        return;
    }
    remove_courses(&(*root)->left);
    remove_courses(&(*root)->right);
    free(*root);
    *root = NULL;
}

void closeData(arbreS *studentTree, arbreC *courseTree, arbreE *enrollmentTree)
{
    remove_enrollment(enrollmentTree);
    remove_Students(studentTree);
    remove_courses(courseTree);
}
void loadData(arbreS *studentTree, arbreC *courseTree, arbreE *enrollmentTree)
{
    parse_students_csv(studentTree);
    parse_courses_csv(courseTree);
    parse_follow_course_csv(enrollmentTree);
}

void saveData(arbreS studentTree, arbreC courseTree, arbreE enrollmentTree)
{
    serialize_students_csv(studentTree);
    serialize_courses_csv(courseTree);
    serialize_follow_course_csv(enrollmentTree);
}

void insert_oneStudent(arbreS *studentTree, Student student)
{

    *studentTree = insert_student(*studentTree, student);
}

int getMaxStudentId(arbreS root)
{
    if (root == NULL)
    {
        return -1;
    }

    // Get the max student ID in the left subtree
    int maxLeft = getMaxStudentId(root->left);

    // Get the max student ID in the right subtree
    int maxRight = getMaxStudentId(root->right);

    // Return the maximum of the current node's student ID, maxLeft, and maxRight
    return fmax(root->student.student_id, fmax(maxLeft, maxRight));
}

int getMaxCourseId(arbreC root)
{

    if (root == NULL)
    {
        return -1;
    }

    // Get the max student ID in the left subtree
    int maxLeft = getMaxCourseId(root->left);

    // Get the max student ID in the right subtree
    int maxRight = getMaxCourseId(root->right);

    // Return the maximum of the current node's student ID, maxLeft, and maxRight
    return fmax(root->course.course_id, fmax(maxLeft, maxRight));
}

int get_course_id(arbreC root, char coursename[])
{

    if (root == NULL)
    {
        return -1;
    }

    int i = 0;
    int j = 0;
    while (root->course.course_name[i] != '\0' && coursename[j] != '\0')
    {
        if (root->course.course_name[i] != ' ' && isalpha(coursename[j]))
        {
            if (tolower(root->course.course_name[i]) != tolower(coursename[j]))
            {
                break;
            }
            j++;
        }
        i++;
    }

    if (coursename[j] == '\0')
    {
        return root->course.course_id;
    }

    int left = get_course_id(root->left, coursename);
    int right = get_course_id(root->right, coursename);

    if (left != -1)
    {
        return left;
    }
    else
    {
        return right;
    }
}

void displayStudent(Student student)
{
    printf("ID: %d, Name: %s, Age: %d, Major: %s\n", student.student_id, student.name, student.age, student.major);
}

// Function to select students based on criteria
void selectStudent(arbreS root, char criteria[], char op[], char value[])
{
    if (root == NULL)
    {
        return;
    }

    selectStudent(root->left, criteria, op, value);

    if ((strcasecmp(criteria, "name") == 0 && strcmp(op, "=") == 0 && strcasecmp(root->student.name, value) == 0) ||
        (strcmp(criteria, "age") == 0 && ((strcmp(op, ">") == 0 && root->student.age > atoi(value)) ||
                                          (strcmp(op, "<") == 0 && root->student.age < atoi(value)) ||
                                          (strcmp(op, "=") == 0 && root->student.age == atoi(value)))) ||
        (strcmp(criteria, "ID") == 0 && ((strcmp(op, ">") == 0 && root->student.student_id > atoi(value)) ||
                                         (strcmp(op, "<") == 0 && root->student.student_id < atoi(value)) ||
                                         (strcmp(op, "=") == 0 && root->student.student_id == atoi(value)))) ||
        (strcmp(criteria, "major") == 0 && strcmp(op, "=") == 0 && strcmp(root->student.major, value) == 0))
    {
        // Display the matching student
        displayStudent(root->student);
    }

    selectStudent(root->right, criteria, op, value);
}
void displayCourse(Course course)
{
    printf(" Course ID: %d, Course Name: %s, Instructor: %s\n",
           course.course_id, course.course_name, course.instructor);
}

void selectCourse(arbreC root, char criteria[], char op[], char value[])
{
    if (root == NULL)
    {
        return;
    }

    selectCourse(root->left, criteria, op, value);

    // Modify the conditions based on the course structure
    if ((strcmp(criteria, "ID") == 0 && ((strcmp(op, ">") == 0 && root->course.course_id > atoi(value)) ||
                                         (strcmp(op, "<") == 0 && root->course.course_id < atoi(value)) ||
                                         (strcmp(op, "=") == 0 && root->course.course_id == atoi(value)))) ||

        (strcmp(criteria, "instructor") == 0 && strcmp(op, "=") == 0 && strcmp(root->course.instructor, value) == 0) ||
        (strcmp(criteria, "name") == 0 && strcmp(op, "=") == 0 && strcmp(root->course.course_name, value) == 0))
    {
        // Display the matching course
        displayCourse(root->course);
    }

    selectCourse(root->right, criteria, op, value);
}
void select(arbreC rootC, arbreS rootS, char type[], char criteria[], char op[], char value[])
{

    if ((strcmp(type, "student") == 0))
    {
        printf("Selected students:\n");
        selectStudent(rootS, criteria, op, value);
    }
    else if (strcmp(type, "course") == 0)
    {

        // Select and display courses based on the criteria
        printf("Selected courses:\n");
        selectCourse(rootC, criteria, op, value);
    }
}
void delete_student_from_enrollment(arbreE *rootE, int studentId)
{
    if (*rootE == NULL)
    {
        return;
    }

    // Recursively search for the student in the left and right subtrees
    delete_student_from_enrollment(&(*rootE)->left, studentId);
    delete_student_from_enrollment(&(*rootE)->right, studentId);

    // If the current node's student ID matches the given ID, delete the node
    if ((*rootE)->value.student_id == studentId)
    {
        arbreE temp = *rootE;

        if ((*rootE)->left == NULL)
        {
            *rootE = (*rootE)->right;
        }
        else if ((*rootE)->right == NULL)
        {
            *rootE = (*rootE)->left;
        }
        else
        {
            arbreE minRight = (*rootE)->right;
            while (minRight->left != NULL)
            {
                minRight = minRight->left;
            }
            (*rootE)->value = minRight->value;
            delete_student_from_enrollment(&(*rootE)->right, studentId);
        }

        free(temp);
    }
}

void delete_course_from_enrollment(arbreE *rootE, int courseId)
{
    if (*rootE == NULL)
    {
        return;
    }

    // Recursively search for the course in the left and right subtrees
    delete_course_from_enrollment(&(*rootE)->left, courseId);
    delete_course_from_enrollment(&(*rootE)->right, courseId);

    // If the current node's course ID matches the given ID, delete the node
    if ((*rootE)->value.course_id == courseId)
    {
        arbreE temp = *rootE;

        if ((*rootE)->left == NULL)
        {
            *rootE = (*rootE)->right;
        }
        else if ((*rootE)->right == NULL)
        {
            *rootE = (*rootE)->left;
        }
        else
        {
            arbreE minRight = (*rootE)->right;
            while (minRight->left != NULL)
            {
                minRight = minRight->left;
            }
            (*rootE)->value = minRight->value;
            delete_course_from_enrollment(&(*rootE)->right, courseId);
        }

        free(temp);
    }
}

void delete_student(arbreS *rootS, arbreE *rootE, char criteria[], char op[], char value[])
{
    if (*rootS == NULL)
    {
        return;
    }

    delete_student(&(*rootS)->left, rootE, criteria, op, value);
    delete_student(&(*rootS)->right, rootE, criteria, op, value);

    int comparison_result;

    if (strcmp(criteria, "id") == 0)
    {
        comparison_result = (*rootS)->student.student_id - *((int *)value);
    }
    else if (strcmp(criteria, "name") == 0)
    {
        comparison_result = strcmp((*rootS)->student.name, (char *)value);
    }
    else if (strcmp(criteria, "age") == 0)
    {
        comparison_result = (*rootS)->student.age - *((int *)value);
    }
    else
    {
        // Invalid criteria type
        return;
    }

    if (((strcmp(op, "=") == 0 && comparison_result == 0) ||
         (strcmp(op, "<") == 0 && comparison_result < 0) ||
         (strcmp(op, ">") == 0 && comparison_result > 0)))
    {
        // Node found with matching criteria, perform deletion
        arbreS temp = *rootS;

        // Delete the student from the enrollment tree
        delete_student_from_enrollment(rootE, (*rootS)->student.student_id);

        if ((*rootS)->left == NULL)
        {
            *rootS = (*rootS)->right;
        }
        else if ((*rootS)->right == NULL)
        {
            *rootS = (*rootS)->left;
        }
        else
        {
            arbreS minRight = (*rootS)->right;
            while (minRight->left != NULL)
            {
                minRight = minRight->left;
            }
            (*rootS)->student = minRight->student;
            delete_student(&(*rootS)->right, rootE, criteria, op, value);
        }
        free(temp);
    }
}
void delete_course(arbreC *rootc, arbreE *TreeEnrollment, char *criteria, char op[], void *value)
{
    if (*rootc == NULL)
    {
        return;
    }

    delete_course(&(*rootc)->left, TreeEnrollment, criteria, op, value);
    delete_course(&(*rootc)->right, TreeEnrollment, criteria, op, value);

    int comparison_result;

    if (strcmp(criteria, "id") == 0)
    {
        comparison_result = (*rootc)->course.course_id - *((int *)value);
    }
    else if (strcmp(criteria, "name") == 0)
    {
        comparison_result = strcmp((*rootc)->course.course_name, (char *)value);
    }
    else if (strcmp(criteria, "instructor") == 0)
    {
        comparison_result = strcmp((*rootc)->course.instructor, (char *)value);
    }
    else
    {
        // Invalid criteria type
        return;
    }

    if (((strcmp(op, "=") == 0 && comparison_result == 0) ||
         (strcmp(op, "<") == 0 && comparison_result < 0) ||
         (strcmp(op, ">") == 0 && comparison_result > 0)))
    {
        // Node found with matching criteria, perform deletion
        arbreC tempCourse = *rootc;
        delete_course_from_enrollment(TreeEnrollment, (*rootc)->course.course_id);
        // Delete corresponding enrollment records
        if (tempCourse->left == NULL)
        {
            *rootc = tempCourse->right;
        }
        else if (tempCourse->right == NULL)
        {
            *rootc = tempCourse->left;
        }
        else
        {
            arbreC minRight = tempCourse->right;
            while (minRight->left != NULL)
            {
                minRight = minRight->left;
            }
            tempCourse->course = minRight->course;
            delete_course(&tempCourse->right, TreeEnrollment, criteria, op, value);
        }
        free(tempCourse);
    }
}

int main()
{

    printf("Hello and welcome to our database\n");
    printf("Please choose one of the following options:\n");
    printf("1- Load \n");
    printf("2- Save\n");
    printf("3- Close\n");

    printf("Skip\n");
    printf("Enter your command: ");

    char choice[10];
    scanf("%[^\n]", choice);
    arbreC courseTree = NULL;
    arbreE enrollmentTree = NULL;
    arbreS studentTree = NULL;

    if (strcmp(choice, "load") == 0)
    {
        printf("Loading data...\n");
        loadData(&studentTree, &courseTree, &enrollmentTree);
        printf("Data loaded successfully\n");
    }
    else if (strcmp(choice, "save") == 0)
    {
        printf("Saving data...\n");
        saveData(studentTree, courseTree, enrollmentTree);
        printf("Data saved successfully\n");
    }
    else if (strcmp(choice, "delete") == 0)
    {
        printf("Closing data...\n");
        closeData(&studentTree, &courseTree, &enrollmentTree);
        printf("Data closed successfully\n");
    }
    else
    {
        printf("Invalid choice\n");
    }

    // char command[100];
    // printf("Enter command: ");
    // getchar(); // Remove the newline character from the buffer
    // fgets(command, sizeof(command), stdin);
    // command[strcspn(command, "\n")] = 0; // Remove the newline character

    // // Parse the command into separate variables
    // char ComandType[10], type[20], criteria[20], op[5], value[50];

    // if (sscanf(command, "%s %s %s %s %[^\n]", ComandType, type, criteria, op, value) != 5)
    // {
    //     fprintf(stderr, "Invalid command format\n");
    //     return 1; // Return an error code
    // }
    // if ((strcmp(ComandType, "select") == 0))
    // {
    //     select(courseTree, studentTree, type, criteria, op, value);
    // }

    printf("you can now insert, select or delete\n ");
    printf("________________________insert__________________________________ \n");
    printf("insert student <name> <age> <major> \n insert course <course_name> <instructor>\n ");
    printf("________________________select__________________________________ \n");
    printf("select student <criteria> <op> <value> \n select course <criteria> <op> <value> \n");
    printf("________________________delete__________________________________ \n");
    printf("delete student <criteria> <op> <value> \n delete course <criteria> <op> <value> \n");

    printf("Enter your command : ");
    char str[100];
    getchar();
    scanf("%[^\n]", str);

    char command[10];
    char name[20];
    char age[3];
    char major[50];
    char course_name[50];
    char instructor[50];

    sscanf(str, "%s", command);

    if (strcmp(command, "insert") == 0)
    {
        loadData(&studentTree, &courseTree, &enrollmentTree);

        char type[100];
        sscanf(str, "%*s %s", type);

        if (strcmp(type, "student") == 0)
        {
            sscanf(str, "%*s %*s \"%[^\"]\" %s \"%[^\"]\"", name, age, major);

            int major_already_exists = get_course_id(courseTree, major);

            if (major_already_exists == -1)
            {
                printf("Major does not exist\n");
                return 0;
            }
            else
            {
                Student student;
                strcpy(student.major, major);
                int y = getMaxStudentId(studentTree);

                student.student_id = y + 1;
                strcpy(student.name, name);
                student.age = atoi(age);

                studentTree = insert_student(studentTree, student);

                serialize_students_csv(studentTree);
                Enrollement enrollementbasedonstudent = {student.student_id, major_already_exists};
                enrollmentTree = insert_enrollment(enrollmentTree, enrollementbasedonstudent);
                serialize_follow_course_csv(enrollmentTree);

                printf("Student inserted successfully\n");

                printf("follow course inserted succesfully ");
            }
        }
        else if (strcmp(type, "course") == 0)
        {
            sscanf(str, "%*s %*s \"%[^\"]\" \"%[^\"]\"", course_name, instructor);
            int x = getMaxCourseId(courseTree);
            Course course;
            course.course_id = x + 1;
            strcpy(course.course_name, course_name);
            strcpy(course.instructor, instructor);
            insertCourse(courseTree, course);
            serialize_courses_csv(courseTree);
            printf("Course inserted successfully\n");
        }
    }
    else if (strcmp(command, "select") == 0)
    {
        char ComandType[10], type[20], criteria[20], op[5], value[50];
        sscanf(str, "%*s %s %s %s %s %[^\n]", type, criteria, op, value);
        select(courseTree, studentTree, type, criteria, op, value);
    }
    else if (strcmp(command, "delete") == 0)

    {
        char ComandType[10], type[20], criteria[20], op[5], value[50];
        sscanf(str, "%*s %s %s %s %s %[^\n]", type, criteria, op, value);
        if (strcmp(
                type, "student") == 0)
        {
            delete_student(&studentTree, &enrollmentTree, criteria, op, value);
            serialize_students_csv(studentTree);

            print_tree(enrollmentTree);
            serialize_follow_course_csv(enrollmentTree);
            printf("Student deleted successfully\n");
        }
        else if (strcmp(type, "course") == 0)
        {
            delete_course(&courseTree, &enrollmentTree, criteria, op, value);
            serialize_courses_csv(courseTree);
            serialize_follow_course_csv(enrollmentTree);
            printf("Course deleted successfully\n");
        }
    }

    // printf("%s\n", command);
    // printf("%s\n", name);
    // printf("%s\n", age);
    // printf("%s\n", major);

    // arbreS StudentTree = NULL;

    // arbreE enrollmentTree = NULL;

    // Enrollement enrollment;
    // enrollment.student_id = 100;
    // enrollment.course_id = 20;
    // enrollmentTree = insert_enrollment(enrollmentTree, enrollment);

    // arbreS studentTree = NULL;
    // Student student;
    // student.student_id = 1;
    // strcpy(student.name, "Amine");
    // student.age = 20;
    // strcpy(student.major, "DataScience");

    // studentTree = insert_student(studentTree, student);

    // // serialize_students_csv(studentTree);

    // // print_tree(enrollmentTree);
    // // serialize_follow_course_csv(enrollmentTree);

    // arbreC courseTree = NULL;
    // // arbreE enrollmentTree = NULL;
    // // arbreS studentTree = NULL;

    // Course course;
    // course.course_id = 1;
    // strcpy(course.course_name, "hihi");
    // strcpy(course.instructor, "Amine");

    // courseTree = insertCourse(courseTree, course);
    // printf("%d\n", courseTree->course.course_id);
    // serialize_courses_csv(courseTree);

    // saveData(studentTree, courseTree, enrollmentTree);
    return 0;
}