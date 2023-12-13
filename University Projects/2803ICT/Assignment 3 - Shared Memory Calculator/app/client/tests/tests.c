#include "greatest.h"
#include "../src/input_validation.h"

TEST test_is_input_numeric_valid_input(void)
{
    char input[] = "12345";
    int result = is_input_numeric(input);
    ASSERT_EQ(1, result);
    PASS();
}

TEST test_is_input_numeric_invalid_input(void)
{
    char input[] = "1234a";
    int result = is_input_numeric(input);
    ASSERT_EQ(0, result);
    PASS();
}

TEST test_is_input_numeric_alphanumeric_input(void)
{
    char input[] = "1234a5678";
    int result = is_input_numeric(input);
    ASSERT_EQ(0, result);
    PASS();
}

TEST test_is_input_numeric_empty_input(void)
{
    char input[] = "";
    int result = is_input_numeric(input);
    ASSERT_EQ(0, result);
    PASS();
}

SUITE(input_validation_suite)
{
    RUN_TEST(test_is_input_numeric_valid_input);
    RUN_TEST(test_is_input_numeric_invalid_input);
    RUN_TEST(test_is_input_numeric_alphanumeric_input);
    RUN_TEST(test_is_input_numeric_empty_input);
}

GREATEST_MAIN_DEFS();

int main(int argc, char *argv[])
{
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(input_validation_suite);
    GREATEST_MAIN_END();
}