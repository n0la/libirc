#include <stddef.h>
#include <setjmp.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <cmocka.h>

#include <irc/tag.h>

static int setup(void **data)
{
    irc_tag_t t = irc_tag_new();
    if (t == NULL) {
        return -1;
    }

    *data = t;

    return 0;
}

static int teardown(void **data)
{
    irc_tag_free(*data);

    return 0;
}

static void test_tag_parse_no_value(void **data)
{
    irc_tag_t t = *data;
    const char *str = "key";
    irc_error_t error = irc_error_internal;

    assert_ptr_not_equal(t, NULL);
    error = irc_tag_parse(t, str);
    assert_return_code(error, irc_error_success);
    assert_string_equal(t->key, "key");
    assert_ptr_equal(t->value, NULL);
}

static void test_tag_parse_single(void **data)
{
    irc_tag_t t = *data;
    const char *str = "key=value";
    irc_error_t error = irc_error_internal;

    assert_ptr_not_equal(t, NULL);
    error = irc_tag_parse(t, str);
    assert_return_code(error, irc_error_success);
    assert_string_equal(t->key, "key");
    assert_string_equal(t->value, "value");
}

#define ASSERT_TAG_UNSESCAPED(STR, EXPECTED) \
    unscaped = irc_tag_unescape(STR); \
    assert_string_equal(unscaped, EXPECTED); \
    free(unscaped)

static void test_tag_unescape(void **data)
{
    char *unscaped = NULL;

    ASSERT_TAG_UNSESCAPED("\\:", ";");
    ASSERT_TAG_UNSESCAPED("\\s", " ");
    ASSERT_TAG_UNSESCAPED("\\\\", "\\");
    ASSERT_TAG_UNSESCAPED("\\r", "\r");
    ASSERT_TAG_UNSESCAPED("\\n", "\n");
    ASSERT_TAG_UNSESCAPED("test", "test");
    ASSERT_TAG_UNSESCAPED("test\\", "test");
    ASSERT_TAG_UNSESCAPED("test\\b", "testb");
    ASSERT_TAG_UNSESCAPED("\\:\\s\\\\\\r\\n", "; \\\r\n");
}

#define ASSERT_TAG_ESCAPED(STR, EXPECTED) \
    escaped = irc_tag_escape(STR); \
    assert_string_equal(escaped, EXPECTED); \
    free(escaped);

static void test_tag_escape(void **data)
{
    char *escaped = NULL;

    ASSERT_TAG_ESCAPED(";", "\\:");
    ASSERT_TAG_ESCAPED(" ", "\\s");
    ASSERT_TAG_ESCAPED("\\", "\\\\");
    ASSERT_TAG_ESCAPED("\r", "\\r");
    ASSERT_TAG_ESCAPED("\n", "\\n");
    ASSERT_TAG_ESCAPED("test", "test");
    ASSERT_TAG_ESCAPED("; \\\r\n", "\\:\\s\\\\\\r\\n");
}

int main(int ac, char **av)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_tag_parse_no_value, setup,
                                        teardown),
        cmocka_unit_test_setup_teardown(test_tag_parse_single, setup, teardown),
        cmocka_unit_test(test_tag_unescape),
        cmocka_unit_test(test_tag_escape),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
