#include <stdio.h>
#include <string.h>

// Function Declarations
void createAccount();
void login();
void checkBalance(int inputAcc, int inputPin);
void depositMoney(int inputAcc, int inputPin);
void withdrawMoney(int inputAcc, int inputPin);
void deleteAccountA();
void deleteAccountU(int inputAcc, int inputPin);
void getAccountIndex(int inputAcc, int inputPin, int *acc_no, char name[], int *pin, float *balance, int *found);

int main() {
    int opt;
    printf("********** Welcome to C Bank **********\n\n");

    do {
        printf("MAIN MENU\n1. Create Account\n2. Login\n3. Exit\nEnter your choice: ");
        scanf("%d", &opt);

        switch (opt) {
            case 1: createAccount(); break;
            case 2: login(); break;
            case 3: printf("THANK YOU & Goodbye!\n"); break;
            default: printf("INVALID CHOICE\n");
        }
    } while (opt != 3);

    return 0;
}

// Create a new account (checks for duplicate account number)
void createAccount() {
    int acc_no, pin, exists = 0;
    char name[30];
    float balance;

    printf("\n-- Create Account --\n");
    printf("Enter Account Number: ");
    scanf("%d", &acc_no); getchar(); // remove newline from buffer

    // Check if account number already exists
    FILE *check = fopen("accounts.txt", "r");
    if (check != NULL) {
        int a, p;
        float b;
        char n[30];
        while (fscanf(check, "%d \"%[^\"]\" %d %f", &a, n, &p, &b) != EOF) {
            if (a == acc_no) {
                exists = 1;
                break;
            }
        }
        fclose(check);
    }

    if (exists) {
        printf("An account with this number already exists. Please try again with a different number.\n");
        return;
    }

    // Take user input for account details
    printf("Enter Name: ");
    fgets(name, 30, stdin);
    name[strcspn(name, "\n")] = '\0'; // remove newline

    printf("Set 4-digit PIN: ");
    scanf("%d", &pin);
    if (pin < 1000 || pin > 9999) {
        printf("Invalid PIN. Must be 4 digits.\n");
        return;
    }

    printf("Enter Initial Deposit Amount (Rs): ");
    scanf("%f", &balance);

    // Save account to file
    FILE *fatm = fopen("accounts.txt", "a");
    fprintf(fatm, "%d \"%s\" %d %.2f\n", acc_no, name, pin, balance);
    fclose(fatm);

    printf("Account created successfully!\n");
}

// Login menu (User or Admin)
void login() {
    int log_opt;
    printf("\nLogin Menu\n1. User Login\n2. Admin Login\nEnter your choice: ");
    scanf("%d", &log_opt);

    switch (log_opt) {
        case 1: {
            int inputAcc, inputPin, acc_no, pin, found;
            char name[30];
            float balance;

            printf("Enter Account Number: ");
            scanf("%d", &inputAcc);
            printf("Enter 4-digit PIN: ");
            scanf("%d", &inputPin);

            getAccountIndex(inputAcc, inputPin, &acc_no, name, &pin, &balance, &found);

            if (found) {
                int choice;
                printf("Login successful! Welcome, %s\n", name);
                do {
                    printf("\n***** User Menu *****\n1. Check Balance\n2. Deposit Money\n3. Withdraw Money\n4. Delete Account\n5. Logout\nEnter your choice: ");
                    scanf("%d", &choice);

                    switch (choice) {
                        case 1: checkBalance(inputAcc, inputPin); break;
                        case 2: depositMoney(inputAcc, inputPin); break;
                        case 3: withdrawMoney(inputAcc, inputPin); break;
                        case 4: deleteAccountU(inputAcc, inputPin); choice = 5; break;
                        case 5: printf("Logging out...\n"); break;
                        default: printf("Invalid option.\n");
                    }

                } while (choice != 5);
            } else {
                printf("Invalid Account Number or PIN\n");
            }
            break;
        }

        case 2: {
            char adminPass[20];
            printf("Enter Admin Password: ");
            scanf("%s", adminPass);

            if (strcmp(adminPass, "secret") == 0) {
                int choice;
                do {
                    printf("\n***** Admin Menu *****\n1. View All Accounts\n2. Delete an Account\n3. Logout\nEnter your choice: ");
                    scanf("%d", &choice);

                    switch (choice) {
                        case 1: {
                            FILE *fatm = fopen("accounts.txt", "r");
                            if (fatm == NULL) {
                                printf("Unable to open file.\n");
                                break;
                            }
                            int acc_no, pin;
                            char name[30];
                            float balance;

                            printf("\n-- All Accounts --\n");
                            while (fscanf(fatm, "%d \"%[^\"]\" %d %f", &acc_no, name, &pin, &balance) != EOF)
                                printf("Account: %d | Name: %s | PIN: %d | Balance: %.2fRs\n", acc_no, name, pin, balance);
                            fclose(fatm);
                            break;
                        }
                        case 2: deleteAccountA(); break;
                        case 3: printf("Logging out of Admin Menu...\n"); break;
                        default: printf("Invalid choice.\n");
                    }

                } while (choice != 3);

            } else {
                printf("Incorrect Admin Password!\n");
            }
            break;
        }

        default: printf("Invalid option!\n");
    }
}

