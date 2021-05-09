struct test_struct {
    char *a;
    int b;
};

int test_int_el1 = 1;
int test_int_el2 = 2;
int test_int_el3 = 3;
int test_int_el4 = 4;
int test_int_el5 = 5;

char *test_string_el1 = "Lorem ipsum dolor sit amet";
char *test_string_el2 = "consectetur adipiscing elit";
char *test_string_el3 = "Nam posuere lectus sit amet malesuada dapibus";
char *test_string_el4 = "Proin volutpat";
char *test_string_el5 = "nisi at varius semper";

#define TEST_STRUCT(el) {                   \
struct test_struct test_struct_el(el);      \
test_struct_el(el).a = test_string_el(el);  \
test_struct_el(el).b = test_int_el(el);     \
}
//
//struct test_struct test_struct_el2;
//test_struct_el2.a = test_string_el2;
//test_struct_el2.b = test_int_el2;
//
//struct test_struct test_struct_el3;
//test_struct_el3.a = test_string_el3;
//test_struct_el3.b = test_int_el3;
//
//struct test_struct test_struct_el4;
//test_struct_el4.a = test_string_el4;
//test_struct_el4.b = test_int_el4;
//
//struct test_struct test_struct_el5;
//test_struct_el5.a = test_string_el6;
//test_struct_el5.b = test_int_el5;

int test_struct_cmp(const void *actual, const void *expected) {
    struct test_struct actual_struct;
    struct test_struct expected_struct;

    actual_struct = *(struct test_struct *)actual;
    expected_struct = *(struct test_struct *)expected;

    if (strcmp(actual_struct.a, expected_struct.a) != 0) {
        return -1;
    }

    if (actual_struct.b != expected_struct.b) {
        return -1;
    }

    return 0;
}

