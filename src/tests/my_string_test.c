#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../utils/my_string.h"

void test_initialization() {
    printf("Testing Initialization...\n");
    //
    // Test stack initialization
    string_t s1;
    assert(init_string(&s1, 10) != NULL);
    assert(s1.capacity == 10);
    assert(s1.len == 0);
    deinit_string(&s1);

    // Test init with message
    string_t s2;
    const char *msg = "Hello";
    assert(init_with_msg(&s2, msg) != NULL);
    assert(s2.len == 5);
    assert(s2.capacity == 6);
    assert(strcmp(s2.buffer, "Hello") == 0);
    deinit_string(&s2);

    // Test heap allocation
    string_t *s3 = new_string(20);
    assert(s3 != NULL);
    assert(s3->capacity == 20);
    delete_string(s3);

    printf("Initialization tests passed!\n");
}

void test_copy_and_resize() {
    printf("Testing Copy and Resize...\n");

    string_t s;
    init_string(&s, 5); // Small capacity

    // Test string_cpy with auto-resize
    string_cpy(&s, "This is a long string");
    assert(s.len == 21);
    assert(s.capacity >= 21);
    assert(strncmp(s.buffer, "This is a long string", s.len) == 0);

    // Test string_ncpy
    string_ncpy(&s, "ABCDEF", 3);
    assert(s.len == 3);
    assert(strncmp(s.buffer, "ABC", 3) == 0);

    deinit_string(&s);
    printf("Copy and Resize tests passed!\n");
}

void test_concatenation() {
    printf("Testing Concatenation...\n");

    string_t s;
    init_with_msg(&s, "Foot");
    
    // Test cat
    string_cat(&s, "ball");
    assert(s.len == 8);
    // Note: your implementation doesn't null-terminate automatically, 
    // but the content should be correct.
    assert(memcmp(s.buffer, "Football", 8) == 0);

    // Test ncat
    string_ncat(&s, " is fun", 3); // Just " is"
    assert(s.len == 11);
    assert(memcmp(s.buffer, "Football is", 11) == 0);

    deinit_string(&s);
    printf("Concatenation tests passed!\n");
}

void test_comparison() {
    printf("Testing Comparison...\n");

    string_t *a = new_string(10);
    string_t *b = new_string(10);

    string_cpy(a, "Apple");
    string_cpy(b, "Apple");
    assert(string_cmp(a, b) == 0);

    string_cpy(b, "Apply");
    assert(string_cmp(a, b) < 0); // 'e' < 'y'

    string_cpy(b, "App");
    assert(string_cmp(a, b) > 0); // "Apple" is longer than "App"

    // Test NULL handling
    assert(string_cmp(NULL, NULL) == 0);
    assert(string_cmp(a, NULL) > 0);

    delete_string(a);
    delete_string(b);
    printf("Comparison tests passed!\n");
}

int main() {
    test_initialization();
    test_copy_and_resize();
    test_concatenation();
    test_comparison();

    printf("\nAll tests passed successfully!\n");
    return 0;
}
