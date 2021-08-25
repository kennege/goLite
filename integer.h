#ifndef _INT_H_
#define _INT_H_

struct token *integer_binary_operations(const struct token *left, const struct token *right, const char *op);
struct token *integer_unary_operations(const struct token *token, const char *op);

#endif // _INT_H_
