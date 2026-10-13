#include <stdio.h>
#include <stdlib.h>

void show_menu() {
  printf("\n==================================\n");
  printf("       LAB 4 Transformations      \n");
  printf("==================================\n");
  printf("1. Reflection\n");
  printf("2. Rotation\n");
  printf("3. Scaling\n");
  printf("4. Shearing\n");
  printf("5. Translation\n");
  printf("0. Exit\n");
  printf("==================================\n");
  printf("Enter your choice: ");
}

int main() {
  int choice;

  while (1) {
    show_menu();
    if (scanf("%d", &choice) != 1) {
      printf("Invalid input. Please enter a number.\n");
      // Clear input buffer
      while (getchar() != '\n')
        ;
      continue;
    }

    switch (choice) {
    case 1:
      printf("\n--- Running Reflection ---\n");
      system("gcc reflection.c -lm -o reflection && ./reflection");
      break;
    case 2:
      printf("\n--- Running Rotation ---\n");
      system("gcc rotation.c -lm -o rotation && ./rotation");
      break;
    case 3:
      printf("\n--- Running Scaling ---\n");
      system("gcc scaling.c -lm -o scaling && ./scaling");
      break;
    case 4:
      printf("\n--- Running Shearing ---\n");
      system("gcc shearing.c -lm -o shearing && ./shearing");
      break;
    case 5:
      printf("\n--- Running Translation ---\n");
      system("gcc translation.c -lm -o translation && ./translation");
      break;
    case 0:
      printf("Exiting menu.\n");
      return 0;
    default:
      printf("Invalid choice. Please try again.\n");
    }
  }

  return 0;
}
