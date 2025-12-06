#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

extern "C" {

// Print functions
void print_int(int val) { printf("%d", val); }

void print_float(double val) { printf("%f", val); }

void print_string(const char *val) {
  if (val) {
    printf("%s", val);
  } else {
    printf("(null)");
  }
}

void print_bool(bool val) { printf("%s", val ? "true" : "false"); }

void println_int(int val) { printf("%d\n", val); }

void println_float(double val) { printf("%f\n", val); }

void println_string(const char *val) {
  if (val) {
    printf("%s\n", val);
  } else {
    printf("(null)\n");
  }
}

void println_bool(bool val) { printf("%s\n", val ? "true" : "false"); }

// Input functions
// Returns a heap-allocated string that must be freed (though we don't have GC
// yet)
char *input_string() {
  std::string line;
  std::getline(std::cin, line);
  char *cstr = (char *)malloc(line.length() + 1);
  strcpy(cstr, line.c_str());
  return cstr;
}

int input_int() {
  int val;
  scanf("%d", &val);
  // Consume newline
  char c;
  while ((c = getchar()) != '\n' && c != EOF)
    ;
  return val;
}

double input_float() {
  double val;
  scanf("%lf", &val);
  // Consume newline
  char c;
  while ((c = getchar()) != '\n' && c != EOF)
    ;
  return val;
}

// String operations
char *string_concat(const char *s1, const char *s2) {
  if (!s1)
    s1 = "";
  if (!s2)
    s2 = "";
  size_t len1 = strlen(s1);
  size_t len2 = strlen(s2);
  char *result = (char *)malloc(len1 + len2 + 1);
  strcpy(result, s1);
  strcat(result, s2);
  return result;
}

} // extern "C"