// Shows current balance
void checkBalance(int inputAcc, int inputPin) {
    int acc_no, pin, found;
    char name[30];
    float balance;

    getAccountIndex(inputAcc, inputPin, &acc_no, name, &pin, &balance, &found);

    if (found)
        printf("Your current balance is: %.2fRs\n", balance);
    else
        printf("Account not found!\n");
}

// Deposits money into account
void depositMoney(int inputAcc, int inputPin) {
    FILE *fatm = fopen("accounts.txt", "r");
    if (fatm == NULL) {
        printf("Unable to open file.\n");
        return;
    }

    int acc_no[100], pin[100], accountCount = 0, targetIndex = -1;
    char name[100][30];
    float balance[100], db;

    // Read all accounts and find the target account
    while (fscanf(fatm, "%d \"%[^\"]\" %d %f", &acc_no[accountCount], name[accountCount], &pin[accountCount], &balance[accountCount]) != EOF) {
        if (acc_no[accountCount] == inputAcc && pin[accountCount] == inputPin)
            targetIndex = accountCount;
        accountCount++;
    }
    fclose(fatm);

    if (targetIndex != -1) {
        printf("Enter amount to deposit (Rs): ");
        scanf("%f", &db);
        balance[targetIndex] += db;

        // Rewrite updated accounts
        fatm = fopen("accounts.txt", "w");
        for (int j = 0; j < accountCount; j++)
            fprintf(fatm, "%d \"%s\" %d %.2f\n", acc_no[j], name[j], pin[j], balance[j]);
        fclose(fatm);

        printf("Deposit successful. New Balance: %.2fRs\n", balance[targetIndex]);
    } else {
        printf("Account not found!\n");
    }
}

// Withdraws money from account
void withdrawMoney(int inputAcc, int inputPin) {
    FILE *fatm = fopen("accounts.txt", "r");
    if (fatm == NULL) {
        printf("Unable to open file.\n");
        return;
    }

    int acc_no[100], pin[100], accountCount = 0, targetIndex = -1;
    char name[100][30];
    float balance[100], wd;

    // Read all accounts and find the target account
    while (fscanf(fatm, "%d \"%[^\"]\" %d %f", &acc_no[accountCount], name[accountCount], &pin[accountCount], &balance[accountCount]) != EOF) {
        if (acc_no[accountCount] == inputAcc && pin[accountCount] == inputPin)
            targetIndex = accountCount;
        accountCount++;
    }
    fclose(fatm);

    if (targetIndex != -1) {
        printf("Enter amount to withdraw (Rs): ");
        scanf("%f", &wd);

        if (wd > balance[targetIndex]) {
            printf("Insufficient balance.\n");
            return;
        }

        balance[targetIndex] -= wd;

        // Rewrite updated accounts
        fatm = fopen("accounts.txt", "w");
        for (int j = 0; j < accountCount; j++)
            fprintf(fatm, "%d \"%s\" %d %.2f\n", acc_no[j], name[j], pin[j], balance[j]);
        fclose(fatm);

        printf("Withdrawal successful. New Balance: %.2fRs\n", balance[targetIndex]);
    } else {
        printf("Account not found!\n");
    }
}

// Admin deletes any account
void deleteAccountA() {
    int inputAcc;
    printf("Enter Account Number to delete: ");
    scanf("%d", &inputAcc);

    FILE *fp = fopen("accounts.txt", "r"), *temp = fopen("temp.txt", "w");
    int a, p, deleted = 0;
    float b;
    char n[30];

    // Copy all except the account to delete
    while (fscanf(fp, "%d \"%[^\"]\" %d %f", &a, n, &p, &b) != EOF) {
        if (a == inputAcc) {
            deleted = 1;
            continue;
        }
        fprintf(temp, "%d \"%s\" %d %.2f\n", a, n, p, b);
    }

    fclose(fp); fclose(temp);
    remove("accounts.txt");
    rename("temp.txt", "accounts.txt");

    if (deleted)
        printf("Account deleted successfully.\n");
    else
        printf("Account not found.\n");
}

// User deletes their own account
void deleteAccountU(int inputAcc, int inputPin) {
    char confirm[4];
    printf("Are you sure you want to delete your account? (yes/no): ");
    scanf("%s", confirm);

    if (strcmp(confirm, "yes") != 0) {
        printf("Account deletion cancelled.\n");
        return;
    }

    FILE *fp = fopen("accounts.txt", "r"), *temp = fopen("temp.txt", "w");
    int a, p;
    float b;
    char n[30];

    while (fscanf(fp, "%d \"%[^\"]\" %d %f", &a, n, &p, &b) != EOF) {
        if (a != inputAcc || p != inputPin)
            fprintf(temp, "%d \"%s\" %d %.2f\n", a, n, p, b);
    }

    fclose(fp); fclose(temp);
    remove("accounts.txt");
    rename("temp.txt", "accounts.txt");

    printf("Your account has been deleted successfully.\n");
}

// Fetch account by acc_no and pin
void getAccountIndex(int inputAcc, int inputPin, int *acc_no, char name[], int *pin, float *balance, int *found) {
    FILE *fatm = fopen("accounts.txt", "r");
    *found = 0;

    if (fatm == NULL) {
        printf("Unable to open file.\n");
        return;
    }

    while (fscanf(fatm, "%d \"%[^\"]\" %d %f", acc_no, name, pin, balance) != EOF) {
        if (*acc_no == inputAcc && *pin == inputPin) {
            *found = 1;
            break;
        }
    }

    fclose(fatm);
}
