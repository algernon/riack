#include <riack/riack.h>

START_TEST (test_riack_setop_new_and_free)
{
  riack_setop_t *setop;

  setop = riack_setop_new();
  ck_assert (setop != NULL);
  riack_setop_free (setop);
}

END_TEST

START_TEST (test_riack_setop_set)
{
  
  riack_setop_t *setop;
  
  
  setop = riack_setop_new();
  

  
     
  ck_assert_errno
    (riack_setop_set (setop,
                  RIACK_SETOP_FIELD_ADD, "addsomething",
                  RIACK_SETOP_FIELD_NONE),
                  0);
                  
  
   

  
  
  ck_assert_str_eq (setop->adds[0].data, "addsomething");
  
 

  
  
  riack_setop_free (setop);
  
  
}
END_TEST

static TCase *
test_riack_setop (void)
{
  TCase *test_setop;

  test_setop = tcase_create ("setop");
  tcase_add_test (test_setop, test_riack_setop_new_and_free);
  tcase_add_test (test_setop, test_riack_setop_set);

  return test_setop;
}
