#include <riack/riack.h>

START_TEST (test_riack_content_new_and_free)
{
  riack_content_t *content;

  content = riack_content_new ();
  ck_assert (content != NULL);
  riack_content_free (content);
}
END_TEST

START_TEST (test_riack_content_set)
{
  riack_content_t *content;

  content = riack_content_new ();

  ck_assert_errno
    (riack_content_set
     (NULL, RIACK_CONTENT_FIELD_NONE),
     EINVAL);

  ck_assert_errno
    (riack_content_set (content,
                        RIACK_CONTENT_FIELD_VALUE, "some-value", -1,
                        RIACK_CONTENT_FIELD_CONTENT_TYPE, "text/plain", -1,
                        RIACK_CONTENT_FIELD_CONTENT_ENCODING, "none", -1,
                        RIACK_CONTENT_FIELD_CHARSET, "utf8", -1,
                        RIACK_CONTENT_FIELD_NONE),
     0);

  /*
  ck_assert_str_eq (content->value, "some-value");
  ck_assert_str_eq (content->content_type, "text/plain");
  ck_assert_str_eq (content->content_encoding, "none");
  ck_assert_str_eq (content->charset, "utf8");
  */

  riack_content_free (content);
}
END_TEST

static TCase *
test_riack_content (void)
{
  TCase *test_content;

  test_content = tcase_create ("Content");
  tcase_add_test (test_content, test_riack_content_new_and_free);
  tcase_add_test (test_content, test_riack_content_set);

  return test_content;
}
