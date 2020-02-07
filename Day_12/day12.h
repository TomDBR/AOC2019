#ifndef DAY12_H_
#define DAY12_H_

struct coord {
	int x;
	int y;
	int z;
};

struct moon {
	struct coord *position;
	struct coord *velocity;
};

struct moon_list {
	struct moon **moons;
	int size;
};

struct moon_pair {
	struct moon *first;
	struct moon *second;
};

struct hashnode {
	unsigned long hash;
	struct hashnode *next;
};


struct moon_list *load_data();
struct coord *create_coord(int x, int y, int z);
void add_moon(struct moon_list *list, int x, int y, int z);
void add_to_hash_string(char *str, int num);
unsigned long hash(struct moon_list *l);
void print_moons(struct moon_list *list);
void apply_gravity(struct moon_list **l, int m);
void apply_velocity(struct moon *m);
int calculate_energy(struct moon_list *l);
struct hashnode *make_hashnode(struct moon_list *l);
long add_node(struct hashnode *first, struct hashnode *add);
void cleanup(struct moon_list *l);
void add_moon(struct moon_list *list, int x, int y, int z);
struct moon_pair **get_pairs(struct moon_list *m, int *size);
int main();
#endif
