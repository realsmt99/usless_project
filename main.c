#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "malloc.h"
#include <stddef.h>

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
    char course_name[50];
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

    if (course.course_id < root->course.course_id)
    {
        root->left = insertCourse(root->left, course);
    }
    else if (course.course_id > root->course.course_id)
    {
        root->right = insertCourse(root->right, course);
    }

    return root;
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
    else if (e.student_id < root->student.student_id)
    {
        root->left = insert_student(root->left, e);
    }
    else if (e.student_id > root->student.student_id)
    {
        root->right = insert_student(root->right, e);
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
    else if (e.student_id < root->value.student_id)
    {
        root->left = insert_enrollment(root->left, e);
    }
    else if (e.student_id > root->value.student_id)
    {
        root->right = insert_enrollment(root->right, e);
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

    if (root->right != NULL)
    {
        return getMaxStudentId(root->right);
    }

    return root->student.student_id;
}

int getMaxCourseId(arbreC root)
{

    if (root == NULL)
    {
        return -1;
    }

    if (root->right != NULL)
    {
        return getMaxCourseId(root->right);
    }

    return root->course.course_id;
}

int main()
{

    printf("Hello and welcome to our database\n");
    printf("Please choose one of the following options:\n");
    printf("1- Load \n");
    printf("2- Save\n");
    printf("3- Close\n");

    printf("Skip");

    int choice;
    scanf("%d", &choice);
    arbreC courseTree = NULL;
    arbreE enrollmentTree = NULL;
    arbreS studentTree = NULL;

    switch (choice)
    {
    case 1:
        printf("Loading data...\n");
        loadData(&studentTree, &courseTree, &enrollmentTree);
        printf("Data loaded successfully\n");

    case 2:
        printf("Saving data...\n");
        saveData(studentTree, courseTree, enrollmentTree);
        printf("Data saved successfully\n");
        break;
    case 3:
        printf("Closing data...\n");
        closeData(&studentTree, &courseTree, &enrollmentTree);
        printf("Data closed successfully\n");
        break;
    default:
        printf("Invalid choice\n");
        break;
    }

    printf("Enter the insert Command ");
    char str[100];
    getchar();
    scanf("%[^\n]", str);

    char command[100];
    char name[100];
    char age[100];
    char major[100];
    char course_name[100];
    char instructor[100];

    sscanf(str, "%s", command);

    if (strcmp(command, "insert") == 0)
    {
        char type[100];
        sscanf(str, "%*s %s", type);

        if (strcmp(type, "student") == 0)
        {
            sscanf(str, "%*s %*s \"%[^\"]\" %s \"%[^\"]\"", name, age, major);
            int y = getMaxStudentId(studentTree);
            Student student;
            student.student_id = y + 1;
            strcpy(student.name, name);
            student.age = atoi(age);
            strcpy(student.major, major);
            insert_student(studentTree, student);
            serialize_students_csv(studentTree);
            printf("Student inserted successfully\n");
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