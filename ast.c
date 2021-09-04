#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "token.h"
#include "ast.h"
#include "utils.h"

struct node *ast_destroy(struct node *ast)
{
    if (NULL == ast)
    {
        return NULL;
    }

    if (NULL != ast->left)
    {
        ast->left = ast_destroy(ast->left);
    }

    if (NULL != ast->right)
    {
        ast->right = ast_destroy(ast->right);
    }

    if (NULL != ast->arguments)
    {
        ast->arguments = ast_destroy(ast->arguments);
    }

    ast->set = 0;
    free(ast);

    return NULL;
}

static struct node *ast_node_create()
{
    struct node *ast;

    ast = malloc(sizeof(*ast));
    if (NULL == ast)
    {
        return NULL;
    }
    memset(ast, 0, sizeof(*ast));

    ast->left = malloc(sizeof(*ast->left));
    if (NULL == ast->left)
    {
        return ast_destroy(ast);
    }
    memset(ast->left, 0, sizeof(*ast->left));

    ast->right = malloc(sizeof(*ast->right));
    if (NULL == ast->right)
    {
        return ast_destroy(ast);
    }
    memset(ast->right, 0, sizeof(*ast->right));

    ast->arguments = malloc(sizeof(*ast->arguments));
    if (NULL == ast->arguments)
    {
        return ast_destroy(ast);
    }
    memset(ast->arguments, 0, sizeof(*ast->arguments));

    ast->set = 0;
    ast->type = NULL;
    ast->next = NULL;

    return ast;
}

struct node *ast_binary_node_add(struct node *left, struct token *op, struct node *right)
{
    struct node *node;

    if (NULL == left || NULL == op || NULL == right)
    {
        return NULL;
    }

    node = ast_node_create();
    if (NULL == node)
    {
        return NULL;
    }
    node->left = left;
    node->op = op;
    node->set = 1;
    node->type = BINARY;
    node->right = right;

    return node;
}

struct node *ast_unary_node_add(struct token *op, struct node *left)
{
    struct node *node;

    if (NULL == op || NULL == left)
    {
        return NULL;
    }

    node = ast_node_create();
    if (NULL == node)
    {
        return NULL;
    }
    node->left = left;
    node->op = op;
    node->set = 1;
    node->type = UNARY;

    return node;
}

struct node *ast_value_node_set(struct token *op)
{
    struct node *node;

    if (NULL == op)
    {
        return NULL;
    }

    node = ast_node_create();
    if (NULL == node)
    {
        return NULL;
    }
    node->op = op;
    node->set = 1;
    node->type = VALUE;

    return node;
}

struct node *ast_variable_node_add(struct token *op)
{
    struct node *node;

    if (NULL == op)
    {
        return NULL;
    }

    node = ast_node_create();
    if (NULL == node)
    {
        return NULL;
    }
    node->op = op;
    node->type = VARIABLE;
    node->set = 1;

    return node;
}

struct node *ast_assignment_node_add(struct node *left, struct token *op, struct node *right)
{
    struct node *node;

    if (NULL == left || NULL == op || NULL == right)
    {
        return NULL;
    }

    node = ast_node_create();
    if (NULL == node)
    {
        return NULL;
    }
    node->left = left;
    node->op = op;
    node->right = right;
    node->type = ASSIGNMENT;
    node->set = 1;

    return node;
}

struct node *ast_declaration_node_add(struct node *left, struct token *op, struct node *right)
{
    struct node *node;

    if (NULL == left || NULL == op || NULL == right)
    {
        return NULL;
    }

    node = ast_node_create();
    if (NULL == node)
    {
        return NULL;
    }
    node->left = left;
    node->op = op;
    node->right = right;
    node->type = DECLARATION;
    node->set = 1;

    return node;
}

struct node *ast_function_node_add(struct token *name)
{
    struct node *node;

    if (NULL == name)
    {
        return NULL;
    }

    node = ast_node_create();
    if (NULL == node)
    {
        return NULL;
    }
    node->op = name;
    node->left = NULL;
    node->right = NULL;
    node->arguments = NULL;
    node->type = FUNCTION;
    node->set = 1;

    return node;
}

int ast_function_node_add_arguments(struct node *node, struct node *arguments)
{
    if (NULL == node || NULL == arguments)
    {
        return -1;
    }

    node->arguments = arguments;

    return 0;
}

int ast_function_node_add_body(struct node *node, struct node *assignment_list)
{
    if (NULL == node || NULL == assignment_list)
    {
        return -1;
    }

    node->left = assignment_list;

    return 0;
}

int ast_function_node_add_return(struct node *node, struct node *return_value)
{
    if (NULL == return_value || NULL == node)
    {
        return -1;
    }

    node->right = return_value;

    return 0;
}

struct node *ast_program_node_add(struct node *function_list)
{
    struct node *node;

    if (NULL == function_list)
    {
        return NULL;
    }

    node = ast_node_create();
    if (NULL == node)
    {
        return NULL;
    }
    node->left = function_list;
    node->type = PROGRAM;
    node->set = 1;

    return node;
}

int ast_node_append(struct node **list_head, struct node *new)
{   
    struct node *last;

    if (NULL == new)
    {
        return -1;
    }

    if (NULL == *list_head)
    {
        *list_head = new;
        return 0;
    }
    
    last = *list_head;
    while (last->next != NULL)
    {
        last = last->next;
    }

    last->next = new;
    new->next = NULL;

    return 0;
}

const struct node *ast_node_index(const struct node *list_head, int index)
{
    for (int i=0; i<index; i++)
    {
        list_head = list_head->next;
    }

    return list_head;
}

size_t ast_num_nodes(const struct node *list_head)
{
    if (NULL == list_head)
    {
        return 0;
    }

    return 1 + ast_num_nodes(list_head->next);
}

void ast_print(const struct node *ast, int level, char *location)
{
    int print_left, print_right;

    print_left = print_right = 0;
    if (NULL != ast->op)
    {
        printf("Level %d, %s, %s: %s \n", level, location, ast->type, token_get_display(ast->op));
    
        if (NULL != ast->left)
        {
            printf("Level %d, %s, %s: %s \n", level, location, ast->left->type, token_get_display(ast->left->op));
            print_left = 1;
        }
        
        if (NULL != ast->right)
        {
            printf("Level %d, %s, %s: %s \n", level, location, ast->right->type, token_get_display(ast->right->op));
            print_right = 1;
        }
        printf("\n");

        if (print_left)
        {
            ast_print(ast->left, level + 1, "left");
        }
        if (print_right)
        {
            ast_print(ast->right, level + 1, "right");
        }
    }
}
