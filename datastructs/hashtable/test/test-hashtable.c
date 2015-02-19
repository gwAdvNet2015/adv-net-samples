#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "../hashtable.h"

#define BUCKET_COUNT 2

#define KEY1 "key1"
#define VAL1 2
#define KEY2 "key2"
#define VAL2 4
#define KEY3 "key3"
#define VAL3 7
#define BAD_KEY "meow"

int hash_node(char* key);
int node_compare(struct ht_node* node1, struct ht_node* node2);

void assertSimpleTable(struct ht* table);
void assertNodeEquals(struct ht_node* node1, struct ht_node* node2, struct ht_node* node3);
void assertSingleNode(struct ht* table, struct ht_node* node);
void assertLookups(struct ht* table);
void assertMultiNode(struct ht* table);

int
main()
{
        struct ht* table;
        struct ht_node *node1, *node2, *node3;

        printf("Testing hashtable...\n");

        table = ht_init_simple(BUCKET_COUNT, &hash_node, &node_compare);
        /* Assert that we've gotten memory for our table */
        printf("Checking table has been allocated... ");
        assert(table);
        printf("OK\n");

        assertSimpleTable(table);

        printf("Creating node1 with key %s and value %d...", KEY1, VAL1);
        node1 = ht_create_node(KEY1, (void*)VAL1);
        assert(node1);
        printf("OK\nCreating node2 with key %s and value %d...", KEY2, VAL2);
        node2 = ht_create_node(KEY2, (void*)VAL2);
        assert(node2);
        printf("OK\nCreating node3 with key %s and value %d...", KEY3, VAL3);
        node3 = ht_create_node(KEY3, (void*)VAL3);
        assert(node3);
        printf("OK\n");

        printf("Inserting node1 into the table...");
        ht_insert(table, node1);
        printf("OK\n");
        assertSingleNode(table, node1);
        printf("Inserting nodes 2 and 3 into the table...");
        ht_insert(table, node2);
        ht_insert(table, node3);
        printf("OK\n");
        assertNodeEquals(node1, node2, node3);
        assertLookups(table);
        assertMultiNode(table);

        printf("All hashtable tests passed\n\n");
}

void
assertMultiNode(struct ht* table)
{
        printf("Checking table size is 3...");
        assert(table->node_count == 3);
        printf("OK\nChecking that all buckets are being used...");
        assert(table->used == BUCKET_COUNT);
        printf("OK\n");
}

void
assertNodeEquals(struct ht_node* node1, struct ht_node* node2, struct ht_node* node3)
{
        printf("Checking node1 == node1...");
        assert(node_compare(node1, node1));
        printf("OK\nChecking node2 != node3...");
        assert(!node_compare(node2, node3));
        printf("OK\n");
}

void
assertLookups(struct ht* table)
{
        printf("Checking if a node with key %s exists in the table...", KEY1);
        assert(ht_lookup(table, KEY1));
        printf("OK\nChecking if node with key %s exists in the table...", KEY2);
        assert(ht_lookup(table, KEY2));
        printf("OK\nChecking if node with key %s exists in the table...", KEY3);
        assert(ht_lookup(table, KEY3));
        printf("OK\nChecking node with key %s isn't in the table...", BAD_KEY);
        assert(!ht_lookup(table, BAD_KEY));
        printf("OK\n");
}

void
assertSimpleTable(struct ht* table)
{
        printf("Checking table has 0 elements...");
        assert(table->node_count == 0);
        printf("OK\nChecking the table has %d buckets...", BUCKET_COUNT);
        assert(table->size == BUCKET_COUNT);
        printf("OK\nChecking that no buckets are in use...");
        assert(table->used == 0);
        printf("OK\nChecking largest bucket is 0...");
        assert(table->current_max == 0);
        printf("OK\nChecking bucket array has been allocated...");
        assert(table->buckets);
        printf("OK\nChecking each linkedlist has been allocated...\n");
        for(int i=0; i<BUCKET_COUNT; i++){
            printf("  %d...", i);
            assert(table->buckets[i]);
            printf("OK\n");
        }
}

void
assertSingleNode(struct ht* table, struct ht_node* node)
{
    printf("Checking table has 1 element...");
    assert(table->node_count == 1);
    printf("OK\nChecking one bucket is being used...");
    assert(table->used == 1);
    printf("OK\n");
}

int
hash_node(char* key)
{
        int sum = 0;
        char *str = key;
        while(*str) { sum += *str++; }
        return sum;
}

int
node_compare(struct ht_node* node1, struct ht_node* node2)
{
        return node1->hash == node2->hash;
}
