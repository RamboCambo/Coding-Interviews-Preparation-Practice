#include "greatest.h"
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "../src/numbers.h"

TEST test_rotate_number_right_with_input_2_bits_1_should_return_1(void) {
    uint32_t input = 2;
    uint8_t bits = 1;
    uint32_t expected = 1;
    uint32_t actual = rotate_number_right(input, bits);
    ASSERT_EQ(expected, actual);
    PASS();
}

TEST test_rotate_number_right_with_input_234981_bits_7_should_return_3388999467(void) {
    uint32_t input = 234981;
    uint8_t bits = 7;
    uint32_t expected = 3388999467;
    uint32_t actual = rotate_number_right(input, bits);
    ASSERT_EQ(expected, actual);
    PASS();
}

TEST test_rotate_number_right_with_input_80000_bits_32_should_return_80000(void) {
    uint32_t input = 80000;
    uint8_t bits = 32;
    uint32_t expected = 80000;
    uint32_t actual = rotate_number_right(input, bits);
    ASSERT_EQ(expected, actual);
    PASS();
}

SUITE(rotate_number_right_suite) {
    RUN_TEST(test_rotate_number_right_with_input_2_bits_1_should_return_1);
    RUN_TEST(test_rotate_number_right_with_input_234981_bits_7_should_return_3388999467);
    RUN_TEST(test_rotate_number_right_with_input_80000_bits_32_should_return_80000);
}

TEST test_trial_division(uint32_t n, uint32_t* expected_factors, size_t expected_len) {
    size_t len;
    uint32_t* factors = trial_division(n, &len);
    ASSERT_EQ_FMT(expected_len, len, "%zu");
    for (size_t i = 0; i < len; i++) {
        ASSERT_EQ_FMT(expected_factors[i], factors[i], "%u");
    }
    free(factors);
    PASS();
}

TEST test_trial_division_1() {
    uint32_t n = 1;
    uint32_t *expected_factors = NULL;
    size_t expected_len = 0;
    return test_trial_division(n, expected_factors, expected_len);
}

TEST test_trial_division_2() {
    uint32_t n = 2;
    uint32_t expected_factors[] = {2};
    size_t expected_len = 1;
    return test_trial_division(n, expected_factors, expected_len);
}

TEST test_trial_division_3() {
    uint32_t n = 12;
    uint32_t expected_factors[] = {2, 2, 3};
    size_t expected_len = 3;
    return test_trial_division(n, expected_factors, expected_len);
}

TEST test_trial_division_4() {
    uint32_t n = 10;
    uint32_t expected_factors[] = {2, 5};
    size_t expected_len = 2;
    return test_trial_division(n, expected_factors, expected_len);
}

TEST test_trial_division_5() {
    uint32_t n = 15;
    uint32_t expected_factors[] = {3, 5};
    size_t expected_len = 2;
    return test_trial_division(n, expected_factors, expected_len);
}

SUITE(trial_division_suite) {
    RUN_TEST(test_trial_division_1);
    RUN_TEST(test_trial_division_2);
    RUN_TEST(test_trial_division_3);
    RUN_TEST(test_trial_division_4);
    RUN_TEST(test_trial_division_5);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(rotate_number_right_suite);
    RUN_SUITE(trial_division_suite);
    GREATEST_MAIN_END();
}