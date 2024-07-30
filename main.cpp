#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define a structure for the email node
struct EmailNode {
    char subject[50];
    char sender[50];
    char message[200];
    struct EmailNode* next;
};

// Define a structure for the user
struct User {
    char* username;
    unsigned int passwordHash;
    struct EmailNode* inbox;
};

// Simple hashing function
unsigned int hashPassword(const char* password) {
    unsigned int hash = 0;
    int c;

    while ((c = *password++)) {
        hash = c + (hash << 6) + (hash << 16) - hash;
    }

    return hash;
}

// Function to find a user by username
struct User* findUser(struct User* users, int numUsers, const char* targetUsername) {
    for (int i = 0; i < numUsers; ++i) {
        if (strcmp(users[i].username, targetUsername) == 0) {
            return &users[i];
        }
    }
    return NULL; // User not found
}

// Function to send an email from one user to another
void sendEmail(struct User* users, int numUsers, const char* senderUsername, const char* receiverUsername,
               const char* subject, const char* message) {
    struct User* sender = findUser(users, numUsers, senderUsername);
    struct User* receiver = findUser(users, numUsers, receiverUsername);

    if (sender != NULL && receiver != NULL) {
        struct EmailNode* newEmail = (struct EmailNode*)malloc(sizeof(struct EmailNode));
        if (newEmail == NULL) {
            printf("Memory allocation error.\n");
            exit(EXIT_FAILURE);
        }

        strncpy(newEmail->subject, subject, sizeof(newEmail->subject) - 1);
        newEmail->subject[sizeof(newEmail->subject) - 1] = '\0'; // Null-terminate the string
        strncpy(newEmail->sender, senderUsername, sizeof(newEmail->sender) - 1);
        newEmail->sender[sizeof(newEmail->sender) - 1] = '\0'; // Null-terminate the string
        strncpy(newEmail->message, message, sizeof(newEmail->message) - 1);
        newEmail->message[sizeof(newEmail->message) - 1] = '\0'; // Null-terminate the string

        // Insert the new email at the beginning of the receiver's inbox
        newEmail->next = receiver->inbox;
        receiver->inbox = newEmail;
        printf("Email sent successfully from %s to %s\n", senderUsername, receiverUsername);
    } else {
        printf("User not found. Email not sent.\n");
    }
}

// Function to display the inbox of a specific user
void displayInbox(const struct User* user) {
    printf("Inbox for user %s:\n", user->username);
    struct EmailNode* current = user->inbox;
    while (current != NULL) {
        printf("-----------------------------\n");
        printf("Subject: %s\n", current->subject);
        printf("Sender: %s\n", current->sender);
        printf("Message: %s\n", current->message);
        
        current = current->next;
    }
}

// Function to register a new user with hashed password
void registerUser(struct User** users, int* numUsers, const char* newUsername, const char* newPassword) {
    *users = (struct User*)realloc(*users, (*numUsers + 1) * sizeof(struct User));
    if (*users == NULL) {
        printf("Memory allocation error.\n");
        exit(EXIT_FAILURE);
    }

    (*users)[*numUsers].username = strdup(newUsername);
    if ((*users)[*numUsers].username == NULL) {
        printf("Memory allocation error.\n");
        exit(EXIT_FAILURE);
    }
    (*users)[*numUsers].passwordHash = hashPassword(newPassword);
    (*users)[*numUsers].inbox = NULL;

    printf("User %s registered successfully.\n", newUsername);
    (*numUsers)++;
}

// Function to get user input for strings
char* getUserInput(const char* prompt) {
    char buffer[200];

    printf("%s", prompt);
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline character

    // Allocate memory for the string and copy the input
    char* userInput = (char*)malloc(strlen(buffer) + 1);
    if (userInput == NULL) {
        printf("Memory allocation error.\n");
        exit(EXIT_FAILURE);
    }
    strcpy(userInput, buffer);

    return userInput;
}

// Function to handle the main menu
int mainMenu() {
    int choice;
    printf("\nMain Menu:\n");
    printf("1. Login\n");
    printf("2. Sign Up\n");
    printf("3. Exit\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    getchar(); // Consume the newline character in the buffer

    return choice;
}

// Function to handle the user menu
int userMenu() {
    int choice;
    printf("\nUser Menu:\n");
    printf("1. View Messages\n");
    printf("2. Send Email\n");
    printf("3. Logout\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    getchar(); // Consume the newline character in the buffer

    return choice;
}


int main() {
    struct User* users = NULL;
    int numUsers = 0;
    int mainChoice, userChoice;
    char* username;

    do {
        mainChoice = mainMenu();

        switch (mainChoice) {
            case 1: {

                username = getUserInput("Enter your username: ");
                char* password = getUserInput("Enter your password: ");
                struct User* currentUser = findUser(users, numUsers, username);
                if (currentUser != NULL && currentUser->passwordHash == hashPassword(password)) {
                    do {
                        userChoice = userMenu();

                        switch (userChoice) {
                            case 1:

                                displayInbox(currentUser);
                                break;
                            case 2: {

                                char* receiver = getUserInput("Enter receiver's username: ");
                                char* subject = getUserInput("Enter email subject: ");
                                char* message = getUserInput("Enter email message: ");

                                sendEmail(users, numUsers, username, receiver, subject, message);

                                // Free the memory allocated for inputs
                                free(receiver);
                                free(subject);
                                free(message);
                                break;
                            }
                            case 3:
                                
                                printf("Logging out...\n");
                                break;
                            default:
                                
                                printf("Invalid choice. Please enter a valid option.\n");
                                break;
                        }
                    } while (userChoice != 3);
                } else {
                    
                    printf("User not found or password incorrect.\n");
                }

                // Free the memory allocated for username and password
                free(username);
                free(password);
                break;
            }
            case 2: {
                
                char* newUsername = getUserInput("Enter a new username: ");
                char* newPassword = getUserInput("Enter a new password: ");
                registerUser(&users, &numUsers, newUsername, newPassword);

                // Free the memory allocated for the new username and password
                free(newUsername);
                free(newPassword);
                break;
            }
            case 3:
                
                printf("Exiting program.\n");
                break;
            default:
                
                printf("Invalid choice. Please enter a valid option.\n");
                break;
        }

    } while (mainChoice != 3);

    // Clean up allocated memory for emails (deallocate linked lists)
    for (int i = 0; i < numUsers; ++i) {
        struct EmailNode* current = users[i].inbox;
        while (current != NULL) {
            struct EmailNode* next = current->next;
            free(current);
            current = next;
        }
        free(users[i].username);
    }

    // Clean up allocated memory for users
    free(users);

    return 0;
}
