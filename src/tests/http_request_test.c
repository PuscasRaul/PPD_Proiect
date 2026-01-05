#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../utils/string_view.h"
#include "../http/request.h"
#include "../utils/logger.h"

// Helper to check string_view against a C string
int view_cmp(string_view view, const char *str) {
    if (view.len != strlen(str)) return -1;
    return memcmp(view.buffer, str, view.len);
}

void test_extract_request_line() {
    printf("Testing Request Line Extraction...\n");
    
    http_request req;
    init_http_request(&req);

    const char *raw_req = "GET /index.html HTTP/1.1\r\nHost: localhost\r\n\r\n";
    int result = http_extract_req_line(raw_req, strlen(raw_req), &req);

    assert(result == 0);
    assert(req.method == GET);
    assert(view_cmp(req.http_version, "HTTP/1.1") == 0);
    log_debug("URI: %s", req.uri.buffer);
    assert(view_cmp(req.uri, "/index.html") == 0);
    
    // Note: Verify if your implementation includes the CRLF in status_line
    printf("Request Line Method and URI: OK\n");

    deinit_http_request(&req);
}

void test_extract_headers() {
    printf("Testing Header Extraction...\n");
    
    http_request req;
    init_http_request(&req);
    
    // Manually setting up status line as extract_headers depends on raw_data state
    const char *status_line = "POST /api/data HTTP/1.1\r\n";
    http_extract_req_line(status_line, strlen(status_line), &req);

    const char *header_block = "Content-Type: application/json\r\nContent-Length: 15\r\n\r\n";
    int result = http_extract_headers(header_block, strlen(header_block), &req);

    assert(result == 0);
    assert(req.headers_amt == 2);
    // Check first header
    assert(view_cmp(req.header_tab[0].name, "Content-Type") == 0);
    // Be careful: your current code might include the leading space in the value
    assert(view_cmp(req.header_tab[0].value, " application/json") == 0);

    printf("Headers Extracted: %zu (Expected 2): OK\n", req.headers_amt);
    deinit_http_request(&req);
}

void test_extract_body() {
    printf("Testing Body Extraction...\n");
    
    http_request req;
    init_http_request(&req);

    // Mocking the previous state
    string_cpy(&req.raw_data, "GET / HTTP/1.1\r\n\r\n");
    
    const char *body_content = "{\"key\":\"value\"}\r\n";
    int result = http_extract_body(body_content, strlen(body_content), &req);

    assert(result == 0);
    assert(view_cmp(req.body, "{\"key\":\"value\"}") == 0);

    printf("Body Extraction: OK\n");

    deinit_http_request(&req);
}

void test_malformed_request() {
    printf("Testing Malformed Requests...\n");

    http_request req;
    init_http_request(&req);

    // No CRLF in request line
    const char *bad_req = "GET /index.html"; 
    assert(http_extract_req_line(bad_req, strlen(bad_req), &req) == -1);

    // Unknown method
    const char *unknown_method = "FLY /sky HTTP/1.1\r\n";
    http_extract_req_line(unknown_method, strlen(unknown_method), &req);
    assert(req.method == UNRECOGNIZED);

    deinit_http_request(&req);
    printf("Error Handling: OK\n");
}

int main() {
    test_extract_request_line();
    test_extract_headers();
    test_extract_body();
    test_malformed_request();

    printf("\nAll HTTP Request tests passed!\n");
    return 0;
}
