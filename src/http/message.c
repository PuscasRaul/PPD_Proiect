#include "message.h"

http_message *http_parse(
    const char *data,
    size_t len,
    http_message *msg
    ) {
  if (data == NULL) 
    return NULL; 

  if (msg == NULL) 
    /* 
     * can't do this since it's only a pointer 
     * solve this issue after parsing logic is done
     */
    msg = new_http_message(len + 1);
  
  /* 
   * Each message section is in itself separated by a CRLF
   * start_line CRLF
   * *(header CRLF) CRLF
   * [body]
   *
   * In order to correctly identify the start_line we can split and
   * go untill the very first CRLF marking the start_line view.
   * In order to move forward we must check the validity of it.
   * For it to be valid, it must have:
   * METHOD SP URI SP HTTP VERSION -> for a request
   * 
   * After checking it's validity, we delimitate the header section
   * by searching for the CRLF CRLF pattern, marking the start of body.
   * In order to have validity of headers, we must compare them to a
   * DB, I think. But that is for later.
   * 
   *
   * For the body, we simply extract its contents.
   */
  
}
