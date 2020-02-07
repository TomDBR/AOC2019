#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../UTILS/util.h"
#include "day12.h"

static int _DEBUG = 0;

struct moon_list *load_data()
{
	struct moon_list *l = malloc(sizeof(struct moon_list));
	l->moons = NULL;
	l->size = 0;
	FILE *f; int x, y, z;
	if (!( f = fopen("./input.txt", "r"))) die("Failed opening file!");
	while (fscanf(f, "<x=%d, y=%d, z=%d>\n", &x, &y, &z) == 3) 
		add_moon(l, x, y, z);
	fclose(f);
	return l;
}

struct coord *create_coord(int x, int y, int z)
{
	struct coord *coord = malloc(sizeof(struct coord));
	coord->x = x; coord->y = y; coord->z = z;
	return coord;
}

void add_moon(struct moon_list *list, int x, int y, int z) 
{
	if (_DEBUG) printf("x: %d, y: %d, z: %d, list size: %d\n", x, y, z, list->size);
	list->moons = realloc(list->moons, ++list->size * sizeof(struct moon *));
	list->moons[list->size-1] = malloc(sizeof(struct moon));
	struct moon *moon = list->moons[list->size-1];
	moon->position = create_coord(x, y, z);
	moon->velocity = create_coord(0, 0, 0);
}

void add_to_hash_string(char *str, int num) 
{
		char tmp[100]; memset(tmp, '\0', 100 * sizeof(char));
		sprintf(tmp, "%d", num);
		strcat(str, tmp);
}

// hashcode taken from http://www.cse.yorku.ca/~oz/hash.html
unsigned long hash(struct moon_list *l)
{
	char hash_string[1000]; memset(hash_string, '\0', 1000 * sizeof(char));
	for (int i = 0; i < l->size; i++) {
		struct moon *m = l->moons[i];
		add_to_hash_string(hash_string, m->position->x);
		add_to_hash_string(hash_string, m->position->y);
		add_to_hash_string(hash_string, m->position->z);
		add_to_hash_string(hash_string, m->velocity->x);
		add_to_hash_string(hash_string, m->velocity->y);
		add_to_hash_string(hash_string, m->velocity->z);
	}
	char *str = hash_string;
	unsigned long hash = 5381;
	int c;
	while ((c = *str++))
	hash = ((hash << 5) + hash) + c;
	return hash;
}

void print_moons(struct moon_list *list)
{
	struct moon **moons = list->moons;
	for (int i = 0; i < list->size; i++) {
		struct moon *m = moons[i]; struct coord *pos = m->position, *vel = m->velocity;
		printf("Moon no. %d.\t POS: (%d, %d, %d), \tVEL: (%d, %d, %d)\n", i, pos->x, pos->y, pos->z, vel->x, vel->y, vel->z);
	}
}

void apply_gravity(struct moon_list **l, int m) 
{
	for (int i = 0; i < (*l)->size; i++) {
		if (i == m) continue; 
		struct moon *first = (*l)->moons[m], *second = (*l)->moons[i];
		struct coord *pos1 = first->position, *pos2 = second->position, *vel1 = first->velocity; // *vel2 = second->velocity;
		if (_DEBUG) printf("moon BEFORE:\t POS: (%d,%d,%d),\tVEL: (%d,%d,%d)\t", pos1->x, pos1->y, pos1->z, vel1->x, vel1->y, vel1->z);
		if (_DEBUG) printf("changing its velocity for moon with pos: POS: (%d,%d,%d)\n", pos2->x, pos2->y, pos2->z);

		if 	(pos1->x < pos2->x) 	first->velocity->x += 1;
		else if (pos1->x > pos2->x) 	first->velocity->x -= 1;
		if 	(pos1->y < pos2->y) 	first->velocity->y += 1; 
		else if (pos1->y > pos2->y) 	first->velocity->y -= 1; 
		if 	(pos1->z < pos2->z) 	first->velocity->z += 1; 
		else if (pos1->z > pos2->z) 	first->velocity->z -= 1; 

		if (_DEBUG) printf("RESULT: \t POS: (%d,%d,%d),\tVEL: (%d,%d,%d)\n", pos1->x, pos1->y, pos1->z, vel1->x, vel1->y, vel1->z);
		if (_DEBUG) printf("----------------------------------------------------------------\n");
	}
}

void apply_velocity(struct moon *m) 
{
	m->position->x += m->velocity->x;
	m->position->y += m->velocity->y;
	m->position->z += m->velocity->z;
}

int calculate_energy(struct moon_list *l) 
{
	int total_energy = 0;
	for (int i = 0; i < l->size; i++) {
		struct moon *m = l->moons[i];
		int pot_energy = abs(m->position->x) + abs(m->position->y) + abs(m->position->z);
		int kin_energy = abs(m->velocity->x) + abs(m->velocity->y) + abs(m->velocity->z);
		total_energy += ( pot_energy * kin_energy );
	}
	return total_energy;
}

struct hashnode *make_hashnode(struct moon_list *l)
{
	struct hashnode *node = malloc(sizeof(struct hashnode));
	node->hash = hash(l);
	node->next = NULL;
	return node;
}

long add_node(struct hashnode *first, struct hashnode *add)
{
	if (first == NULL) die("wtf?");
	if (_DEBUG) printf("first: %p, add: %p\n", (void *) first, (void *) add);
	unsigned long iter = 0;
	while (1) {
		if (!first) break;
		if (!(first->next)) {
			if (first->hash < add->hash) {
				first->next = add;
				return -1;
			} else if(first->hash > add->hash) {
				add->next = first;
				first = add;
				return -1;
			} else return iter;
		} else {
			if (first->hash < add->hash && first->next->hash > add->hash) {
				struct hashnode *tmp = first->next;
				first->next = add;
				add->next = tmp;
				return -1;
			} 
			else if(first->hash == add->hash) { 
				if (_DEBUG) printf("first hash: %lu, add hash: %lu\n", first->hash, add->hash);
				return iter;
			}
		}
		first = first->next;
		iter++;
	}
	die("we should never get to this point");
	return -1;
}

void cleanup(struct moon_list *l)
{
	for (int i = 0; i < l->size; i++) {
		struct moon *m = l->moons[i];
		free(m->position); free(m->velocity);
		free(m);
	}
	free(l->moons);
	free(l);
}

int main() 
{
	// PART 1
	struct moon_list *moon_list = load_data();
	for (int x = 0; x < 1000; x++) {
		for (int i = 0; i < moon_list->size; i++) apply_gravity(&moon_list, i);
		for (int i = 0; i < moon_list->size; i++) apply_velocity(moon_list->moons[i]);
	}
	int energy = calculate_energy(moon_list);
	printf("System's total energy is: %d\n", energy);
	cleanup(moon_list);

	// PART 2
	moon_list = load_data();
	struct hashnode *first = make_hashnode(moon_list);
	long iter = 0, res = 0;
	while(1) {
		if (_DEBUG) printf("hashnode: %p, hash: %lu\n", (void *) first, first->hash);
		for (int i = 0; i < moon_list->size; i++) apply_gravity(&moon_list, i);
		for (int i = 0; i < moon_list->size; i++) apply_velocity(moon_list->moons[i]);
		iter++;
		struct hashnode *tmp = make_hashnode(moon_list);
		if ((res = add_node(first, tmp)) != -1) break;
	}
	printf("size was: %lu\n", iter);
	cleanup(moon_list);
	return 0;
}
