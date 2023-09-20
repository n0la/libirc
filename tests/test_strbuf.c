#include <setjmp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include <cmocka.h>

#include <irc/strbuf.h>

static void test_strbuf_new(void **data)
{
    strbuf_t b = strbuf_new();

    assert_true(strbuf_len(b) == 0);
    assert_true(strbuf_strdup(b) == NULL);

    strbuf_free(b);
}

static void test_strbuf_append(void **data)
{
    strbuf_t b = strbuf_new();

    assert_true(strbuf_append(b, "test\n", -1)
              == strlen("test\n")
        );
    assert_true(strbuf_append(b, "foo\n", -1)
              == strlen("foo\n")
        );

    assert_true(strbuf_len(b) == (strlen("foo\n") + strlen("test\n")));

    strbuf_free(b);
}

static void test_strbuf_getline(void **data)
{
    strbuf_t b = strbuf_new();
    char *line = NULL;
    size_t linelen = 0;

    strbuf_append(b, "test\n", -1);
    strbuf_append(b, "foo\n", -1);
    strbuf_append(b, "bar\n", -1);

    assert_true(strbuf_getline(b, &line, &linelen) == 0);
    assert_true(strcmp(line, "test\n") == 0);
    assert_true(strbuf_len(b) == (strlen("foo\n") + strlen("bar\n")));

    free(line);
    strbuf_free(b);
}

static void test_strbuf_getline_empty(void **data)
{
    strbuf_t b = strbuf_new();
    char *line = NULL;
    size_t linelen = 0;

    strbuf_append(b, "test\n", -1);
    strbuf_append(b, "foo\n", -1);
    strbuf_append(b, "bar\n", -1);

    assert_true(strbuf_getline(b, &line, &linelen) == 0);
    assert_true(strcmp(line, "test\n") == 0);
    assert_true(strbuf_len(b) == (strlen("foo\n") + strlen("bar\n")));

    free(line);
    linelen = 0;

    assert_true(strbuf_getline(b, &line, &linelen) == 0);
    assert_true(strcmp(line, "foo\n") == 0);
    assert_true(strbuf_len(b) == strlen("bar\n"));

    free(line);
    linelen = 0;

    assert_true(strbuf_getline(b, &line, &linelen) == 0);
    assert_true(strcmp(line, "bar\n") == 0);
    assert_true(strbuf_len(b) == 0);

    free(line);
    linelen = 0;
    strbuf_free(b);
}

static void test_strbuf_getstr(void **data)
{
    strbuf_t b = strbuf_new();
    char *line = NULL;
    size_t linelen = 0;

    strbuf_append(b, "test\r\n", -1);
    strbuf_append(b, "foo\r\n", -1);
    strbuf_append(b, "bar\r\n", -1);

    assert_true(strbuf_getstr(b, &line, &linelen, "\r\n") == 0);
    assert_true(strcmp(line, "test\r\n") == 0);
    assert_true(strbuf_len(b) == (strlen("foo\r\n") + strlen("bar\r\n")));

    free(line);
    strbuf_free(b);
}

static void test_strbuf_getline_depleted(void **data)
{
    strbuf_t b = strbuf_new();
    char *line = NULL;
    size_t linelen = 0;

    strbuf_append(b, "test\n", -1);
    strbuf_append(b, "foo\n", -1);
    strbuf_append(b, "bar\n", -1);

    assert_true(strbuf_getline(b, &line, &linelen) == 0);
    free(line);
    linelen = 0;
    assert_true(strbuf_getline(b, &line, &linelen) == 0);
    free(line);
    linelen = 0;
    assert_true(strbuf_getline(b, &line, &linelen) == 0);
    free(line);
    linelen = 0;

    /* no more data available
     */
    assert_true(strbuf_getline(b, &line, &linelen) < 0);

    strbuf_free(b);
}

static void test_strbuf_getstr_empty(void **data)
{
    strbuf_t b = strbuf_new();
    char *line = NULL;
    size_t linelen = 0;

    strbuf_append(b, "test\r\n", -1);
    strbuf_append(b, "foo\r\n", -1);
    strbuf_append(b, "bar\r\n", -1);

    assert_true(strbuf_getstr(b, &line, &linelen, "\r\n") == 0);
    assert_true(strcmp(line, "test\r\n") == 0);
    assert_true(strbuf_len(b) == (strlen("foo\r\n") + strlen("bar\r\n")));

    free(line);
    linelen = 0;

    assert_true(strbuf_getstr(b, &line, &linelen, "\r\n") == 0);
    assert_true(strcmp(line, "foo\r\n") == 0);
    assert_true(strbuf_len(b) == strlen("bar\r\n"));

    free(line);
    linelen = 0;

    assert_true(strbuf_getstr(b, &line, &linelen, "\r\n") == 0);
    assert_true(strcmp(line, "bar\r\n") == 0);
    assert_true(strbuf_len(b) == 0);

    free(line);
    linelen = 0;
    strbuf_free(b);
}

static void test_strbuf_getstr_depleted(void **data)
{
    strbuf_t b = strbuf_new();
    char *line = NULL;
    size_t linelen = 0;

    strbuf_append(b, "test\r\n", -1);
    strbuf_append(b, "foo\r\n", -1);
    strbuf_append(b, "bar\r\n", -1);

    assert_true(strbuf_getstr(b, &line, &linelen, "\r\n") == 0);
    free(line);
    linelen = 0;
    assert_true(strbuf_getstr(b, &line, &linelen, "\r\n") == 0);
    free(line);
    linelen = 0;
    assert_true(strbuf_getstr(b, &line, &linelen, "\r\n") == 0);
    free(line);
    linelen = 0;

    /* no more data available
     */
    assert_true(strbuf_getstr(b, &line, &linelen, "\r\n") < 0);

    strbuf_free(b);
}

static void test_strbuf_getstr_partial(void **data)
{
    strbuf_t b = strbuf_new();
    char *line = NULL;
    size_t linelen = 0;

    strbuf_append(b, "test\n", -1);
    strbuf_append(b, "foo\r\n", -1);
    strbuf_append(b, "bar\r\n", -1);
    strbuf_append(b, "meh", -1);

    assert_true(strbuf_getstr(b, &line, &linelen, "\r\n") == 0);
    assert_true(strcmp(line, "test\nfoo\r\n") == 0);
    free(line);
    linelen = 0;
    assert_true(strbuf_getstr(b, &line, &linelen, "\r\n") == 0);
    assert_true(strcmp(line, "bar\r\n") == 0);
    free(line);
    linelen = 0;
    assert_true(strbuf_len(b) == strlen("meh"));

    /* no more data available
     */
    assert_true(strbuf_getstr(b, &line, &linelen, "\r\n") < 0);

    strbuf_free(b);
}

int main(int ac, char **av)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_strbuf_new),
        cmocka_unit_test(test_strbuf_append),
        cmocka_unit_test(test_strbuf_getline),
        cmocka_unit_test(test_strbuf_getline_empty),
        cmocka_unit_test(test_strbuf_getstr),
        cmocka_unit_test(test_strbuf_getline_depleted),
        cmocka_unit_test(test_strbuf_getstr_empty),
        cmocka_unit_test(test_strbuf_getstr_depleted),
        cmocka_unit_test(test_strbuf_getstr_partial),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
